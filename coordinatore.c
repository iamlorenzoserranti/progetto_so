#include <stdio.h>
#include "utility.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>


int visualizzatori;
int N;
pid_t* children = NULL;
int sig;
pid_t active_pid;

int main(int argc, char *argv[])
{
    pid_t pid;
    initialize();
    
    pthread_t tid;
    pthread_create(&tid, NULL, inputThread, NULL);
    
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
        while (pausa) {
        sleep(1); // attesa passiva ma controllata
        }
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

    // Terminazione del thread di input
    fine = 1;
    pthread_cancel(tid);        // Forza la chiusura se bloccato su getchar()
    pthread_join(tid, NULL);    // Attende la sua terminazione

    exit(EXIT_SUCCESS);
    
    return 0;
}    
