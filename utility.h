#ifndef UTILITY_H
#define UTILITY_H

#include <sys/types.h>
#include <signal.h>


extern int visualizzatori, N;
extern pid_t* children;
extern sigset_t sigset;
extern int sig;


extern int* next_number_shm;

int setInputs(int argc, char *argv[], int *visualizzatori, int *N);
int shmAllocate();
void childrenHandler();


#endif // UTILITY_H