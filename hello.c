#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

//BASIC USES OF THREADS
void *hello(void *arg) {
    printf("Hello, World!\n");
    return NULL;
}
void* print_message(void* arg){
    char* message = (char*)arg;
    printf("%s\n", message);
    return NULL;
    
}

void* square(void*arg){
    int* num = (int*)arg;
    int*result = malloc(sizeof(int));
    *result = (*num) * (*num);
    pthread_exit(result);
}

void *worker(void *arg) {
    int *num = (int *)arg;
    printf("Worker thread received: %d\n", *num);
    return NULL;
}
int main(){
    pthread_t thread;
    pthread_create(&thread, NULL, hello, NULL);
    pthread_join(thread, NULL);
    const char* msg = "This is a message from the thread.";


    pthread_t msg_thread;
    pthread_create(&msg_thread, NULL, print_message, (void*)msg);
    pthread_join(msg_thread, NULL);


    int number = 5;
    pthread_t square_thread;
    pthread_create(&square_thread, NULL, square, (void*)&number);
    void* square_result;
    pthread_join(square_thread, &square_result);
    printf("Square of %d is %d\n", number, *(int*)square_result);
    free(square_result);

    pthread_t worker_thread[6];

    int numbers[6] = {10, 20, 30, 40, 50, 60};
    for (int i = 0; i < 6; i++) {
        pthread_create(&worker_thread[i], NULL, worker, (void*)&numbers[i]);}

    for (int i = 0; i < 6; i++) {
        pthread_join(worker_thread[i], NULL);}

    return 0;
}