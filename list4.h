#ifndef LIST4_H
#define LIST4_H

#include "basic.h"

typedef struct data4{
    int pid;
    time_t date;
    char* status;
    char* cmdline;
    int priority;
}data4;

typedef struct Node4{
    data4 data;
    struct Node4* next;
}Node4;

typedef Node4* Pos4;
typedef Pos4 List4;

void createList4(List4* List4);
bool insert4(List4* List4, data4);
void printList4(List4 List4);
bool deleteStatus4(List4* List4, char* status);
void printByPid(List4 List4, int pid);


#endif