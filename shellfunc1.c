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

char * ConvierteModo (mode_t m, char *permisos){
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

void Cmd_stat (char* tr[]){
    if (tr[0]==NULL) { //No arguments
        Cmd_chdir(tr);
    }
    else{
        int i=0;
        struct stat statbuf;
        bool statlong =false, acc =false, link=false;

        for (; tr[i]!=NULL;i++){ //Check arguments
            if (!strcmp(tr[i], "-long")){
                statlong=true;
            }else if (!strcmp(tr[i], "-acc")){
                acc=true;
            }else if (!strcmp(tr[i], "-link")){
                link =true;
            }else break;
        }

        for (;tr[i]!=NULL;i++){ //Check names of files

            if (lstat(tr[i], &statbuf)==-1){
                perror("Impossible to access file or directory");
            } else{ 
                if (statlong){
                    //PERMISSIONS
                    char permissions[12];
                    ConvierteModo(statbuf.st_mode, permissions);
                    
                    //DATES
                    char date[200];
                    struct tm tm;
                    if(acc){
                        tm = *localtime (&statbuf.st_atime);
                    }
                    else{
                        tm= *localtime (&statbuf.st_ctime);
                    }
                    strftime(date, sizeof(date), "%d/%m/%Y %H:%M", &tm);

                    //OWNERSHIPS
                    struct passwd *pws;
                    pws = getpwuid(statbuf.st_uid);
                    struct group *grs;
                    grs = getgrgid(statbuf.st_gid);

                    //LINK
                    char linkbuf[204] = "\0"; // Increase the size to accommodate the concatenated link target
                    if (link) { 
                        char buff[200];
                        if (readlink(tr[i], buff, 200) == -1) {
                            perror("Error reading the route");
                        } else {
                            strcpy(linkbuf, "-> ");
                            strcat(linkbuf, buff);
                        }
                    }

                    //PRINT
                    printf ("1. Last status change   2. Link count   3. (I-node number)   4. Ownership    5. File size    6. Mode\n");
                    
                    printf ("%s  %ld (%ld) %s %s %lld  %s %s", date, (long) statbuf.st_nlink, (long) statbuf.st_ino, pws->pw_name, grs->gr_name, (long long) statbuf.st_size, permissions, linkbuf);
                }
            }
        }
    }
}

void Cmd_list (char* tr[]){
    
}

void Cmd_delete (char* tr[]){

}

void Cmd_deltree (char* tr[]){
    
}


