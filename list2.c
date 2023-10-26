//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "list2.h"

//LIST OPEN

void createList2(List2* List2){
    *List2 = NULL;
}

bool createNode2 (Pos2 *q){
    *q=malloc(sizeof(Node2));
    return *q !=NULL;
}

bool insertOpenFile(List2* List2, int desc, char* name, int mode){
    Pos2 q;

    if (!createNode2(&q)) return false;

    q->data.descriptor = desc;
    strcpy(q->data.name, name);
    q->data.mode = mode;
    q->next=NULL;

    if (*List2==NULL) { //If list was empty
        *List2 = q;
    }
    else{
        q->next = *List2; //Insert at the beginning
        *List2 = q;
    }
    return true;
}

Pos2 findOpenFile(List2 List2, int desc){
    Pos2 q;
    for (q = List2; q->next->data.descriptor!=desc && q->next!=NULL; q=q->next); //until points to node before file searched
    return q;
}

bool deleteNodeOpen(List2* List2, int desc){
    if((*List2)->data.descriptor == desc){
        Pos2 tmp;
        tmp= *List2;
        *List2 = tmp->next;
        free(tmp);
        return true;
    }
    Pos2 p = findOpenFile(*List2, desc);
    if (p->next==NULL){
        return false;
    }else {
        Pos2 tmp;
        tmp=p->next;
        p->next = p->next->next;
        free(tmp);
        return true;
    }
}

bool duplicateNode (List2* List2, int desc, int newdesc){
    Pos2 p;
    if((*List2)->data.descriptor == desc) {
        p = *List2;
    }
    else{
        p = findOpenFile(*List2, desc)->next;
    }
    Pos2 q;
    if(p==NULL){
        return false;
    }else {
        if (!createNode2(&q)) return false;

        q->data.descriptor = newdesc;
        strcpy(q->data.name, p->data.name);
        q->data.mode = p->data.mode;

        q->next = p->next;
        p->next = q;
        return true;
    }
}

void printList2(List2 List2){
    if (List2==NULL){
        return;
    }else {
        printf("%5s%10s%15s\n", "Descriptor", "Name", "Mode");
        Pos2 q;
        for (q = List2; q != NULL; q = q->next) {
            printf("%5d%15s%15d\n", q->data.descriptor, q->data.name, q->data.mode);
        }
    }
}