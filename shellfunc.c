#include "shellfunc.h"

//MAIN FUNCTIONS
int CutCommand(char *cadena, char* trozos[]){  //Create an array of strings with the separated parts
    int i=1;
    if ((trozos[0]=strtok(cadena," \n\t"))==NULL)
        return 0;
    while ((trozos[i]=strtok(NULL," \n\t"))!=NULL) //The function strtok() remembers the previous string
        i++;
    return i; //Returns number of parts of the command
}

void ProcessCommand(char* linea, char *tr[], List* his, List2* openFiles){ //Takes the array of strings (divided command)

    int i;
    static struct CMD C[]={ //Array C of struct CMD {name, function}
            {"authors", Cmd_authors},
            {"quit", Cmd_quit},
            {"exit", Cmd_quit},
            {"bye", Cmd_quit},
            {"pid", Cmd_pid},
            {"chdir", Cmd_chdir},
            {"date",Cmd_date},
            {"time", Cmd_time},
            {"help",Cmd_help},
            {"infosys", Cmd_infosys},
            {"create", Cmd_create},
            {"stat", Cmd_stat},
            {NULL,NULL}
    };

    if (tr[0]==NULL)
        return;

    for (i=0;C[i].name!=NULL;i++) { //Loop comparing the tokens with the name of the command

        if (!strcmp(tr[0], "hist")) { //Cases where commands need lists
            if(!insertCmd(linea, his)){
                printf("Error: Could not add to command history (not enough memory?)");
            }
            Cmd_hist(tr + 1, his);
            return;
        } else if(!strcmp(tr[0], "command")) {
            Cmd_command(tr + 1, his, openFiles);
            return;
        }else if (!strcmp (tr[0], "open")) {
            Cmd_open(tr + 1, openFiles);
            return;
        }else if (!strcmp (tr[0], "close")) {
            Cmd_close(tr + 1, openFiles);
            return;
        }else if (!strcmp (tr[0], "dup")) {
            Cmd_dup(tr + 1, openFiles);
            return;
        }else if (!strcmp (tr[0], "listopen")) {
            Cmd_listopen(tr + 1, *openFiles);
            return;
        }
        else if (!strcmp(tr[0], C[i].name)) { //Cases where commands don't need anything else
            if(!insertCmd(linea, his)){
                printf("Error: Could not add to command history (not enough memory?)");
            }
            (*C[i].func)(tr + 1);
            return;
        }
    }
    printf("Error: invalid command\n");
}


//COMMANDS
void Cmd_authors (char *tr[]){

    if (tr[0]==NULL){
        printf("Name: Javier Carballal Morgade    Login: javier.carballal.morgade@udc.es \n");
        printf("Name: Ana Barrera Novas           Login: ana.barrera@udc.es\n");
    }
    else if (!strcmp(tr[0], "-l"))
        printf("Logins:\njavier.carballal.morgade@udc.es\nana.barrera@udc.es\n");
    else if (!strcmp(tr[0],"-n"))
        printf("Names:\nJavier Carballal Morgade\nAna Barrera Novas\n");
    else
        printf("Error: Invalid argument\n");
}

void Cmd_quit (char *tr[]){
    exit(0);
}

void Cmd_pid (char *tr[]){
    if (tr[0]==NULL)
        printf ("Shell's pid is %d\n", getpid());
    else if (!strcmp (tr[0],"-p"))
        printf ("Shell parent's pid is %d\n", getppid());
    else
        printf("Error: Invalid argument\n");
}

void Cmd_chdir (char *tr[]){
    if (tr[0]==NULL) {
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) == NULL) //gets current directory
            perror("Impossible to show directory");
        else
            printf("%s\n", cwd);
    }
    else
    if (chdir(tr[0])!=0) //changes directory to the given
        perror ("Impossible to change directory");
}

void Cmd_date (char *tr[]){
    if (tr[0]!=NULL){
        printf("Warning: Unnecessary argument\n");
    }
    time_t t = time(NULL);
    if(t == -1){
        perror("Impossible to get date");
        return;
    }
    struct tm tm= *localtime (&t);
    printf("%.2d/%.2d/%.2d\n", tm.tm_mday,tm.tm_mon+1,tm.tm_year+1900); //time library functions work like this
}

void Cmd_time (char *tr[]){
    if (tr[0]!=NULL){
        printf("Warning: Unnecessary argument\n");
    }
    time_t t = time(NULL);
    if(t == -1){
        perror("Impossible to get time");
        return;
    }
    struct tm tm= *localtime (&t);
    printf("%.2d:%.2d:%.2d\n", tm.tm_hour,tm.tm_min,tm.tm_sec); //time library functions work like this
}

void Cmd_hist(char *tr[], List* his){
    if(tr[0]==NULL){
        printList(*his,-1);
    }
    else if (!strcmp(tr[0],"-c")){
        removeList(his);
    }
    else if (atoi (tr[0]) < 0){
        printList(*his, abs(atoi (tr[0])));
    }
    else printf("Error: Invalid argument\n");
}

void Cmd_command (char *tr[], List* his, List2* openFiles){
    if (atoi(tr[0])!=0 &&(tr[0]!= NULL)){
        int numcomm = numberOfCommands(*his) - 1;
        int numindex = atoi(tr[0]);
        char* tmp1;
        char* tr1[MAXLINEA/2];
        if(numindex <= numcomm && numindex > 0){
            tmp1 = getItem(*his,atoi(tr[0]));
        }
        else{
            printf("Error: Command number not found\n");
            return;
        }
        CutCommand(tmp1, tr1);
        ProcessCommand(tmp1,tr1, his, openFiles);
    }else if (atoi(tr[0]) == 0){
        printf("Error : Command number not found\n");
    }
    else{
        printf("Error: Invalid argument\n");
    }
}

void Cmd_help (char* tr[]){
    struct Help{
        char* cmdName;
        char* cmdDescription;
    };

    static struct Help H[]={
            {"authors [-l]",": Prints the names and logins of the program authors.\n"
                       "-l : prints only the logins and authors\n-n : prints only the names"},

            {"quit", ": Ends the shell"},

            {"exit", ": Ends the shell"},

            {"bye", "Ends the shell"},

            {"pid [-p]","Prints the pid of the process executing the shell.\n"
                   "-p : prints the pid of the shell’s parent process"},

            {"chdir","Changes the current working directory of the shell to dir. "
                     "When invoked without auguments it prints the current working directory"},

            {"date","Prints the current date"},

            {"time", "Prints the current time"},

            {"help","displays a list of available commands. help cmd gives a brief help "
                    "on the usage of comand cmd"},

            {"hist [-c] [-N]", "Shows/clears the historic of commands executed by this shell.\n"
                     "-c : Clears (empties) the list of historic commands\n-N : Prints the first N comands"},

            {"command N", "Repeats command number N"},

            {"infosys", "Prints information on the machine running the shell"},

            {"open {file} {mode}","Opens the file {file} and adds it to the list of shell open files\n" 
                    "mode values : cr, ap, ex, ro, rw, wo, tr"},

            {"close", "Closes the file with df file descriptor and eliminates the corresponding item from the list"},

            {"dup", "Duplicates the df file descriptor (creating the corresponding new entry on the file list"},

            {"listopen","Lists the shell open files. For each file it lists its descriptor, the file "
                        "name and the opening mode."},
            {"stat [-long][-link] [-acc]", "Gives information on a file or directory.\n"
                    "-long : long list\n-acc : access time\n-link : if it is symbolic link, the path contained"},
            {NULL,NULL}
    };

    if (tr[0] == NULL) { //If no argument, print all command names
        for (int i=0;H[i].cmdName !=NULL;i++) {
            printf("%s\n", H[i].cmdName);
        }
        return;
    } else { //If argument valid, print name and description
        for (int i=0;H[i].cmdName !=NULL;i++) {
            if (!strcmp(tr[0], H[i].cmdName)) {
                printf("%s: %s\n", H[i].cmdName, H[i].cmdDescription);
                return;
            }
        }
    }
    printf("Error: Invalid argument");
}

void Cmd_infosys(char* tr[]){
    if (tr[0]!=NULL){
        printf("Warning: Unnecessary argument\n");
    }
    struct utsname u;
    if (uname(&u) != 0) {
        perror("Could not get system info");
    }

    printf("system name = %s\n", u.sysname);
    printf("node name   = %s\n", u.nodename);
    printf("release     = %s\n", u.release);
    printf("version     = %s\n", u.version);
    printf("machine     = %s\n", u.machine);
}

void Cmd_listopen(char* tr[], List2 openFiles){
    if(tr[0] != NULL){
        printf("Warning: Unnecessary argument\n");
    }
    printList2(openFiles);
    return;
}

void Cmd_open (char * tr[], List2* openFiles){
    int i,df, mode=0;

    if (tr[0]==NULL) { //no argument, only print the list
        printList2(*openFiles);
        return;
    }
    for (i=1; tr[i]!=NULL; i++)
        if (!strcmp(tr[i],"cr")) mode|=O_CREAT;
        else if (!strcmp(tr[i],"ex")) mode|=O_EXCL;
        else if (!strcmp(tr[i],"ro")) mode|=O_RDONLY;
        else if (!strcmp(tr[i],"wo")) mode|=O_WRONLY;
        else if (!strcmp(tr[i],"rw")) mode|=O_RDWR;
        else if (!strcmp(tr[i],"ap")) mode|=O_APPEND;
        else if (!strcmp(tr[i],"tr")) mode|=O_TRUNC;
        else break;

    if ((df=open(tr[0],mode,0777))==-1)
        perror ("Impossible to open file");
    else{
        insertOpenFile(openFiles, df, tr[0], mode);
        printf ("Added to open files table:\n");
        printList2(*openFiles);
    }
}

void Cmd_close (char *tr[], List2* openFiles){
    int df;

    if (tr[0]==NULL || (df=atoi(tr[0])<0)) { //no parameter or descriptor<0
        printList2(*openFiles);
        return;
    }
    df = atoi(tr[0]);
    if(df == 0 && (strcmp(tr[0], "0") != 0)){
        printf("Error: Invalid argument\n");
        return;
    }
    if (close(df)==-1)
        perror("Impossible to close descriptor");
    else{
        deleteNodeOpen(openFiles,df);
        printf("Removed from open files table:\n");
        printList2(*openFiles);
    }
}

void Cmd_dup (char * tr[], List2* openFiles){
    int df, newdf;

    if (tr[0]==NULL || (df=atoi(tr[0]))<0) { //no parameter or descriptor <0
        printList2(*openFiles);
        return;
    }
    df = atoi(tr[0]);
    if(df == 0 && (strcmp(tr[0], "0") != 0)){
        printf("Error: Invalid argument\n");
        return;
    }
    if ((newdf = dup(df))==-1)
        perror("Impossible to duplicate descriptor");
    else{
        duplicateNode(openFiles,df, newdf);
        printf("Duplicated in the open files table:\n");
        printList2(*openFiles);
    }
}

void Cmd_create (char* tr[]){
    if (tr[0]==NULL) { //No arguments
        Cmd_chdir(tr);
    }
    else if (!strcmp(tr[0], "-f")){
        if(creat(tr[1], 00777) == -1){
            perror ("Impossible to create file");
        }
    }
    else{
        if (mkdir(tr[0], 00777)== -1){
            perror ("Impossible to create directory");
        }
    }
}

void Cmd_stat (char* tr[]){
    if (tr[0]==NULL) { //No arguments
        Cmd_chdir(tr);
    }
    else{
        int i=0;
        struct stat statbuf;
        bool statlong =false, acc =false, link=false;

        for (i; tr[i]!=NULL;i++){ //Check arguments
            if (!strcmp(tr[i], "-long")){
                statlong=true;
            }else if (!strcmp(tr[i], "-acc")){
                acc=true;
            }else if (!strcmp(tr[i], "-link")){
                link =true;
            }else break;
        }

        for (;tr[i]!=NULL;i++){ //Check names of files

            if (stat(tr[i], &statbuf)==-1){
                perror("Impossible to access file or directory");
            } else{ 
                if (statlong){
                    printf ("1. Last status change   2. Link count   3. (I-node number)   4. Ownership   5.Permissions   6. File size\n");
                    printf ("%s  %ld (%ld)   %lld", statbuf.st_mtime, (long) statbuf.st_nlink, (long) statbuf.st_ino, (long long) statbuf.st_size);
                }
                if (acc){
                    printf("Last file access: %s", ctime(&statbuf.st_atime));
                }
                if (link){
                    printf("lalala");
                }


                printf("I-node number:            %ld\n", (long) statbuf.st_ino);
                printf("Mode:                     %lo (octal)\n", (unsigned long) statbuf.st_mode);
                printf("Link count:               %ld\n", (long) statbuf.st_nlink);
                printf("Ownership:                UID=%ld   GID=%ld\n", (long) statbuf.st_uid, (long) statbuf.st_gid);
                printf("Preferred I/O block size: %ld bytes\n", (long) statbuf.st_blksize);
                printf("File size:                %lld bytes\n", (long long) statbuf.st_size);
                printf("Blocks allocated:         %lld\n", (long long) statbuf.st_blocks);
                printf("Last status change:       %s", ctime(&statbuf.st_ctime));
                printf("Last file access:         %s", ctime(&statbuf.st_atime));
                printf("Last file modification:   %s", ctime(&statbuf.st_mtime));

            }
        }
    }
}

void Cmd_list (char* tr[]){

}

void Cmd_delete (char* tr[]){

}

void Cmd_deltree (char* tr[]){
    
}


