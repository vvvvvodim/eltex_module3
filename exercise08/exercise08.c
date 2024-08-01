#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/ipc.h>

// Semaphore operations
struct sembuf sem_lock = { 0, -1, 0 };
struct sembuf sem_unlock = { 0, 1, 0 };

void generate_numbers(int pipefd, int count, int semid) {
    srand(time(NULL) ^ (getpid() << 16));

    for (int i = 0; i < count; ++i) {
        int num = rand() % 100;
        write(pipefd, &num, sizeof(int));

        // Lock semaphore before reading
        semop(semid, &sem_lock, 1);

        FILE* file = fopen("output.txt", "r");
        if (file == NULL) {
            perror("Error: fopen");
            exit(EXIT_FAILURE);
        }

        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("Child read: %s", buffer);
        }
        fclose(file);

        // Unlock semaphore after reading
        semop(semid, &sem_unlock, 1);

        sleep(1);
    }

    close(pipefd);
}

void receive_numbers(int pipefd, int count, int semid) {
    FILE* file = fopen("output.txt", "w");
    if (file == NULL) {
        perror("Error: fopen");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < count; ++i) {
        int num;
        read(pipefd, &num, sizeof(int));
        printf("Received number: %d\n", num);
        fprintf(file, "Received number: %d\n", num);
        fflush(file);

        // Unlock semaphore after writing
        semop(semid, &sem_unlock, 1);

        // Lock semaphore before reading
        semop(semid, &sem_lock, 1);
    }

    fclose(file);
    close(pipefd);  
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <count>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int count = atoi(argv[1]);
    if (count <= 0) {
        fprintf(stderr, "Count must be a positive integer\n");
        exit(EXIT_FAILURE);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Error: pipe");
        exit(EXIT_FAILURE);
    }

    // Creating semaphore System V
    key_t key = ftok("semfile", 65);
    int semid = semget(key, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("Error: semget");
        exit(EXIT_FAILURE);
    }

    // Initialization
    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("Error: semctl");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    switch (pid = fork()) {
    case -1:
        perror("Error: fork.");
        exit(EXIT_FAILURE);
    case 0:
        // Child process
        close(pipefd[0]);
        generate_numbers(pipefd[1], count, semid);
        exit(EXIT_SUCCESS);
    default:
        // Parent process
        close(pipefd[1]);
        receive_numbers(pipefd[0], count, semid);
        wait(NULL);
    }

    // Delete semaphore
    semctl(semid, 0, IPC_RMID);

    return 0;
}