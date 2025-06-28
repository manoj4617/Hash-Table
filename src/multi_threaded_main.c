#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // for sleep

#include "hash_table.h"

#define NUM_THREADS 6
#define ITEMS_PER_THREAD 100

ht_hash_table* a; // Global hash table

// Worker function for each thread
void* worker(void* arg) {
    int thread_id = *(int*)arg;
    
    // Each thread inserts its own set of keys
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        char key[50];
        char value[50];
        // Create unique keys for each thread
        sprintf(key, "key_t%d_i%d", thread_id, i);
        sprintf(value, "value_t%d_i%d", thread_id, i);
        printf("Thread %ld: inserting => key: %s: value :%s\n", (long)pthread_self(), key, value);
        ht_insert(a, key, value);
    }
    
    // Each thread searches for items it inserted
    for (int i = 0; i < ITEMS_PER_THREAD; i++) {
        char key[50];
        sprintf(key, "key_t%d_i%d", thread_id, i);
        char* found_value = ht_search(a, key);
        if (found_value == NULL) {
            printf("Thread %d: ERROR! Did not find key %s\n", thread_id, key);
        } else {
            // Remember to free the value returned by ht_search
            printf("Thread %ld: Searched => key: %s: value :%s\n", (long)pthread_self(), key, found_value);
            free(found_value);
        }
    }

    free(arg); // Free the memory allocated for the thread ID
    return NULL;
}

int main() {
    a = ht_new();
    pthread_t threads[NUM_THREADS];
    
    printf("Starting %d threads...\n", NUM_THREADS);

    // Create threads
    for (int i = 0; i < NUM_THREADS; i++) {
        int* thread_id = malloc(sizeof(int));
        *thread_id = i;
        if (pthread_create(&threads[i], NULL, worker, thread_id) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }

    printf("\nAll threads finished.\n");
    printf("Final hash table count: %d\n", a->count);
    printf("Expected count: %d\n", NUM_THREADS * ITEMS_PER_THREAD);

    if (a->count == NUM_THREADS * ITEMS_PER_THREAD) {
        printf("SUCCESS: Final count matches expected count.\n");
    } else {
        printf("FAILURE: Final count does not match expected count.\n");
    }

    ht_del_hash_table(a);

    return 0;
}