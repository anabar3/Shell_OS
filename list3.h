//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "basic.h"

typedef struct data3{
    void* address;
    size_t size;
    char* timeAlloc;
    char type[7];
    key_t cl;
    
}data3;

typedef struct Node3{
    data3 data;
    struct Node3* next;
}Node3;

typedef Node3* Pos3;
typedef Pos3 List3;

void createList3(List3* List3);
Pos3 find3 (List3 List3, size_t size);
bool insert3(List3* List3, data3);
bool delete3(List3* List3, size_t size);
void printList3(List3 List3, char* type);
