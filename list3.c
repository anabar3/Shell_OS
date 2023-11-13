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
    strcpy(q->data.timeAlloc ,input.timeAlloc);
    strcpy(q->data.type, input.type);
    q->data.cl = input.cl;
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

Pos3 find3 (List3 List3, size_t size){
    Pos3 q;
    for (q = List3; q->next!=NULL && q->next->data.size!=size; q=q->next){ //until points to node before node searched
        if(strcmp(q->data.type, "malloc")) continue;
    } 
    return q;
}


bool delete3(List3* List3, size_t size){
    if((*List3)->data.size == size){
        Pos3 tmp;
        tmp= *List3;
        *List3 = tmp->next;
        free(tmp->data.address);
        free(tmp->data.timeAlloc);
        free(tmp);
        return true;
    }
    Pos3 p = find3(*List3, size);
    if (p->next==NULL){
        return false;
    }else {
        Pos3 tmp;
        tmp=p->next;
        p->next = p->next->next;
        free(tmp->data.address);
        free(tmp->data.timeAlloc);
        free(tmp);
        return true;
    }
}



void printList3(List3 List3, char * type){
    if (List3==NULL){
        return;
    }else {
        printf("%5s%22s%20s%8s\n", "Address", "Size", "Time allocation", "Type");
        Pos3 q;
        for (q = List3; q != NULL; q = q->next) {
            if (strcmp (type,q->data.type)) printf("%14p%15ld%20s%10s\n", q->data.address, q->data.size, q->data.timeAlloc, q->data.type); //print only of the type asked
        }
    }
}