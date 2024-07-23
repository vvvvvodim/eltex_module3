#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s num1 num2 .. numN", argv[0]);
		return 0;
	}

	int max = -100;
	for (int i = 1; i < argc; i++) {
		if (atoi(argv[i]) > max) max = atoi(argv[i]);
	}

	printf("Max = %d\n", max);

	return 0;
}