#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

volatile sig_atomic_t file_locked = 0;
volatile sig_atomic_t terminate = 0;

void handle_sigusr1(int sig) {
    file_locked = 1;
}

void handle_sigusr2(int sig) {
    file_locked = 0;
}

void handle_sigterm(int sig) {
    terminate = 1;
}

void generate_numbers(int pipefd, int count) {
    srand(time(NULL) ^ (getpid() << 16));

    for (int i = 0; i < count; ++i) {
        int num = rand() % 100;
        write(pipefd, &num, sizeof(int));

        // Sending SIGUSR1 to parent process
        kill(getppid(), SIGUSR1);

        while (file_locked) {
            pause();
        }

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

        // Sending SIGUSR2 to parent process
        kill(getppid(), SIGUSR2);

        sleep(1);
    }

    // Waiting for exit from programm
    while (!terminate) {
        pause();
    }

    close(pipefd);
}

void receive_numbers(int pipefd, int count, pid_t child_pid) {
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

        // Sending SIGUSR2 to child process
        kill(child_pid, SIGUSR2);

        // Waiting for SIGUSR1
        while (!file_locked) {
            pause();
        }
    }

    fclose(file);
    close(pipefd);

    // Send signal to child process for exit 
    kill(child_pid, SIGTERM);
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

    // SIGUSR1
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("Error: sigaction");
        exit(EXIT_FAILURE);
    }

    // SIGUSR2
    struct sigaction sa_usr2;
    sa_usr2.sa_handler = handle_sigusr2;
    sigemptyset(&sa_usr2.sa_mask);
    sa_usr2.sa_flags = 0;
    if (sigaction(SIGUSR2, &sa_usr2, NULL) == -1) {
        perror("Error: sigaction");
        exit(EXIT_FAILURE);
    }

    // For exit from programm
    struct sigaction sa_term;
    sa_term.sa_handler = handle_sigterm;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("Error: sigaction");
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
        generate_numbers(pipefd[1], count);
        exit(EXIT_SUCCESS);
    default:
        // Parent process
        close(pipefd[1]);
        receive_numbers(pipefd[0], count, pid);
        wait(NULL);
    }

    return 0;
}
