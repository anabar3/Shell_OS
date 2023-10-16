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