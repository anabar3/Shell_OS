//AUTHORS - Group 6.1 - 14/12/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "shellfunc.h"


uid_t Name(char* name){ 
    struct passwd *pws;
    if ((pws = getpwnam(name)) == NULL) return -1;
    return pws->pw_uid;
}

void Do_setUid(uid_t newUid){
    if (setuid(newUid) == 0) {
        printf("Established real credential to: %d, (%s)\n", newUid, User(newUid));
        printf("Established effective credential to: %d, (%s)\n", newUid, User(newUid));
    } else {
        perror("Can't establish that credential\n");
    }
}

void Cmd_uid(char* tr[]){
        
    if (!strcmp(tr[0],"-get")|| tr[0] == NULL){
        uid_t real = getuid();
        uid_t effective = geteuid();

        printf("Real Credential: %d, (%s)\n", real, User(real));
        printf("Effective Credential: %d, (%s)\n", effective, User(effective));
        
    }else if (!strcmp(tr[0],"-set")){
        if (tr[1]==NULL){
            printf("Invalid argument\n");
            return;
        }else if (strcmp(tr[1],"-l")){
            if (tr[2]==NULL){
                printf("Invalid argument\n");
                return;
            }
            uid_t newUid;
            if ((newUid = Name(tr[2])) == -1){
                printf("Can't find that user\n");
                return;
            }else{
                Do_setUid(newUid);
            }
        
        }else{
            Do_setUid(atoi(tr[1]));
        }
    }
}