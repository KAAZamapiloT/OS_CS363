#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct {
	int *arr;
	int start;
	int end;
	int sum;
} SumArgs;

void *sum_array(void *args) {
	SumArgs *data = (SumArgs *)args;
	data->sum = 0;
	for (int i = data->start; i < data->end; ++i) {
		data->sum += data->arr[i];
	}
	return NULL;
}

int main() {
	int L = 10;
	int arr[10] = {1,2,3,4,5,6,7,8,9,10};
    int Single_total_sum = 0;
	SumArgs args1 = {arr, 0, L/2, 0};
	SumArgs args2 = {arr, L/2, L, 0};

	pthread_t t1, t2;
	pthread_create(&t1, NULL, sum_array, &args1);
	pthread_create(&t2, NULL, sum_array, &args2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	int total_sum = args1.sum + args2.sum;
    for(int i=0;i<10;i++){
        Single_total_sum += arr[i];
    }
    printf("Single-threaded sum: %d\n", Single_total_sum);
	printf("Total sum: %d\n", total_sum);
	return 0;
}
