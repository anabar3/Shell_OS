//AUTHORS - Group 6.1 - 28/09/2023
//Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es
//Name: Ana Barrera Novas           Login: ana.barrera@udc.es

#include "basic.h"
#include "list1.h"
#include "list2.h"
#include "list3.h"
#include "list4.h"

struct CMD{
    char * name;
    void (*func)(char **);
};

int CutCommand(char *cadena, char* trozos[]);
void ProcessCommand(char* linea, char *tr[], List* his, List2* openFiles, List3* memlist);

void Cmd_authors (char *tr[]);
void Cmd_quit (char *tr[]);
void Cmd_pid (char *tr[]);
void Cmd_chdir (char *tr[]);
void Cmd_date (char *tr[]);
void Cmd_time (char *tr[]);
void Cmd_hist(char *tr[], List* his);
void Cmd_command (char *tr[], List* his, List2* openFiles, List3* memlist);
void Cmd_help (char* tr[]);
void Cmd_infosys(char* tr[]);
void Cmd_listopen(char* tr[], List2 openFiles);
void Cmd_open (char * tr[], List2* openFiles);
void Cmd_close (char *tr[], List2* openFiles);
void Cmd_dup (char * tr[], List2* openFiles);
void Cmd_create(char * tr[]);
char* Date(time_t filedate);
void Cmd_stat(char* tr[]);
void Cmd_list (char* tr[]);
void Cmd_delete (char* tr[]);
void Cmd_deltree (char* tr[]);
bool Do_chdir(char* name);
void Cmd_malloc (char* tr[], List3* memlist);
void Cmd_shared (char* tr[], List3* memlist);
void Cmd_mmap (char *tr[], List3* memlist);
void Cmd_read (char *tr[]);
void Cmd_write (char *tr[]);
void Cmd_memdump(char *tr[]);
void Cmd_memfill(char* tr[]);
void Cmd_recurse(char *tr[]);
void Cmd_mem(char *tr[], List3* memlist);
void Cmd_uid(char*tr[], List4* proclist);