#include "basic.h"

struct openNode{
    int descriptor;
    char name[MAXFILENAME];
    int mode;
};

typedef struct Node2{
    struct openNode data;
    struct Node2* next;
}Node2;

typedef Node2* Pos2;
typedef Pos2 List2;

void createList2(List2* List2);
bool insertOpenFile(List2* List2, int desc, char* name, int mode);
bool deleteNodeOpen(List2* List2, int desc);
bool duplicateNode (List2* List2, int desc, int newdesc);
void printList2(List2 List2);
