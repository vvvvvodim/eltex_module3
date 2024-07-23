#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s num1 num2 .. numN\n", argv[0]);
		return 1;
	}
	int sum;

	for (int i = 1; i < argc; i++) {
		sum += atoi(argv[i]);
	}

	printf("Sum = %d\n", sum);

	return 0;
}