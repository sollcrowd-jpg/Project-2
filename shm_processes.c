#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

#define SEM_NAME "bank_sem"
#define INITIAL_BALANCE 100

void dear_old_dad(int *bank_account);
void lovable_mom(int *bank_account);
void poor_student(int *bank_account, int id);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <num_parents> <num_children>\n", argv[0]);
        exit(1);
    }

    int num_parents = atoi(argv[1]);
    int num_children = atoi(argv[2]);

    if (num_parents < 1 || num_children < 1) {
        printf("Number of parents and children must both be at least 1\n");
        exit(1);
    }

    int shm_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget error");
        exit(1);
    }

    int *bank_account = (int *)shmat(shm_id, NULL, 0);
    if (bank_account == (int *)-1) {
        perror("shmat error");
        exit(1);
    }

    *bank_account = INITIAL_BALANCE; // Initialize bank account balance

    sem_t *mutex = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if (mutex == SEM_FAILED) {
        perror("sem_open error");
        exit(1);
    }

    int i; // Declare loop variable outside the loop to avoid redefinition

    // Fork parent processes
    for (i = 0; i < num_parents; i++) {
        if (fork() == 0) {
            if (i == 0) {
                dear_old_dad(bank_account);
            } else {
                lovable_mom(bank_account);
            }
            exit(0);
        }
    }

    // Fork child processes
    for (i = 0; i < num_children; i++) {
        if (fork() == 0) {
            poor_student(bank_account, i + 1);
            exit(0);
        }
    }

    // Wait for all child processes
    while (wait(NULL) > 0);

    // Cleanup shared memory and semaphore
    shmdt(bank_account);
    shmctl(shm_id, IPC_RMID, NULL);
    sem_close(mutex);
    sem_unlink(SEM_NAME);

    printf("Server has cleaned up shared memory and semaphore. Exiting...\n");
    return 0;
}

void dear_old_dad(int *bank_account) {
    sem_t *mutex = sem_open(SEM_NAME, 0);
    srand(getpid());

    while (1) {
        sleep(rand() % 5);
        printf("Dear Old Dad: Attempting to check balance...\n");

        sem_wait(mutex);
        int local_balance = *bank_account;
        sem_post(mutex);

        if (rand() % 2 == 0) {
            if (local_balance < 100) {
                int deposit = rand() % 100;
                printf("Dear Old Dad: Deposits $%d\n", deposit);

                sem_wait(mutex);
                *bank_account += deposit;
                sem_post(mutex);
            } else {
                printf("Dear Old Dad: Thinks the student has enough cash ($%d)\n", local_balance);
            }
        } else {
            printf("Dear Old Dad: Last checking balance = $%d\n", local_balance);
        }
    }
}

void lovable_mom(int *bank_account) {
    sem_t *mutex = sem_open(SEM_NAME, 0);
    srand(getpid());

    while (1) {
        sleep(rand() % 10);
        printf("Lovable Mom: Attempting to check balance...\n");

        sem_wait(mutex);
        int local_balance = *bank_account;
        sem_post(mutex);

        if (local_balance <= 100) {
            int deposit = rand() % 125;
            printf("Lovable Mom: Deposits $%d\n", deposit);

            sem_wait(mutex);
            *bank_account += deposit;
            sem_post(mutex);
        }
    }
}

void poor_student(int *bank_account, int id) {
    sem_t *mutex = sem_open(SEM_NAME, 0);
    srand(getpid());

    while (1) {
        sleep(rand() % 5);
        printf("Poor Student %d: Attempting to check balance...\n", id);

        sem_wait(mutex);
        int local_balance = *bank_account;
        sem_post(mutex);

        if (rand() % 2 == 0) {
            int need = rand() % 50;
            printf("Poor Student %d: Needs $%d\n", id, need);

            if (need <= local_balance) {
                printf("Poor Student %d: Withdraws $%d\n", id, need);

                sem_wait(mutex);
                *bank_account -= need;
                sem_post(mutex);
            } else {
                printf("Poor Student %d: Not enough cash ($%d)\n", id, local_balance);
            }
        } else {
            printf("Poor Student %d: Last checking balance = $%d\n", id, local_balance);
        }
    }
}
