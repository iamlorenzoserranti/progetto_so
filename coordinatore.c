#include <stdio.h>
#include "utility.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>



int visualizzatori;
int N;
pid_t* children = NULL;
int sig;
pid_t active_pid;

int main(int argc, char *argv[])
{
    pid_t pid;
    initialize();
    
    // pthread_t tid;
    // pthread_create(&tid, NULL, inputThread, NULL);
    
    richiestaInputs(argc, argv, &visualizzatori, &N);
    shmAllocate();
    createSemaforo();

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
            while (1)
            {
                sigwait(&sigset, &sig);
                childrenHandler(sig); //comportamento visualizzatore 
            }
            
        }
        else
        {
            children[i] = pid;
        }
    }

    //sblocco del semaforo
    sem_post(sem);

    for (size_t i = 0; i < N; i++)
    {
        // while (pausa) {
        // pause(); // attesa passiva di un segnale
        // }
        assignView();
        waitConfirm();
        pidWritten(active_pid);
    }

    killChildren();


    for (size_t i = 0; i < visualizzatori; i++){
        wait(NULL);
    }

    sem_close(sem);
    sem_unlink(SEM_NAME);
    munmap(next_number_shm, sizeof(int));
    shm_unlink(SHM_NAME);

    exit(EXIT_SUCCESS);
    
    return 0;
}    
