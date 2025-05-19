#ifndef UTILITY_H
#define UTILITY_H

#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>



extern int visualizzatori, N;
extern pid_t* children;
extern sigset_t sigset;
extern int sig;
extern sem_t* sem;


extern int* next_number_shm;

int setInputs(int argc, char *argv[], int *visualizzatori, int *N);
int shmAllocate();
void childrenHandler();
int initialize();


#endif // UTILITY_H