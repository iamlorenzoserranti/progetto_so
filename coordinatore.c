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
    shm_allocate();
    
    *next_number_shm = 1 ;

    for (int i = 0; i < visualizzatori; i++)
    {
        pid = fork(); //crea nuovo processo
        if (pid < 0)
        {
            perror("fork fallita\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Codice del figlio
            printf("Figlio %d: PID = %d, PPID = %d\n", i, getpid(), getppid());
            exit(0); // termina il figlio dopo aver stampato
        }
    }

    
    return 0;
}    
