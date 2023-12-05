#include "list4.h"
#include "shellfunc.h"

void createList4(List4* List4){
    *List4 = NULL;
}

bool createNode4 (Pos4 *q){
    *q=malloc(sizeof(Node4));
    return *q !=NULL;
}

bool insert4(List4* List4, data4 input){
    Pos4 q;

    if (!createNode4(&q)) return false;

    q->data.pid = input.pid;
    q->data.date = input.date;
    q->data.status = input.status; //cambiar esto
    strcpy(q->data.cmdline,input.cmdline);
    q->data.priority = input.priority;
    q->next=NULL;
    

    if (*List4==NULL) { //If list was empty
        *List4 = q;
    }
    else{
        q->next = *List4; //Insert at the beginning
        *List4 = q;
    }
    return true;
}


void printList4(List4 List4){
    if (List4==NULL){
        return;
    }else {
        printf("%5s%10s%15s%13s%13s\n", "Pid", "Date", "Status", "Command", "Priority");
        Pos4 q;
        for (q = List4; q != NULL; q = q->next) {
            printf("%5d%15s%15s%15d\n", q->data.pid, Date(q->data.date), q->data.cmdline, q->data.priority);
        }
    }
}