#ifndef LIST4_H
#define LIST4_H

#include "basic.h"

struct status{
    bool finished;
    bool stopped;
    bool signaled;
    bool active;
};

typedef struct data4{
    int pid;
    time_t date;
    struct status status; //o hacerlo con char y pista
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

#endif