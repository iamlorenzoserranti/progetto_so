#include <stdio.h>
#include "utility.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>



int visualizzatori;
int N;
pid_t* children = NULL;
int sig;

int main(int argc, char *argv[])
{
    pid_t pid;
    initialize();

    if (setInputs(argc, argv, &visualizzatori, &N) != 0)
    {
        fprintf(stderr, "Parametri non validi. Uscita.\n");
        exit(EXIT_FAILURE);
    }
    shmAllocate();

    //puntatore per contenere i pid dei processi figli
    children = (pid_t *)malloc(sizeof(pid_t) * visualizzatori);
    
    *next_number_shm = 1 ;

    for (int i = 0; i < visualizzatori; i++)
    {
        pid = fork(); //crea nuovo processo 
        if (pid < 0)
        {
            perror("fork fallita\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) //figlio 
        {
            printf("fuori dal ciclo infinito\n");
            while (1)
            {
                printf("dentro dal ciclo infinito\n");
                sigwait(&sigset, &sig);
                childrenHandler(sig); //comportamento visualizzatore 
            }
            
        }
        else
        {
            children[i] = pid;
        }
    }

    
    return 0;
}    
