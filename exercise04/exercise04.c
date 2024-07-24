#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

void generate_numbers(int pipefd, int count) {
	srand(time(NULL));
	for (int i = 0; i < count; ++i) {
		int num = rand();
		write(pipefd, &num, sizeof(int));
	}
	close(pipefd);
}

void receive_numbers(int pipefd, int count) {
	FILE* file = fopen("output.txt", "w");
	if (file == NULL) {
		perror("Error: fopen.");
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < count; ++i) {
		int num;
		read(pipefd, &num, sizeof(int));
		printf("Received number: %d\n", num);
		fprintf(file, "Received number: %d\n", num);
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

	pid_t pid;
	switch (pid = fork()) {
	case -1:
		perror("Error: fork.");
		exit(EXIT_FAILURE);
	case 0:
		close(pipefd[0]);
		generate_numbers(pipefd[1], count);
		exit(EXIT_SUCCESS);
	default:
		close(pipefd[1]);
		receive_numbers(pipefd[0], count);
		wait(NULL);
	}

	exit(EXIT_SUCCESS);
}