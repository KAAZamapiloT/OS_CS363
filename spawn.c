#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_index(void *arg) {
	int idx = *(int *)arg;
	printf("Thread index: %d\n", idx);
	return NULL;
}

int main() {
	int N;
	printf("Enter N: ");
	scanf("%d", &N);

	pthread_t *threads = malloc(N * sizeof(pthread_t));
	int *indices = malloc(N * sizeof(int));

	for (int i = 0; i < N; ++i) {
		indices[i] = i;
		pthread_create(&threads[i], NULL, print_index, &indices[i]);
	}

	for (int i = 0; i < N; ++i) {
		pthread_join(threads[i], NULL);
	}

	printf("DONE\n");
	free(threads);
	free(indices);
	return 0;
}