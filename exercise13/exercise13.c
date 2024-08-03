#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

// Shared memory size
#define SHM_SIZE 1024

volatile sig_atomic_t stop = 0;

int processed_sets = 0;

void handle(int sig) {
    stop = 1;
}

// Generate random numbers in the range
int random_in_range(int low, int high) {
    return low + rand() % (high - low + 1);
}

int main() {
    // SIGINT
    struct sigaction sa;
    sa.sa_handler = handle;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    const char* shm_name = "/my_shm";
    int shm_fd;
    int* shm;

    // Creating a shared memory segment
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error: shm_open");
        exit(1);
    }

    // Setting the size of the shared memory segment
    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("Error: ftruncate");
        exit(1);
    }

    // Mapping the shared memory segment to the address space
    shm = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm == MAP_FAILED) {
        perror("Error: mmap");
        exit(1);
    }

    pid_t pid;

    switch (pid = fork()) {
    case -1:
        perror("Error: fork.");
        exit(EXIT_FAILURE);
    case 0:
        // Child process
        while (!stop) {
            if (shm[0] > 0) {
                int n = shm[0];
                int max = shm[1];
                int min = shm[1];
                for (int i = 1; i < n; i++) {
                    if (shm[i + 1] > max) max = shm[i + 1];
                    if (shm[i + 1] < min) min = shm[i + 1];
                }
                shm[n + 1] = min;
                shm[n + 2] = max;
                shm[0] = -1;
            }
            sleep(1);
        }
        munmap(shm, SHM_SIZE);
        exit(EXIT_SUCCESS);
    default:
        // Parent process
        srand(time(NULL));
        while (!stop) {
            int n = random_in_range(5, 10);
            shm[0] = n;
            for (int i = 0; i < n; i++) {
                shm[i + 1] = random_in_range(1, 100);
            }
            while (shm[0] != -1 && !stop) {
                usleep(100000);
            }
            if (stop) break;
            int min = shm[n + 1];
            int max = shm[n + 2];
            printf("Set %d: Minimum = %d, Maximum = %d\n", processed_sets + 1, min, max);
            processed_sets++;

            sleep(1);
        }

        // Waiting child process
        kill(pid, SIGINT);
        wait(NULL);

        printf("Number of datasets processed: %d\n", processed_sets);

        // Deleting a shared memory segment
        munmap(shm, SHM_SIZE);
        shm_unlink(shm_name);
    }

    return 0;
}