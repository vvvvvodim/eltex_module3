#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#include <fcntl.h>

void generate_numbers(int pipefd, int count, sem_t* sem) {
    srand(time(NULL) ^ (getpid() << 16));

    for (int i = 0; i < count; ++i) {
        int num = rand() % 100;
        write(pipefd, &num, sizeof(int));

        // Lock semaphore before reading
        sem_wait(sem);

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
        sem_post(sem);

        sleep(1);
    }

    close(pipefd);
}

void receive_numbers(int pipefd, int count, sem_t* sem) {
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
        sem_post(sem);

        // Lock semaphore before reading
        sem_wait(sem);
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

    // Creating semaphore POSIX
    sem_t* sem = sem_open("/file_semaphore", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("Error: sem_open");
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
        generate_numbers(pipefd[1], count, sem);
        exit(EXIT_SUCCESS);
    default:
        // Parent process
        close(pipefd[1]);
        receive_numbers(pipefd[0], count, sem);
        wait(NULL);
    }

    // Delete semaphore
    sem_close(sem);
    sem_unlink("/file_semaphore");

    return 0;
}