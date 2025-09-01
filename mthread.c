#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
//RACE CONDITIONS 
volatile int counter = 0;
volatile int n_counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
void *inc(void *arg) {
    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&lock);
        n_counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}
void *increment_counter(void *arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;
    }
    return NULL;
}

int main(){
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, increment_counter, NULL);
    pthread_create(&thread2, NULL, increment_counter, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_t thread3, thread4;
    pthread_create(&thread3, NULL, inc, NULL);
    pthread_create(&thread4, NULL, inc, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    printf("Final counter value without race coditions: %d\n", n_counter);
    printf("Final counter value with race conditions: %d\n", counter);
    return 0;
}