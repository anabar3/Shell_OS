//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "shellfunc.h"


void Cmd_malloc (char* tr[], List3* memlist){
    if (tr[0]==NULL || (strcmp(tr[0],"-free")==0 && tr[1]==NULL)){ //if it has no size
        printf ("---------------- List of allocated blocks of memory\n");
        printList3(*memlist, "malloc");
    }
    else if (strcmp(tr[0],"-free")==0){
        size_t memory = (size_t)strtoul(tr[1], NULL, 10);
        delete3(memlist, memory); //needs size_t to find the node we need and free everything inside
    }else{
        if (atoi(tr[0])==0) {
            printf ("Can't allocate 0-byte blocks\n");
            return;
        }
        size_t memory = (size_t)atoi(tr[0]); //que pasa si aqui el atoi se le hace a -pepas
        void* ptr = malloc(memory);
        if(ptr == NULL){
            perror("Unable to allocate memory");
            return;
        }
        data3 mallocdata = {
            ptr, memory, Date(time(NULL)), "malloc", -1
        };
        insert3(memlist, mallocdata);
         printf("Allocated %ld bytes in address %p\n", memory, ptr);
    } 
}


void createShared(key_t clave, size_t tam, List3* memlist) {
    void * p;
    int aux,id,flags=0777;
    struct shmid_ds s;

    //GET VALUES
    if (tam)     /*tam distito de 0 indica crear */
        flags=flags | IPC_CREAT | IPC_EXCL;
    if (clave==IPC_PRIVATE)  /*no nos vale*/
        {errno=EINVAL; return NULL;}
    if ((id=shmget(clave, tam, flags))==-1)
        return (NULL);
    if ((p=shmat(id,NULL,0))==(void*) -1){
        aux=errno;
        if (tam)
            shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    shmctl (id,IPC_STAT,&s);

    if (p!=NULL){
    //UPDATE LIST
        data3 shareddata = {
            p, s.shm_segsz, Date(time(NULL)), "shared", clave
        };
        insert3(memlist, shareddata);
        
        printf ("Allocated %lu bytes in address %p\n",(unsigned long) tam, p);
    } else{
        printf ("Impossible to allocate shared memory with key %lu:%s\n",(unsigned long) clave, strerror(errno));
    }
}

void Cmd_shared (char *tr[], List3* memlist){
   key_t cl;
   size_t tam;
   void *p;

   if (tr[0]==NULL || tr[1]==NULL) {
		printList3(memlist, "shared");
		return;
   }

   cl=(key_t)  strtoul(tr[0],NULL,10);
   
  
    if (strcmp(tr[0],"-free")==0){   //FREE
        //cosas del free

    }else if (strcmp(tr[0],"-create")==0){      //CREATE
        tam=(size_t) strtoul(tr[1],NULL,10);

        if (tam==0) {
            printf ("Can't allocate 0-byte blocks\n");
            return;
        }
        createShared(cl, tam, memlist);

    }else if (strcmp(tr[0], "-delkey")==0){   //DELKEY

        //cosas del delkey
        
    }else{
        printf("Invalid argument\n");
        return;
    } 

   
   
}


