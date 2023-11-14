//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "shellfunc.h"


void Cmd_malloc (char* tr[], List3* memlist){
    if (tr[0]==NULL || (!strcmp(tr[0],"-free") && tr[1]==NULL)){ //if it has no size
        printf ("---------------- List of allocated blocks of memory\n");
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
        printf ("---------------- List of shared blocks of memory\n");
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
        printf ("---------------- List of assignated mmap blocks of memory\n");
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

/*
void Do_MemPmap (void) //sin argumentos
 { pid_t pid;       //hace el pmap (o equivalente) del proceso actual
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
}*/



