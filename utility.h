#ifndef UTILITY_H
#define UTILITY_H

#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>

#define SHM_NAME "/my_shared_memory_so"
#define SEM_NAME "/semaforo_so_project"

extern int visualizzatori, N;
extern pid_t* children;
extern sigset_t sigset;
extern int sig;
extern sem_t* sem;
extern pid_t active_pid;
extern volatile sig_atomic_t pausa;
//controllo fine thread
extern volatile sig_atomic_t fine;

extern int* next_number_shm;

int richiestaInputs(int argc, char *argv[], int *visualizzatori, int *N);
int shmAllocate();
void childrenHandler();
int initialize();
int createSemaforo();

//operazioni coordinatore
void assignView();
void waitConfirm();
void pidWritten();
void stopProcess();
void killChildren();
void *inputThread();

#endif