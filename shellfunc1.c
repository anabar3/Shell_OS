//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

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


void Do_stat (char *name, bool statlong, bool acc, bool link ){
    struct stat s;
    time_t filetime;
    char buff [MAXFILENAME]; //for links
    int nlink;

    if (lstat(name, &s) == -1){ //save stats to s
        printf("Impossible to access %s: %s\n", name, strerror(errno));
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
    if (statlong) printf("Showing ... Date,  nº of links,  Inode nº,  User,  Group,  Size,  Permissions,  Name  [-> Link]\n\n");
    else printf("Showing ... Size, Name\n\n");
    for (;tr[i]!=NULL; i++) //go through the files
        Do_stat(tr[i], statlong, acc, link);
}

int isDirectoryEmpty(char *dirname) {
    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        perror("Unable to open directory");
        return -1;
    }

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        count++;;
        if(count > 2) break;
    }
    closedir(dir);
    return count <= 2;
}

int isDirectory(char* name){
    struct stat s;
    if(lstat(name, &s) == -1){
        perror("Unable to access element");
        return -1;
    }

    return S_ISDIR(s.st_mode);
}

void Do_list(DIR *dirstream, char* dirname, bool statlong, bool acc, bool link, bool hid){
    struct dirent *nextdir;
    printf("*************%s\n", dirname);
    
    while ((nextdir = readdir(dirstream)) != NULL) { //go through all files, if it isn't hid and the file begins with '.', skip it
        if (hid || nextdir->d_name[0] != '.') {
            Do_stat(nextdir->d_name, statlong, acc, link);
        }
    }
}


void Do_list_reca(DIR *dirstream, char* dirname, bool statlong, bool acc, bool link, bool hid){
    struct dirent *nextdir;
    DIR *subdirstream;
    char pathcopy[MAXDIRNAME];

    while ((nextdir = readdir(dirstream)) != NULL){ //list all elements first
        if(!strcmp(nextdir->d_name, "..") || !strcmp(nextdir->d_name, ".")) continue;

        if(!hid && nextdir->d_name[0] == '.') continue;

        Do_stat(nextdir->d_name, statlong, acc, link);
    }

    rewinddir(dirstream); // reset dirstream to the beginning

    while ((nextdir = readdir(dirstream)) != NULL){ // recursion when directory
        if(!strcmp(nextdir->d_name, "..") || !strcmp(nextdir->d_name, ".")) continue;

        if(!hid && nextdir->d_name[0] == '.') continue;

        if(!isDirectory(nextdir->d_name)) continue;

        printf("***********%s/%s\n", dirname, nextdir->d_name);
        if(!isDirectoryEmpty(nextdir->d_name)){
            if((subdirstream = opendir(nextdir->d_name)) == NULL){
                perror("Unable to open directory");
                continue;
            }
            strcpy(pathcopy,dirname); //for restoring path of previous directory
            strcat(pathcopy, "/");
            strcat(pathcopy, nextdir->d_name);
            if (!Do_chdir(nextdir->d_name)) continue;
            Do_list_reca(subdirstream, pathcopy, statlong, acc, link, hid);
            if (!Do_chdir("..")) return;
            closedir(subdirstream);

        }
    }
}


void Do_list_recb(DIR *dirstream, char* dirname, bool statlong, bool acc, bool link, bool hid){
    struct dirent *nextdir;
    char pathcopy[MAXFILENAME];
    DIR *subdirstream;

    while ((nextdir = readdir(dirstream)) != NULL){ // recursion when directory
        if(!strcmp(nextdir->d_name, "..") || !strcmp(nextdir->d_name, ".")) continue;

        if(!hid && nextdir->d_name[0] == '.') continue;
        
        if(!isDirectory(nextdir->d_name)) continue;
        
        if(!isDirectoryEmpty(nextdir->d_name)){
            if((subdirstream = opendir(nextdir->d_name)) == NULL){
                perror("Unable to open directory");
                continue;
            }
            strcpy(pathcopy, dirname);
            strcat(pathcopy, "/");
            strcat(pathcopy, nextdir->d_name);
            if (!Do_chdir(nextdir->d_name)) continue;
            Do_list_recb(subdirstream, pathcopy, statlong, acc, link, hid);
            if (!Do_chdir("..")) return;
            closedir(subdirstream);
        }
        else{ //in case dir is empty
            printf("******************%s/%s\n", dirname, nextdir->d_name);
        }
    }
    printf("******************%s\n", dirname);

    rewinddir(dirstream); // reset dirstream to the beginning

    while ((nextdir = readdir(dirstream)) != NULL){ //list all elements first
        if(!strcmp(nextdir->d_name, "..") || !strcmp(nextdir->d_name, ".")) continue;

        if(!hid && nextdir->d_name[0] == '.') continue;

        Do_stat(nextdir->d_name, statlong, acc, link);
    }
}

void Cmd_list (char* tr[]){
    int i = 0;
    bool statlong = false, acc = false, link = false, reca=false, recb=false, hid=false;
    DIR *dirstream; 
    char actualpath[MAXDIRNAME]; 

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

        //If it is not a directory, just print stats of file
        if (!isDirectory(tr[i])){
            Do_stat(tr[i], statlong, acc, link);
            continue;
        }
    
        if (getcwd(actualpath, sizeof(actualpath)) == NULL) //gets current directory and saves it in actualpath
            perror("Impossible to show directory");
        if (!Do_chdir(tr[i])) continue;
        
        if((dirstream = opendir(".")) == NULL){
            perror("Unable to open directory");
            continue;
        }

        if (!reca && !recb) Do_list(dirstream, tr[i], statlong, acc, link, hid);
        else if (recb) Do_list_recb(dirstream, tr[i], statlong, acc, link, hid);
        else if (reca) Do_list_reca(dirstream, tr[i], statlong, acc, link, hid);
        Do_chdir(actualpath);
        closedir(dirstream);
    }
}

void Do_delete(char* name){
    if (remove(name)!=0){
        perror ("File can't be deleted");
    }
}

void Cmd_delete (char* tr[]){
    int i;
    for (i=0;tr[i]!=NULL;i++){
        Do_delete(tr[i]);
    }
}

void Do_deltree (char* name){
    DIR *dirstream;
    struct dirent *nextdir;
    struct stat s;
    
    //Cambiar directorio al que hay que borrar
    if (!Do_chdir(name)) return;

    if(!isDirectoryEmpty(".")){
        if((dirstream = opendir(".")) == NULL){
            perror("Unable to open directory");
            return;
        }

        while ((nextdir = readdir(dirstream)) != NULL){ 
            if(!strcmp(nextdir->d_name, "..") || !strcmp(nextdir->d_name, ".")){
                continue;
            }

            if(lstat(nextdir->d_name, &s) == -1){
                perror("Unable to access element");
                continue;
            }

            if(S_ISDIR(s.st_mode)){
                Do_deltree(nextdir->d_name);        
            }
            else{
                Do_delete(nextdir->d_name);
            }
        }
        closedir(dirstream);
    }

    if (!Do_chdir("..")) return;
    
    Do_delete(name);
    return;
}

void Cmd_deltree(char *tr[]){
    int i;
    for (i=0; tr[i]!=NULL;i++){
        Do_deltree(tr[i]);
    }
}

        




