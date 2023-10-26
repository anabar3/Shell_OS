//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "list1.h"

//LIST HIST

void createList1(List* List){
    List->first=NULL;
    List->last=NULL;
    List->n=1;
}

bool createNode (Pos *q){
    *q=malloc(sizeof(Node));
    return *q !=NULL;
}

bool insertCmd (char* lastcmd, List* List){
    Pos q;
    if (!createNode(&q)) return false;
    strcpy(q->input,lastcmd); //create node with info to insert
    q->next=NULL;
    List->n+=1;
    if (List->first==NULL) { //If list was empty
        List->first =q;
        List->last =q;
    }
    else{
        List->last->next=q;
        List->last=q;
    }
    return true;
}

void removeList(List* List){
    Pos p;
    while (List->first != NULL){
        p =List->first;
        List->first =List->first->next;
        free(p);
    }
    List->last=NULL;
    List->n=0;
}

char* removeNewLine (char* text){
    int i = 0;
    while(text[i] != '\0'){
        if(text[i] == '\n') text[i] = '\0';
        else i++;
    }
    return text;
}

void printList(List List, int Nshow){
    Pos q;
    int i=1;

    if ((Nshow==-1)||((List.n)<=(Nshow))){ //if no argument or if number asked is bigger than list itself print all
        for(q=List.first; q->next!=NULL; q=q->next){
            printf("%d: %s\n",i, removeNewLine(q->input));
            i++;
        }
    }else{
        for(q=List.first; Nshow!= (List.n)-1-i; q=q->next){
            i++;
        }
        while(q->next!=NULL){
            printf("%d: %s\n",i,removeNewLine(q->input));
            i++;
            q=q->next;
        }
    }
}

char* getItem(List List, int Nshow){
    Pos q;
    int i=1;
    for (q=List.first; i!=Nshow; q=q->next){
        i++;
    }
    return removeNewLine(q->input);
}

int numberOfCommands(List List){
    return List.n;
}