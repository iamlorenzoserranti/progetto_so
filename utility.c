#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // O_CREAT, O_RDWR
#include <sys/mman.h>   // shm_open, mmap, ftruncate, PROT_*, MAP_*
#include <sys/stat.h>   // S_IRUSR, S_IWUSR
#include <unistd.h>     // ftruncate, close
#include <sys/types.h>
#include <signal.h>


#define MAX_PROCESSI 100
#define MAX_N 10000
#define SHM_NAME "/my_shared_memory_so"


int* next_number_shm = NULL;
sigset_t sigset;

int validaParametri(int visualizzatori, int N){
    if (visualizzatori <= 0 || visualizzatori >= MAX_PROCESSI)
    {
        fprintf(stderr, "Errore: numero di visualizzatori non valido (1 - %d).\n", MAX_PROCESSI);
        exit(EXIT_FAILURE);
    }
    if (N <= 0 || N > MAX_N)
    {
        fprintf(stderr, "Errore: N deve essere un intero positivo <= %d, \n", MAX_N);
        exit(EXIT_FAILURE);
    }
    return 0;
}

int setInputs(int argc, char *argv[], int *visualizzatori, int *N){
    if (argc == 3)
    {
        //per nonf amri dare segmentation fault con ./coordinatore devo passargli anche i due argomenti 
        *visualizzatori = atoi(argv[1]); //atoi() converte da stringa a intero
        *N = atoi(argv[2]);
    }
    else
    {
        printf("Inserisci numero di visualizzatori (max: %d): ", MAX_PROCESSI);
        scanf("%d", visualizzatori);

        printf("Inserisci N massimo d visualizzare (max: %d): ", MAX_N);
        scanf("%d", N);
    }
    return validaParametri(*visualizzatori, *N);
}

int shmAllocate(){
    //creazione della memoria condivisa con permessi lettura/scrittura
    int shm = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (shm == -1)
    {
        perror("Errore in shm_open.\n");
        exit(EXIT_FAILURE);
    }
    //imposta la dimensione della shared memeory a sizeof(int)
    if ( ftruncate(shm, sizeof(int)) == -1 )
    {
        perror("Errore in ftruncate.\n");
        exit(EXIT_FAILURE);
    }
    //mappatura della memoria nello spazio del processo 
    next_number_shm = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if (next_number_shm == MAP_FAILED)
    {
        perror("Errore in mmap\n");
        exit(EXIT_FAILURE);
    }
}

void childrenHandler(int sig)
{
    switch (sig)
    {
    case SIGUSR1:
        childenSharedIncrement();
        break;
    case SIGUSR2:
        childrenStop();
        break;
    case SIGTSTP:
        break;
    default:
        perror("Errore: segnale inattesso.\n");
        exit(EXIT_FAILURE);
    }
}