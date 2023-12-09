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
        perror("Can't establish that credential");
    }
}

void Cmd_uid(char* tr[]){
        
    if (tr[0] == NULL || !strcmp(tr[0],"-get")){
        uid_t real = getuid();
        uid_t effective = geteuid();

        printf("Real Credential: %d, (%s)\n", real, User(real));
        printf("Effective Credential: %d, (%s)\n", effective, User(effective));
        
    }else if (!strcmp(tr[0],"-set")){
        if (tr[1]==NULL){
            printf("Invalid argument\n");
            return;
        }
        
        if (!strcmp(tr[1],"-l")){
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
    else{
        printf("Invalid argument\n");
        return;
    }
}

int BuscarVariable (char * var, char *e[])  /*busca una variable en el entorno que se le pasa como parÃ¡metro*/
{
    int pos=0;
    char aux[MAXVAR];

    strcpy (aux,var);
    strcat (aux,"=");

    while (e[pos]!=NULL)
        if (!strncmp(e[pos],aux,strlen(aux)))
            return (pos);
        else 
            pos++;
    errno=ENOENT;   /*no hay tal variable*/
    return(-1);
}



void Cmd_showvar(char * tr[], char *envp[]){
    int varIndex;

    if (tr[0]==NULL){ 
        printf("Invalid argument\n");
        return;
    }

    varIndex = BuscarVariable(tr[0], envp);
    printf("With arg3 main: %s(%p) @%p\n", envp[varIndex], envp[varIndex], &envp[varIndex]);

    varIndex = BuscarVariable(tr[0], __environ);
    printf("With environ:   %s(%p) @%p\n", __environ[varIndex], __environ[varIndex], &__environ[varIndex]);
    
    char* var = getenv(tr[0]);
    if(var == NULL){
        perror("Variable not found");
        return;
    }
    printf("With getenv:    %s(%p)\n", var, var);
}


int CambiarVariable(char * var, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parÃ¡metro*/
{                                                        /*lo hace directamente, no usa putenv*/
  int pos;
  char *aux;
   
  if ((pos=BuscarVariable(var,e))==-1)
    return(-1);
 
  if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
	return -1;
  strcpy(aux,var);
  strcat(aux,"=");
  strcat(aux,valor);
  e[pos]=aux;
  return (pos);
}

int SubsVariable(char * var, char* var2, char * valor, char *e[]) /*cambia una variable en el entorno que se le pasa como parÃ¡metro*/
{                                                        /*lo hace directamente, no usa putenv*/
  int pos;
  char *aux;
   
  if ((pos=BuscarVariable(var,e))==-1)
    return(-1);
 
  if ((aux=(char *)malloc(strlen(var2)+strlen(valor)+2))==NULL)
	return -1;
  strcpy(aux,var2);
  strcat(aux,"=");
  strcat(aux,valor);
  e[pos]=aux;
  return (pos);
}

void Cmd_changevar(char * tr[], char *envp[]){
    if (tr[0] == NULL || tr[1]==NULL ||tr[2]==NULL){
        printf("Invalid argument\n");
        return;
    }
    if (!strcmp(tr[0], "-a")){
        CambiarVariable(tr[1], tr[2], envp);
        return;
    }
    if (!strcmp(tr[0], "-e")){
        CambiarVariable(tr[1], tr[2], __environ);
        return;
    }
    if (!strcmp(tr[0], "-p")){
        char *aux;
        if ((aux=(char *)malloc(strlen(tr[1])+strlen(tr[2])+2))==NULL){
            perror("Not enough memory");
            return;
        }
        strcpy(aux, tr[1]);
        strcat(aux, "=");
        strcat(aux, tr[2]);
        putenv(aux);
        return;
    }
}

void Cmd_subsvar(char * tr[], char *envp[]){
    if (tr[0] == NULL || tr[1]==NULL ||tr[2]==NULL || tr[3] == NULL){
        printf("Invalid argument\n");
        return;
    }
    if (!strcmp(tr[0], "-a")){
        SubsVariable(tr[1], tr[2], tr[3], envp);
        return;
    }
    if (!strcmp(tr[0], "-e")){
        SubsVariable(tr[1], tr[2], tr[3], __environ);
        return;
    }
}


void Cmd_showenv (char * tr[], char *envp[]){
    if (tr[0] == NULL){
        for(int i = 0; envp[i] != NULL; i++){
            printf("%p->main arg3[%d]=(%p) %s\n", &envp[i], i, envp[i], envp[i]); 
        }
        return;
    }
    if (!strcmp(tr[0], "-addr")){
        printf("environ: %p (stored in %p)\n", __environ, &__environ);
        printf("main arg3: %p (stored in %p)\n", envp, &envp);
        return;  
    }
    if (!strcmp(tr[0], "-environ")){
        for(int i = 0; __environ[i] != NULL; i++){
            printf("%p->environ[%d]=(%p) %s\n", &__environ[i], i, __environ[i], __environ[i]);
        }
        return;
    }
}

void Cmd_fork (char *tr[], List4* proclist){ //queda en standby hasta que tengamos lista de procesos
	pid_t pid;
	
	if ((pid=fork())==0){
		deleteStatus4(proclist, NULL);
		printf ("ejecutando proceso %d\n", getpid());
        printList4(*proclist);
	}
	else if (pid!=-1)
		waitpid (pid,NULL,0);
}

void Cmd_exec (char* tr []){
    if (execvp (tr[0], tr)==-1){
        perror("Could not execute");
        return;
    }
}

void Cmd_jobs(char* tr[], List4* proclist){
    printList4(*proclist);
    return;
}

void Cmd_deljobs(char* tr[], List4* proclist){
    if (tr[0] == NULL){ //por ahora no hace nada 
        printf("Invalid argument\n");
        return;
    }
    if (!strcmp(tr[0],"-term")){
        //char* status = malloc(9*sizeof(char));
        //strcpy(status, "FINISHED");
        deleteStatus4(proclist, "FINISHED");
        //free(status);
        return;
    }
    if (!strcmp(tr[0],"-sig")){
        deleteStatus4(proclist, "SIGNALED");
        return;
    }
}

void Cmd_job (char* tr[], List4* proclist){
    if (tr[0]==NULL){
        printf("Invalid argument\n");
        return;
    }
    if (!strcmp(tr[0], "-fg")){

    }
    else{
        int pid = atoi(tr[0]);
        printByPid(*proclist,pid);
    }
}