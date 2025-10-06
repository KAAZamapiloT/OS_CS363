#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

// Dining Philosophers - 5 philosophers, 5 forks (shared resources).
// Requirements satisfied:
// - Each philosopher needs two adjacent forks to eat.
// - Use synchronization to prevent deadlock and reduce starvation.
// Solution used:
// - A "room" semaphore initialized to N-1 (4). Philosophers must
//   enter the room before trying to pick forks. This guarantees at
//   least one philosopher can always eat (prevents circular wait -> no deadlock).
// - Each fork is a binary semaphore (mutex-like). Philosophers acquire
//   the two adjacent forks after entering the room, then release them when done.
// - This pattern removes deadlock and provides practical starvation avoidance
//   (no philosopher is permanently blocked if scheduling is fair).

#define N 5
#define DEFAULT_MEALS 3

static sem_t forks[N];
static sem_t room; // allows at most N-1 philosophers to try to pick forks

void msleep(unsigned int ms) {
    usleep(ms * 1000);
}

void *philosopher(void *arg) {
    int id = (int)(intptr_t)arg;
    int left = id;              // left fork index
    int right = (id + 1) % N;   // right fork index
    int meals = DEFAULT_MEALS;

    for (int m = 1; m <= meals; ++m) {
        // Thinking
        printf("[Philosopher %d] thinking (meal %d)\n", id, m);
        msleep(100 + (rand() % 200));

        // Try to enter the dining room (limit concurrency to N-1)
        sem_wait(&room);

        // Pick up forks (order does not matter because room prevents deadlock)
        sem_wait(&forks[left]);
        sem_wait(&forks[right]);

        // Eating
        printf("[Philosopher %d] eating (meal %d) — picked forks %d and %d\n", id, m, left, right);
        msleep(120 + (rand() % 200));

        // Put down forks
        sem_post(&forks[right]);
        sem_post(&forks[left]);

        // Leave the room, allowing others to attempt
        sem_post(&room);

        printf("[Philosopher %d] finished meal %d\n", id, m);
    }

    printf("[Philosopher %d] done (all meals eaten)\n", id);
    return NULL;
}

int main(int argc, char **argv) {
    int meals = DEFAULT_MEALS;
    if (argc >= 2) {
        meals = atoi(argv[1]) > 0 ? atoi(argv[1]) : DEFAULT_MEALS;
    }

    srand((unsigned)time(NULL));

    // Initialize forks (binary semaphores)
    for (int i = 0; i < N; ++i) {
        if (sem_init(&forks[i], 0, 1) != 0) {
            perror("sem_init fork");
            return 1;
        }
    }

    // Room semaphore initialized to N-1 prevents deadlock
    if (sem_init(&room, 0, N - 1) != 0) {
        perror("sem_init room");
        return 1;
    }

    pthread_t threads[N];

    // Create philosopher threads passing their id and meal count via global DEFAULT_MEALS
    for (int i = 0; i < N; ++i) {
        if (pthread_create(&threads[i], NULL, philosopher, (void *)(intptr_t)i) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    // Wait for all philosophers to finish
    for (int i = 0; i < N; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup
    for (int i = 0; i < N; ++i) sem_destroy(&forks[i]);
    sem_destroy(&room);

    printf("All philosophers finished.\n");
    return 0;
}
