#include <stdio.h>
#include "utility.h"
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>


int visualizzatori;
int N;

int main(int argc, char *argv[])
{
    pid_t pid;

    setInputs(argc, argv, &visualizzatori, &N);

    pid = fork(); //crea nuovo processo

    if (pid < 0 )
    {
        perror("fork fallita\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0)
    {
        printf("Ciao sono il figlio con PID = %d, PID padre = %d\n", getpid(), getppid());
    }
    else
    {
        printf("Ciao sono il padre PID = %d, figlio %d\n", getpid(), pid);
    }
    
    
    
    return 0;
}    
