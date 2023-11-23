//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "shellfunc.h"


void Cmd_malloc (char* tr[], List3* memlist){
    if (tr[0]==NULL || (!strcmp(tr[0],"-free") && tr[1]==NULL)){ //if it has no size
        printf ("---------------- List of allocated blocks of memory for process pid %d\n", getpid());
        printList3(*memlist, "malloc");
    }
    else if (!strcmp(tr[0],"-free")){
        
        size_t memory = (size_t)strtoul(tr[1], NULL, 10);
        if (memory == 0){ //if it tries to free a string or something
            printf("Invalid argument\n");
            return;
        }
        if(*memlist == NULL){ //if the list is empty
            printf("Error: No assigned memory blocks to free\n");
            return;
        }

        if (!deleteBySize3(memlist, memory)) printf("Can't free that block of memory\n"); //it frees inside the delete function itself
        
    }else{
        if (atoi(tr[0])==0) { //if it tries to allocate 0 bytes or receives a string
            printf ("Invalid argument\n");
            return;
        }
        size_t memory = (size_t)atoi(tr[0]); 
        void* ptr = malloc(memory);
        if(ptr == NULL){
            perror("Unable to allocate memory");
            return;
        }
        data3 mallocdata = {
            ptr, memory, Date(time(NULL)), "malloc", -1, -1
        };
        if(!insert3(memlist, mallocdata)){
            printf("Can't insert in the memory block list");
            return;
        }
        printf("Allocated %ld bytes in address %p\n", memory, ptr);
    } 
}


void SharedCreate(key_t clave, size_t tam, List3* memlist) {
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    //GET VALUES
    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL;
    if (clave==IPC_PRIVATE){  /*no nos vale*/
        errno=EINVAL;
        perror("Could not create shared memory");
        return;
    }
    if ((id=shmget(clave, tam, flags))==-1){
        perror("Could not create shared memory");
        return;
    }
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return;
    }
    shmctl (id,IPC_STAT,&s);

    //UPDATE LIST
    if (p!=NULL){
        data3 shareddata = {
            p, s.shm_segsz, Date(time(NULL)), "shared", clave, -1
        };
        if(!insert3(memlist, shareddata)){
            printf("Can't insert in the memory block list\n");
            return;
        } 

        printf ("Allocated %lu bytes in address %p\n",(unsigned long) tam, p);
    } else{
        printf ("Impossible to allocate shared memory with key %lu:%s\n",(unsigned long) clave, strerror(errno));
    }
}

void SharedDelkey (key_t clave){
   int id;

   if (clave==IPC_PRIVATE){
        errno=EINVAL;
        perror("Could not delete key");
        return;
   }
   if ((id=shmget(clave,0,0666))==-1){
        perror ("shmget: impossible to obtain shared memory");
        return;
   }
   if (shmctl(id,IPC_RMID,NULL)==-1)
        perror ("shmctl: impossible to delete shared memory id\n");
}



void Cmd_shared (char *tr[], List3* memlist){
    key_t cl;
    size_t tam;

    if (tr[0]==NULL || tr[1]==NULL) {
        printf ("---------------- List of shared blocks of memory for process pid %d\n", getpid());
        printList3(*memlist, "shared");
        return;
    }

//FREE
    if (strcmp(tr[0],"-free")==0){   
        cl = (key_t) strtoul(tr[1],NULL,10);

        if (cl == 0){ //if it tries to free a string or something
            printf("Invalid argument\n");
            return;
        }

        if(*memlist == NULL){ //if the list is empty
            printf("Error: No assigned memory blocks to free\n");
        }
        if (!deleteByKey3(memlist, cl)) printf("Can't free that block of memory\n"); //it does the shmdt inside


//CREATE

    }else if (strcmp(tr[0],"-create")==0){   
        if (tr[1]==NULL || tr[2] == NULL){
            printf("Invalid argument\n");
            return;
        }

        cl = (key_t) strtoul(tr[1],NULL,10);
        tam = (size_t) strtoul(tr[2],NULL,10);

        if (cl == 0 || tam ==0){ //if it tries to free a string or something
            printf("Invalid argument\n");
            return;
        }

        SharedCreate(cl, tam, memlist);

//DELKEY
    }else if (strcmp(tr[0], "-delkey")==0){ 
        cl = (key_t) strtoul(tr[1],NULL,10);

        if (cl == 0){ //if it tries to free a string or something
            printf("Invalid argument\n");
            return;
        }

        SharedDelkey(cl);        
    }
    
    else{
        printf("Invalid argument\n");
        return;
    } 
}

void * Mapfile (char * fichero, int protection, List3* memlist){
    int df, map=MAP_PRIVATE,modo=O_RDONLY;
    struct stat s;
    void *p;

    if (protection&PROT_WRITE)
          modo=O_RDWR;
    if (stat(fichero,&s)==-1 || (df=open(fichero, modo))==-1)
          return NULL;
    if ((p=mmap (NULL,s.st_size, protection,map,df,0))==MAP_FAILED)
           return NULL;
    
    close(df);
    
    data3 mapdata = {
        p, s.st_size, Date(time(NULL)), "mapped", -1, df
    };
    
    if (!insert3(memlist,mapdata)) printf("Can't insert in the memory block list\n");
    return p;
}

void Cmd_mmap (char *tr[], List3* memlist){
    char *perm;
    void *p;
    int protection=0;
    
    if (tr[0]==NULL){
        printf ("---------------- List of assignated mmap blocks of memory for process pid %d\n", getpid());
        printList3(*memlist, "mapped"); 
        return;
    }

    if (!strcmp(tr[0],"-free")){
        if (tr[1]==NULL){
            printf("Invalid argument");
            return;
        }
        int df;
        if(*memlist == NULL){ //if the list is empty
            printf("Error: No assigned memory blocks to free\n");
            return;
        }
        
        if((df=open(tr[1], O_RDWR))==-1)
            return;
        
        if(!deleteByDf3(memlist, df)) printf("Can't free that block of memory\n");

    }else {
        
        if ((perm=tr[1])!=NULL && strlen(perm)<4) {
            if (strchr(perm,'r')!=NULL) protection|=PROT_READ;
            if (strchr(perm,'w')!=NULL) protection|=PROT_WRITE;
            if (strchr(perm,'x')!=NULL) protection|=PROT_EXEC;
        }
        if ((p=Mapfile(tr[0],protection, memlist))==NULL) perror ("Impossible to map file");

        else printf ("File %s mapped in %p\n", tr[0], p);
    }
}


ssize_t ReadFile (char *f, void *p, size_t cont){
    struct stat s;
    ssize_t n;  
    int df,aux;

    if (stat (f,&s)==-1 || (df=open(f,O_RDONLY))==-1)
        return -1;     
    if (cont==-1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=s.st_size;
    if ((n=read(df,p,cont))==-1){
        aux=errno; //save the errno to restore later in case close fails
        close(df);
        errno=aux;
        return -1;
    }
    close (df);
    return n;
}

void * strtop(char * string){
    unsigned long pointer = strtoul(string, NULL, 16);
    if (pointer==0)  return NULL;
    return (void *)pointer;
}


void Cmd_read (char *tr[]){ //take contents of file and put them in address in memory (cont bytes)
    void *p;
    size_t cont=-1;  /* -1 indica leer todo el fichero*/
    ssize_t n;
    if (tr[0]==NULL || tr[1]==NULL){
        printf ("Not enough parameters\n");
        return;
    }

    if((p = strtop(tr[1])) == NULL){ /*convertimos de cadena a puntero*/
        printf("Invalid address\n");
        return;
    }  

    if (tr[2]!=NULL)
        cont=(size_t) atoll(tr[2]); //if a cont was specified, replace variable cont

    if ((n=ReadFile(tr[0],p,cont))==-1)
        perror ("Impossible to read file");
    else
        printf ("Read %lld bytes of %s in %p\n",(long long) n,tr[0],p);
}


ssize_t WriteFile (char *f, void *p, size_t cont, int overwrite){
    ssize_t  n;
    int df,aux, flags = O_CREAT | O_WRONLY;

    if (overwrite)
        flags |= O_TRUNC;
    else
        flags |= O_APPEND;
    
    if ((df=open(f,flags,0777))==-1)
        return -1;
    
    if ((n=write(df,p,cont))==-1){
        aux=errno;
        close(df);
        errno=aux;
        return -1;
    }

    close (df);
    return n;
}


void Cmd_write (char *tr[]){
    void *p;
    size_t cont;  
    ssize_t n;
    int overwrite=0;

    if (!strcmp(tr[0], "-o")){
        overwrite = true;
    }

    if (tr[0]==NULL || tr[1]==NULL || tr[2]==NULL || (overwrite && tr[3]==NULL)){
        printf ("Not enough parameters\n");
        return;
    }


    if((p = strtop(tr[1+overwrite])) == NULL){ //overwrite will add 1 when there is an extra argument (-o) so everything will be displaced
        printf("Invalid address\n");
        return;
    }  

    cont=(size_t) atoll(tr[2+overwrite]); //if a cont was specified, replace variable cont
    if(cont == 0){
        printf("Invalid argument\n");
        return;
    }

    if ((n=WriteFile(tr[0+overwrite],p,cont, overwrite))==-1)
        perror ("Impossible to write file");
    else
        printf ("Written %lld bytes of %p in %s\n",(long long) n,p,tr[0+overwrite]);
}

void Cmd_memdump(char *tr[]){
    void *p;
    size_t cont;
    if (tr[0]==NULL || tr[1]==NULL){
        printf ("Not enough parameters\n");
        return;
    }
    
    if((p = strtop(tr[0])) == NULL){ //convertimos de cadena a puntero
        printf("Invalid address\n");
        return;
    }
    
    cont=(size_t) atoll(tr[1]);

    void* p2 = p;
    for(int j = 0; j < cont; j += 25){

        for(int i = 1; i <= 25; i++){
            if(j+i == cont) break;

            printf("%2c ", *((char*)p));
            p += 1;
        }
        printf("\n");
        for(int i = 1; i <= 25; i++){
            if(j+i == cont) break;

            printf("%2x ", *((char*)p2));
            p2 += 1;
        }
        printf("\n\n");
    }
}


void FillMemory (void *p, size_t cont, unsigned char byte)
{
  unsigned char *arr = (unsigned char *)p;
  size_t i;

  for (i=0; i<cont;i++)
		arr[i]=byte;
}

void Cmd_memfill(char *tr[]){
    void *p;
    size_t cont;
    int c;

    if (tr[0]==NULL || tr[1]==NULL){
        printf ("Not enough parameters\n");
        return;
    }
    if((p = strtop(tr[0])) == NULL){ //convertimos de cadena a puntero
        printf("Invalid address\n");
        return;
    }
    
    cont=(size_t) atoll(tr[1]);

    c = atoi(tr[2]);

    printf("Filling %ld bytes of memory from adress %p with byte %c(%x)\n", cont, p, c, c);
    FillMemory(p, cont, c);
}

void Recursiva(int n)
{
  char automatico[4096];
  static char estatico[4096];

  printf ("parameter:%3d(%p) array %p, static array %p\n",n,&n,automatico, estatico);

  if (n>0)
    Recursiva(n-1);
}

void Cmd_recurse(char *tr[]){
    int n;
    if(tr[0] == NULL || (n = atoi(tr[0])) == 0){
        printf("Invalid argument\n");
        return;
    }
    Recursiva(n);
}

void Do_MemPmap (void){ //sin argumentos
   pid_t pid;       //hace el pmap (o equivalente) del proceso actual
   char elpid[32];
   char *argv[4]={"pmap",elpid,NULL};
   
   sprintf (elpid,"%d", (int) getpid());
   if ((pid=fork())==-1){
      perror ("Imposible crear proceso");
      return;
      }
   if (pid==0){ //proceso hijo
      if (execvp(argv[0],argv)==-1)
         perror("cannot execute pmap (linux, solaris)");
      
      argv[0]="vmmap"; argv[1]="-interleave"; argv[2]=elpid;argv[3]=NULL;
      if (execvp(argv[0],argv)==-1) //probamos vmmap Mac-OS
         perror("cannot execute vmmap (Mac-OS)");          
      
      argv[0]="procstat"; argv[1]="vm"; argv[2]=elpid; argv[3]=NULL;   
      if (execvp(argv[0],argv)==-1) //No hay pmap, probamos procstat FreeBSD
         perror("cannot execute procstat (FreeBSD)");
         
      argv[0]="procmap",argv[1]=elpid;argv[2]=NULL;    
            if (execvp(argv[0],argv)==-1)  //probamos procmap OpenBSD
         perror("cannot execute procmap (OpenBSD)");
         
      exit(1);
  }
  waitpid (pid,NULL,0);
}

int global1 = 1, global2 = 2, global3 = 3;
int niglobal1, niglobal2, niglobal3;

void printVars(){
    int local1, local2, local3;
    static int stat1 = 1, stat2 = 2, stat3 = 3;
    static int nistat1, nistat2, nistat3;

    printf("%-18s%18p,%18p,%18p\n", "Local variables", &local1, &local2, &local3);
    printf("%-18s%18p,%18p,%18p\n", "Global variables", &global1, &global2, &global3);
    printf("%-18s%18p,%18p,%18p\n", "Global (N.I.) vars", &niglobal1, &niglobal2, &niglobal3);
    printf("%-18s%18p,%18p,%18p\n", "Static variables", &stat1, &stat2, &stat3);
    printf("%-18s%18p,%18p,%18p\n", "Static (N.I.) vars", &nistat1, &nistat2, &nistat3);
}

void printFuncs(){
    printf("%-18s%18p,%18p,%18p\n", "Program functions", &Cmd_pid, &Cmd_mem, &printFuncs);
    printf("%-18s%18p,%18p,%18p\n", "Library functions", &printf, &malloc, &strcmp);
}

void Cmd_mem(char *tr[], List3* memlist){
    if(tr[0] == NULL || !strcmp(tr[0], "-all")){
        printVars();
        printFuncs();
        printf ("---------------- List of assigned blocks of memory for process pid %d\n", getpid());
        printAllList3(*memlist);
    }
    else if(!strcmp(tr[0], "-blocks")){
        printf ("---------------- List of assigned blocks of memory for process pid %d\n", getpid());
        printAllList3(*memlist);
    }
    else if(!strcmp(tr[0], "-funcs")){
        printFuncs();
    }
    else if(!strcmp(tr[0], "-vars")){
        printVars();
    }
    else if(!strcmp(tr[0], "-pmap")){
        Do_MemPmap();
    }
    else{
        printf("Invalid argument\n");
        return;
    }

}

