#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>      // O_CREAT, O_RDWR
#include <sys/mman.h>   // shm_open, mmap, ftruncate, PROT_*, MAP_*
#include <sys/stat.h>   // S_IRUSR, S_IWUSR
#include <unistd.h>     // ftruncate, close
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include <string.h>




#define MAX_PROCESSI 100
#define MAX_N __INT_MAX__
#define SHM_NAME "/my_shared_memory_so"
#define SEM_NAME "/semaforo_so_project"


//definizone variabili, le dichiarazione stanno in utility.h
int* next_number_shm = NULL;
sigset_t sigset;
sem_t* sem = NULL;


int stop = 0;

int validaInput(const char *prompt, int *value, int max) {
    char buffer[128];
    int res;

    do {
        // Mostra la richiesta all'utente
        printf("%s", prompt);

        // Legge una riga di input da tastiera (fino a 127 caratteri)
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            return -1;

        // Prova a leggere un intero dalla riga appena letta
        res = sscanf(buffer, "%d", value);

        // Controlla se:
        // - è stato letto effettivamente un numero
        // - il numero è > 0
        // - il numero è <= max
        if (res != 1 || *value <= 0 || *value > max) {
            printf("Valore non valido. Riprova.\n");
            res = -1; // forza il ciclo a ripetere
        }

    } while (res != 1); // ripeti finché non leggi un numero valido

    return 0;
}

int validaParametri(int visualizzatori, int N){
    if (visualizzatori <= 0 || visualizzatori > MAX_PROCESSI)
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

int richiestaInputs(int argc, char *argv[], int *visualizzatori, int *N){
    if (argc > 3) {
        fprintf(stderr, "Uso corretto: %s [visualizzatori N]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc == 3)
    {
        *visualizzatori = atoi(argv[1]); //atoi() converte da stringa a intero
        *N = atoi(argv[2]);
    }
    else
    {
        validaInput("Inserisci numero di visualizzatori (max: %d): ", visualizzatori, MAX_PROCESSI);
        validaInput("Inserisci N massimo da visualizzare (max: %d): ", N, MAX_N);
    }

    return validaParametri(*visualizzatori, *N);
}

//avvisa i processi figli di terminare
void killChildren()
{
    stop = 0;
    for (size_t i = 0; i < visualizzatori; i++){
        kill(children[i], SIGUSR2);
    }
}

void sigtermHandle()
{
    struct sigaction sigterm_action;
    memset(&sigterm_action, 0, sizeof(sigterm_action));

    sigterm_action.sa_handler = killChildren;
    sigaction(SIGTERM, &sigterm_action, NULL);
}

void sigintHandle()
{
    struct sigaction sigint_action;
    memset(&sigint_action, 0, sizeof(sigint_action));

    // Imposta il gestore di SIGINT per ignorare il segnale
    sigint_action.sa_handler = SIG_IGN;
    sigaction(SIGINT, &sigint_action, NULL);
}

//funzione di inizilizzazione di: segnali, sem, e i file
int initialize(){
    sigemptyset(&sigset);                  // svuotamento del set
    sigaddset(&sigset, SIGUSR1);           // aggiunta al set di SIGUSR1
    sigaddset(&sigset, SIGUSR2);           // aggiunta al set di SIGUSR2
    sigaddset(&sigset, SIGTSTP);           // aggiunta al set di SIGUSR2
    sigprocmask(SIG_BLOCK, &sigset, NULL); // Mascheramento/blocco di SIGUSR1, SIGINT

    sigtermHandle();
    sigintHandle();
    srand(time(NULL));

    FILE *file;
    // Apre file in modalità scrittura e lo tronca
    file = fopen("coord_pid.txt", "w");
    fclose(file);
    file = fopen("vis_pid.txt", "w");
    fclose(file);

    return 0;
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

/*
    funzione che consente al visualizzatore di accedere al sem, scrivere sul file il proprio PID,
    incrementare next_number_shm, avvisare il process. padre di aver finito e rilascia il sem. 
*/
void executeChildrenTurn(){
    sem_wait(sem);

    printf("%d\n", *next_number_shm);
    FILE *file = fopen("vis_pid.txt", "a+");
    if (file == NULL)
    {
        perror("Errore: fopen.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "%d\n", getpid());
    fclose(file);
    (*next_number_shm)++;

    kill(getppid(), SIGUSR1);//ora il padre sa che il figlio ha terminato 
    
    if (*next_number_shm > N)
    {
        sem_post(sem);
        exit(EXIT_SUCCESS);
    }
    sem_post(sem);
}

void childrenStop(){
    exit(EXIT_SUCCESS);
}

void childrenHandler(int sig)
{
    switch (sig)
    {
    case SIGUSR1:
        executeChildrenTurn();
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

//rende disponibile il semaforo alla variabile globale
int createSemaforo(){
    sem = sem_open(SEM_NAME, O_CREAT, S_IRWXU, 0);
    if (sem == SEM_FAILED)
    {
        perror("Errore in sem_open.\n");
        exit(EXIT_FAILURE);
    }
    
    return 0;
}

void stopProcess(int sig)
{
    do
    {
        sigwait(&sigset, &sig);
    } while (sig != SIGTSTP);
}

//i: operazioni coordinatore
void assignView() {
    size_t j = rand() % visualizzatori;
    active_pid = children[j];
    kill(active_pid, SIGUSR1);
}

void waitConfirm() {
    sigwait(&sigset, &sig);
    if (sig == SIGTSTP) {
        stopProcess(sig);
    }
}

void pidWritten(pid_t pid) {
    FILE *file = fopen("coord_pid.txt", "a");
    if (file != NULL) {
        fprintf(file, "%d\n", pid);
        fclose(file);
    }
}
