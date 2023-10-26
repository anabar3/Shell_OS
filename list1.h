//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "basic.h"

typedef struct Node{
    char input[MAXLINEA];
    struct Node* next;
}Node;

typedef Node* Pos;

typedef struct List{
    int n; //total number of elements
    Pos first;
    Pos last;
}List;


void createList1(List* List);
bool insertCmd (char* lastcmd, List* List);
void removeList(List* List);
void printList(List List, int Nshow);
char* getItem(List List, int Nshow);
int numberOfCommands(List List);