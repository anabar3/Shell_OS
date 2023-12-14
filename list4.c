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
    strcpy(q->data.status,input.status);
    q->data.cmdline = malloc(MAXLINEA*sizeof(char));
    strcpy(q->data.cmdline,input.cmdline);
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

void updateStatus(Pos4 node){
    if(!strcmp(node->data.status, "FINISHED") || !strcmp(node->data.status, "SIGNALED")) return;
    int status;
    pid_t result = waitpid(node->data.pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
    if (result == 0) {
        // child has not changed state
        strcpy(node->data.status, "ACTIVE");
    } else {
        // child has changed state
        if (WIFEXITED(status)){
            strcpy(node->data.status, "FINISHED");
        } else if (WIFSIGNALED(status)){
            strcpy(node->data.status, "SIGNALED");
        } else if (WIFSTOPPED(status)){
            strcpy(node->data.status, "STOPPED");
        }
    }
}

void updateAllStatus(List4 *List4){
    if(*List4 == NULL) return;
    for (Pos4 p = *List4; p != NULL; p = p->next) {
        updateStatus(p);
    }
}

void printList4(List4 List4){
    if (List4==NULL){
        printf("List is empty\n");
        return;
    }else {
        Pos4 q;
        for (q = List4; q != NULL; q = q->next) {
            printf("%7d%20s%15s%5d%15s\n", q->data.pid, Date(q->data.date), q->data.status, getpriority(PRIO_PROCESS, q->data.pid), q->data.cmdline);
        }
    }
}

void printByPid(List4 List4, int pid){
    if(List4 == NULL){
        printf("Process with PID %d not found\n", pid);
        return;
    }
    Pos4 q;
    for (q = List4; q != NULL; q = q->next) {
        if(q->data.pid == pid){
            printf("%7d%20s%15s%5d%15s\n", q->data.pid, Date(q->data.date), q->data.status, getpriority(PRIO_PROCESS, q->data.pid), q->data.cmdline);
            return;
        }
    }
    printf("Process with PID %d not found\n", pid);
}



bool delete4(List4* List4, Pos4 node){
    Pos4 tmp;
    
    if(node == NULL){ //delete first
        tmp = *List4; //node we want to delete
        *List4 = tmp->next;
    }
    else if (node->next==NULL){ //node not found
        printf("Error: Element not found\n");
        return false;
    }
    else{
        tmp = node->next; //node we want to delete
        node->next = tmp->next;
    }

    free(tmp->data.cmdline);
    free(tmp);
    return true;
}

bool deleteStatus4(List4* List4, char* status){
    if(*List4 == NULL) return true;
    if(status == NULL || !strcmp((*List4)->data.status, status)){
        while(*List4 != NULL){
            if((status == NULL || !strcmp((*List4)->data.status, status))){
                if(!delete4(List4, NULL)) return false;
            } 
        }
    }
    Pos4 q = *List4;
    while(q != NULL && q->next != NULL){
        if(status == NULL || !strcmp(q->next->data.status, status)){
            if(!delete4(List4, q)) return false;
        } else {
            q = q->next;
        }
    }
    return true;
}

bool deleteByPid(List4* List4, pid_t pid){
    Pos4 q;
    if((*List4)->data.pid==pid){
        q = NULL;
    }
    else{
        for (q = *List4; q->next!=NULL; q=q->next){ //until points to node before node searched
            if(q->next->data.pid==pid) break;
        }
    }
    return delete4(List4, q);
}
