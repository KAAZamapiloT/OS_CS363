#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

// Readers-Writers problem (writers preference avoided):
// - Multiple readers may read concurrently.
// - A writer requires exclusive access (no readers while writing).
// This program creates 3 reader threads and 1 writer thread. Use semaphores
// to synchronize access to the shared "database".

#define NUM_READERS 3
#define NUM_WRITES 5
#define NUM_READ_OPS 8

static sem_t mutex; // for read_count
static sem_t wrt;   // for writers (and first/last reader coordination)
static int read_count = 0;

// Shared resource (simulated database)
static int shared_data = 0;

void msleep(unsigned int ms) {
    usleep(ms * 1000);
}

void *reader(void *arg) {
    int id = (int)(intptr_t)arg;

    for (int i = 0; i < NUM_READ_OPS; ++i) {
        // Simulate arrival time
        msleep(50 + (rand() % 150));

        // Entry section for readers
        sem_wait(&mutex);
        read_count++;
        if (read_count == 1) {
            // first reader locks writers out
            sem_wait(&wrt);
        }
        sem_post(&mutex);

        // Critical section: reading
        printf("[Reader %d] reading shared_data=%d (read_count=%d)\n", id, shared_data, read_count);
        msleep(80 + (rand() % 120));

        // Exit section for readers
        sem_wait(&mutex);
        read_count--;
        if (read_count == 0) {
            // last reader releases writer
            sem_post(&wrt);
        }
        sem_post(&mutex);
    }

    printf("[Reader %d] finished\n", id);
    return NULL;
}

void *writer(void *arg) {
    (void)arg;

    for (int i = 1; i <= NUM_WRITES; ++i) {
        // Simulate doing something before writing
        msleep(100 + (rand() % 200));

        // Writer wants exclusive access
        sem_wait(&wrt);

        // Critical section: writing
        shared_data += 1;
        printf("[Writer] performing write %d, new shared_data=%d\n", i, shared_data);
        msleep(150 + (rand() % 150));

        sem_post(&wrt);
    }

    printf("[Writer] finished\n");
    return NULL;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;

    srand((unsigned)time(NULL));

    if (sem_init(&mutex, 0, 1) != 0) {
        perror("sem_init mutex");
        return 1;
    }
    if (sem_init(&wrt, 0, 1) != 0) {
        perror("sem_init wrt");
        sem_destroy(&mutex);
        return 1;
    }

    pthread_t readers[NUM_READERS];
    pthread_t writer_tid;

    // Create writer
    if (pthread_create(&writer_tid, NULL, writer, NULL) != 0) {
        perror("pthread_create writer");
        sem_destroy(&mutex);
        sem_destroy(&wrt);
        return 1;
    }

    // Create readers
    for (int i = 0; i < NUM_READERS; ++i) {
        if (pthread_create(&readers[i], NULL, reader, (void *)(intptr_t)(i+1)) != 0) {
            perror("pthread_create reader");
            // continue trying to create remaining threads
        }
    }

    // Join threads
    pthread_join(writer_tid, NULL);
    for (int i = 0; i < NUM_READERS; ++i) {
        pthread_join(readers[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&wrt);

    printf("All threads finished. Final shared_data=%d\n", shared_data);
    return 0;
}
