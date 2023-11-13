//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "basic.h"

struct data{
    void* address;
    size_t size;
    time_t timeAlloc;
    char* type;
    
};

typedef struct Node3{
    struct data data;
    struct Node3* next;
}Node3;

typedef Node3* Pos3;
typedef Pos3 List3;

void createList3(List3* List3);
bool insert3(List3* List3, struct data);
bool delete3(List3* List3, size_t size);
void printList3(List3 List3);
