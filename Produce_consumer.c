#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
// Deadlock-free producer-consumer with proper termination handling
// Uses a sentinel value approach to signal consumers when to stop

#define DEFAULT_BUF_SIZE 5
#define DEFAULT_PER_PRODUCER 10

typedef struct {
    char type[8]; // "audio", "video", or "STOP"
    int id;
} Item;

static Item *buffer = NULL;
static int buf_size = DEFAULT_BUF_SIZE;
static int in = 0, out = 0;

static sem_t empty;
static sem_t full;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static int items_per_producer = DEFAULT_PER_PRODUCER;
static int num_producers = 2;
static int num_consumers = 2;

// Atomic counter for producers finishing
static int producers_finished = 0;
static pthread_mutex_t finish_mutex = PTHREAD_MUTEX_INITIALIZER;

void put_item(const Item *it) {
    buffer[in] = *it;
    in = (in + 1) % buf_size;
}

Item get_item(void) {
    Item it = buffer[out];
    out = (out + 1) % buf_size;
    return it;
}

void *producer(void *arg) {
    const char *type = (const char *)arg;
    
    for (int i = 1; i <= items_per_producer; ++i) {
        Item it;
        snprintf(it.type, sizeof(it.type), "%s", type);
        it.id = i;

        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        put_item(&it);
        printf("[Producer %s] produced %s-%d (in=%d out=%d)\n", 
               type, type, i, in, out);
        pthread_mutex_unlock(&mutex);
        sem_post(&full);

        usleep(100000 + (rand() % 100000)); // 100-200 ms
    }
    
    printf("[Producer %s] finished producing %d items.\n", type, items_per_producer);
    
    // Signal that this producer is done
    pthread_mutex_lock(&finish_mutex);
    producers_finished++;
    int all_done = (producers_finished == num_producers);
    pthread_mutex_unlock(&finish_mutex);
    
    // Last producer sends STOP signals for each consumer
    if (all_done) {
        printf("[Producer %s] Last producer - sending STOP signals.\n", type);
        for (int i = 0; i < num_consumers; ++i) {
            Item stop_signal;
            snprintf(stop_signal.type, sizeof(stop_signal.type), "STOP");
            stop_signal.id = 0;
            
            sem_wait(&empty);
            pthread_mutex_lock(&mutex);
            put_item(&stop_signal);
            printf("[Producer %s] sent STOP signal %d/%d\n", type, i+1, num_consumers);
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
        }
    }
    
    return NULL;
}

void *consumer(void *arg) {
    const char *name = (const char *)arg;
    int consumed = 0;

    while (true) {
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        Item it = get_item();
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
        
        // Check for stop signal
        if (strcmp(it.type, "STOP") == 0) {
            printf("[Consumer %s] received STOP signal, exiting.\n", name);
            break;
        }
        
        consumed++;
        printf("[Consumer %s] playing %s-%d (in=%d out=%d)\n", 
               name, it.type, it.id, in, out);
        
        usleep(150000 + (rand() % 150000)); // 150-300 ms
    }
    
    printf("[Consumer %s] finished consuming %d items.\n", name, consumed);
    return NULL;
}

int main(int argc, char **argv) {
    if (argc >= 2) {
        buf_size = atoi(argv[1]) > 0 ? atoi(argv[1]) : DEFAULT_BUF_SIZE;
    }
    if (argc >= 3) {
        items_per_producer = atoi(argv[2]) > 0 ? atoi(argv[2]) : DEFAULT_PER_PRODUCER;
    }

    buffer = calloc(buf_size, sizeof(Item));
    if (!buffer) {
        perror("calloc");
        return 1;
    }

    srand((unsigned)time(NULL));

    if (sem_init(&empty, 0, buf_size) != 0) {
        perror("sem_init empty");
        free(buffer);
        return 1;
    }
    if (sem_init(&full, 0, 0) != 0) {
        perror("sem_init full");
        sem_destroy(&empty);
        free(buffer);
        return 1;
    }

    pthread_t prod1, prod2, cons1, cons2;

    printf("Starting producer-consumer with buffer size=%d, items_per_producer=%d\n",
           buf_size, items_per_producer);

    // Create producers and consumers
    pthread_create(&prod1, NULL, producer, "audio");
    pthread_create(&prod2, NULL, producer, "video");
    pthread_create(&cons1, NULL, consumer, "Player-1");
    pthread_create(&cons2, NULL, consumer, "Player-2");

    // Wait for all threads
    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);

    // Cleanup
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&finish_mutex);
    free(buffer);

    printf("All done. Total items produced: %d\n", items_per_producer * num_producers);
    return 0;
}