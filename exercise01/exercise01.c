#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void calculate_area(int* sides, int count, const char* process_name) {
	for (int i = 0; i < count; ++i) {
		int side = sides[i];
		int square = side * side;
		printf("%s process: side = %d, square = %d\n", process_name, side, square);
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s side1 side2 ... sideN\n", argv[0]);
		return 1;
	}

	int sides_count = argc - 1;
	int* sides = (int*)malloc(sides_count * sizeof(int));
	if (sides == NULL) {
		perror("Error: malloc.");
		return 1;
	}

	for (int i = 0; i < sides_count; i++) {
		sides[i] = atoi(argv[i + 1]);
	}

	int half = sides_count / 2;
	pid_t pid;
	int rv;

	switch (pid = fork()) {
	case -1:
		perror("fork");
		free(sides);
		exit(EXIT_FAILURE);
	case 0:
		calculate_area(sides + half, sides_count - half, "Child");
		exit(EXIT_SUCCESS);
	default:
		calculate_area(sides, half, "Parent");
		wait(&rv);
	}

	free(sides);
	exit(EXIT_SUCCESS);
}