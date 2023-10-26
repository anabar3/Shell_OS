
//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "shellfunc.h"



int main(){
    char linea[MAXLINEA];
    char* tr[MAXLINEA/2]; //array of strings (pointer to first element of array of chars)
    char tmp[MAXLINEA];

    List his;
    createList1(&his);
    
    List2 openFiles;
    createList2(&openFiles);
    insertOpenFile(&openFiles, 0, "stdin",fcntl(0,F_GETFL));
    insertOpenFile(&openFiles, 1, "stdout", fcntl(1,F_GETFL));
    insertOpenFile(&openFiles, 2, "stderr", fcntl(2,F_GETFL));
    
    while (1) {
        printf("\n-->");
        fgets(linea,200,stdin);
        strcpy (tmp,linea); //make a copy of linea for the list before cutting it
        CutCommand(linea,tr);
        ProcessCommand(tmp,tr, &his, &openFiles);
    }
}
 
