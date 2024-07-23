#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_SIZE 1024

void printline() {
	printf(">>  ");
	fflush(stdout);
}

int main() {
	char input[MAX_SIZE];
	char* args[MAX_SIZE];
	char* token;
	
	while (1) {
		printline();

		if (fgets(input, MAX_SIZE, stdin) == NULL) {
			perror("Error: fgets.");
			continue;
		}

		input[strcspn(input, "\n")] = '\0';

		int i = 0;
		token = strtok(input, " ");
		while (token != NULL) {
			args[i++] = token;
			token = strtok(NULL, " ");
		}
		args[i] = NULL;

		if (args[0] != NULL && strcmp(args[0], "exit") == 0) {
			exit(EXIT_SUCCESS);
		}

		pid_t pid;
		switch (pid = fork()) {
		case -1:
			perror("Error: fork");
			exit(EXIT_FAILURE);
		case 0:
			execvp(args[0], args);
			perror("Error: execvp");
			exit(EXIT_FAILURE);
		case 1:
			wait(NULL);
		}
	}

	exit(EXIT_SUCCESS);
}