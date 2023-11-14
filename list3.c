//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "list3.h"

//LIST OPEN

void createList3(List3* List3){
    *List3 = NULL;
}

bool createNode3 (Pos3 *q){
    *q=malloc(sizeof(Node3));
    return *q !=NULL;
}



bool insert3(List3* List3, data3 input){
    Pos3 q;

    if (!createNode3(&q)) return false;

    q->data.address = input.address;
    q->data.size = input.size;
    q->data.timeAlloc = malloc(MAXDATE*sizeof(char));
    strcpy(q->data.timeAlloc,input.timeAlloc);
    strcpy(q->data.type, input.type);
    q->data.cl = input.cl;
    q->data.df = input.df;
    q->next=NULL;
    

    if (*List3==NULL) { //If list was empty
        *List3 = q;
    }
    else{
        q->next = *List3; //Insert at the beginning
        *List3 = q;
    }
    return true;
}


bool delete3(List3* List3, Pos3 node){
    Pos3 tmp;
    
    if(node == NULL){ //delete first
        tmp = *List3; //node we want to delete
        *List3 = tmp->next;
    }
    else if (node->next==NULL){ //node not found
        printf("Error: Element not found\n");
        return false;
    }
    else{
        tmp = node->next; //node we want to delete
        node->next = tmp->next;
    }

    if(!strcmp(tmp->data.type, "malloc")){
        free(tmp->data.address);
    }
    else if (!strcmp(tmp->data.type, "shared")){
        shmdt(tmp->data.address);
    }
    else if(!strcmp(tmp->data.type, "mapped")){
        munmap(tmp->data.address, tmp->data.size);
    }
    free(tmp->data.timeAlloc);
    free(tmp);
    return true;
}

bool deleteBySize3(List3* List3, size_t size){
    Pos3 q;
    if(!strcmp((*List3)->data.type, "malloc") && (*List3)->data.size==size){
        q = NULL;
    }
    else{
        for (q = *List3; q->next!=NULL; q=q->next){ //until points to node before node searched
            if(!strcmp(q->next->data.type, "malloc") && q->next->data.size==size) break;
        }
    }
    return delete3(List3, q);
}

bool deleteByKey3(List3* List3, key_t key){
    Pos3 q;
    if(!strcmp((*List3)->data.type, "shared") && (*List3)->data.cl==key){    printf("DF: %d\n", df);

        q = NULL;
    }
    else{
        for (q = *List3; q->next!=NULL; q=q->next){ //until points to node before node searched
            if(!strcmp(q->next->data.type, "shared") && q->next->data.cl==key) break;
        }
    }

    return delete3(List3, q);
}

bool deleteByDf3(List3* List3, int df){
    Pos3 q;
    if(!strcmp((*List3)->data.type, "mapped") && (*List3)->data.df==df){
        q = NULL;
    }
    else{
        for (q = *List3; q->next!=NULL; q=q->next){ //until points to node before node searched
            if(!strcmp(q->next->data.type, "mapped") && q->next->data.df==df) break;
        }
    }

    return delete3(List3, q);
}



void printList3(List3 List3, char * type){
    if (List3==NULL){
        return;
    }else {
        printf("%5s%22s%20s%8s", "Address", "Size", "Time allocation", "Type");
        if (!strcmp(type, "shared")) printf("%10s", "Key");
        else if (!strcmp(type, "mapped")) printf("%18s", "Descriptor");
        printf("\n");

        Pos3 q;
        for (q = List3; q != NULL; q = q->next) {
            if (!strcmp(type,q->data.type)) printf("%14p%15ld%20s%10s", q->data.address, q->data.size, q->data.timeAlloc, q->data.type); //print only of the type asked
            if (!strcmp(type, "shared")&&q->data.cl!=-1) printf("%8d", q->data.cl); //if shared also print key
            if (!strcmp(type, "mapped")&&q->data.df!=-1) printf("%8d", q->data.df); //if map also print descriptor
            if (!strcmp(type, q->data.type)) printf("\n");
        }
    }
}