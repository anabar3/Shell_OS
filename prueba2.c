#include <stdio.h>
#include <unistd.h>

int main(){
    for(int i = 0; i < 300; i++){
        printf("A");
        fflush(stdout);
        usleep(100000);
    }
    printf("\n");
    return 0;
}