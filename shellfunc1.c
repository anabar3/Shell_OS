#include "shellfunc.h"

void Cmd_create (char* tr[]){
    if (tr[0]==NULL) { //No arguments
        Cmd_chdir(tr);
    }
    else if (!strcmp(tr[0], "-f")){
        if(creat(tr[1], 00777) == -1){
            perror ("Impossible to create file");
        }
    }
    else{
        if (mkdir(tr[0], 00777)== -1){
            perror ("Impossible to create directory");
        }
    }
}


char LetraTF (mode_t m){
     switch (m&__S_IFMT) { /*and bit a bit con los bits de formato,0170000 */
        case __S_IFSOCK: return 's'; /*socket */
        case __S_IFLNK: return 'l'; /*symbolic link*/
        case __S_IFREG: return '-'; /* fichero normal*/
        case __S_IFBLK: return 'b'; /*block device*/
        case __S_IFDIR: return 'd'; /*directorio */ 
        case __S_IFCHR: return 'c'; /*char device*/
        case __S_IFIFO: return 'p'; /*pipe*/
        default: return '?'; /*desconocido, no deberia aparecer*/
     }
}

char * ConvierteModo (mode_t m){
    static char permisos[12];
    strcpy (permisos,"---------- ");
    
    permisos[0]=LetraTF(m);
    if (m&S_IRUSR) permisos[1]='r';    /*propietario*/
    if (m&S_IWUSR) permisos[2]='w';
    if (m&S_IXUSR) permisos[3]='x';
    if (m&S_IRGRP) permisos[4]='r';    /*grupo*/
    if (m&S_IWGRP) permisos[5]='w';
    if (m&S_IXGRP) permisos[6]='x';
    if (m&S_IROTH) permisos[7]='r';    /*resto*/
    if (m&S_IWOTH) permisos[8]='w';
    if (m&S_IXOTH) permisos[9]='x';
    if (m&S_ISUID) permisos[3]='s';    /*setuid, setgid y stickybit*/
    if (m&S_ISGID) permisos[6]='s';
    if (m&__S_ISVTX) permisos[9]='t';
    
    return permisos;
}

char* Date(time_t filedate){   
    static char date[MAXDATE];
    struct tm tm = *localtime(&filedate);
    strftime(date,MAXDATE, "%d/%m/%Y-%H:%M", &tm);
    return date;
}

char* User(uid_t uid){ 
    struct passwd *pws;
    if ((pws = getpwuid(uid)) == NULL) return "???";
    return pws->pw_name;
}

char* Group(gid_t gid){
    struct group *grs;
    if ((grs = getgrgid(gid)) == NULL) return "???";
    return grs->gr_name;
}


void DoStat (char *name, bool statlong, bool acc, bool link ){
    struct stat s;
    time_t filetime;
    char buff [MAXFILENAME]; //for links
    int nlink;

    if (lstat(name, &s) == -1){ //save stats to s
        printf("Impossible to access %s: %s", name, strerror(errno));
        return;
    }
    if (!statlong){
        printf("%10lld  %s\n", (long long)s.st_size, name);
        return;
    }

    filetime = acc ? s.st_atime : s.st_mtime; //if acc, filetime = atime, else mtime

    //PRINT
    printf("%s %4ld %10ld  %10s %10s %15lld %s %s", 
            Date(filetime), s.st_nlink, s.st_ino, User(s.st_uid), Group(s.st_gid), (long long)s.st_size, ConvierteModo(s.st_mode), name);

    //LINK
    if (link && S_ISLNK(s.st_mode)) {  //only if it has a link
        printf(" -> ");
        if ((nlink = readlink(name, buff, MAXFILENAME)) != -1) {
            buff[nlink] = '\0';
            printf ("%s", buff);
        }
        else{
            printf("Error reading the link: %s", strerror(errno));
        }
    }
    printf ("\n");
}

void Cmd_stat (char* tr[]){
    int i = 0;
    bool statlong = false, acc = false, link = false;

    if (tr[0]==NULL) { //No arguments
        Cmd_chdir(tr);
        return;
    }
    for (; tr[i]!=NULL;i++){ //Check arguments
        if (!strcmp(tr[i], "-long")){
            statlong = true;
        }else if (!strcmp(tr[i], "-acc")){
            acc = true;
        }else if (!strcmp(tr[i], "-link")){
            link = true;
        }else break;
    }
    printf("( Date,  nº of links,  Inode nº,  User,  Group,  Size,  Permissions,  Name  [-> Link] )\n\n");
    for (;tr[i]!=NULL; i++) //go through the files
        DoStat(tr[i], statlong, acc, link);
}


void DoList(DIR *dirstream, char* dirname, bool statlong, bool acc, bool link, bool hid){
    struct dirent *nextdir;
    printf("**********%s\n", dirname);
    
    while ((nextdir = readdir(dirstream)) != NULL) { //go through all files, if it isn't hid and the file begins with '.', skip it
        if (hid || nextdir->d_name[0] != '.') {
            DoStat(nextdir->d_name, statlong, acc, link);
        }
    }
}

void DoListReca(DIR *dirstream, bool statlong, bool acc, bool link, bool hid){
    struct dirent *nextdir;
    struct stat s;
    
    if((nextdir = readdir(dirstream)) == NULL){
        return;
    }

    DoStat(nextdir->d_name, statlong, acc, link);

    lstat(nextdir->d_name, &s);    //If it is not a directory, just print stats of file
    if (S_ISDIR(s.st_mode)){
        DoListReca(dirstream, statlong, acc, link, hid);
    }
}

void DoListRecb(DIR *dirstream, bool statlong, bool acc, bool link, bool hid){
    struct dirent *nextdir;
    struct stat s;
    
    while ((nextdir = readdir(dirstream)) != NULL){
        lstat(nextdir->d_name,&s);
        
        if (S_ISDIR(s.st_mode)){
            printf("*********%s\n", nextdir->d_name);
            if (hid || nextdir->d_name[0] != '.') 
                DoListRecb (dirstream, statlong, acc, link, hid);
        }
        DoStat (nextdir->d_name, statlong, acc, link);
    }
    
}


void Cmd_list (char* tr[]){
    int i = 0;
    bool statlong = false, acc = false, link = false, reca=false, recb=false, hid=false;
    struct stat s;
    DIR *dirstream;  

    if (tr[0]==NULL) { //No arguments
        Cmd_chdir(tr);
        return;
    }
    for (; tr[i]!=NULL;i++){ //Check arguments
        if (!strcmp(tr[i], "-long")){
            statlong = true;
        }else if (!strcmp(tr[i], "-acc")){
            acc = true;
        }else if (!strcmp(tr[i], "-link")){
            link = true;
        }else if (!strcmp(tr[i], "-hid")){
            hid = true;
        }else if (!strcmp(tr[i], "-reca")){
            reca = true;
        }else if (!strcmp(tr[i], "-recb")){
            recb = true;
        }else break;
    }

    for (;tr[i]!=NULL; i++){ //go through the directories

        lstat(tr[i], &s);    //If it is not a directory, just print stats of file
        if (S_ISDIR(s.st_mode)== 0){
            DoStat(tr[i], statlong, acc, link);
            break;
        }

        if((dirstream = opendir(tr[i])) == NULL){
            perror("Unable to open directory");
            break;
        }

        if (!reca && !recb) DoList(dirstream, tr[0], statlong, acc, link, hid);
        else if (reca) DoListReca(dirstream, statlong, acc, link, hid);
        else if (recb) DoListRecb(dirstream, statlong, acc, link, hid);
    }
}

void Cmd_delete (char* tr[]){

}

void Cmd_deltree (char* tr[]){
    
}


