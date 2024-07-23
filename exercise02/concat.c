#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s word1 word2 .. wordN", argv[0]);
		return 0;
	}

	for (int i = 1; i < argc; i++) {
		printf("%s", argv[i]);
	}

	printf("\n");

	return 0;
}