#include <bits/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // for sleep

#include "hash_table.h"

typedef struct {
    int thread_id;
    int num_items;
    ht_hash_table *ht;
    double insert_time_sec;
    double search_time_sec;
} worker_args;

double get_time_sec(){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}


ht_hash_table* a; // Global hash table

// Worker function for each thread
void* worker(void* arg) {
    worker_args * args = (worker_args*)arg;
     
    // Each thread inserts its own set of keys
    // ---- BENCHMARK INSERT PHASE ---- 
    double start_insert = get_time_sec();
    for (int i = 0; i < args->num_items; i++) {
        char key[50];
        char value[50];
        // Create unique keys for each thread
        sprintf(key, "key_t%d_i%d", args->thread_id, i);
        sprintf(value, "value_t%d_i%d", args->thread_id, i);
        printf("Thread %ld: inserting => key: %s: value :%s\n", (long)pthread_self(), key, value);
        ht_insert(a, key, value);
    }
    double end_insert = get_time_sec();
    args->insert_time_sec = end_insert - start_insert;
    
    // Each thread searches for items it inserted
    // ---- BENCHMARK SEARCH PHASE ----
    double start_search = get_time_sec(); 
    for (int i = 0; i < args->num_items; i++) {
        char key[50];
        sprintf(key, "key_t%d_i%d", args->thread_id, i);
        char* found_value = ht_search(a, key);
        if (found_value == NULL) {
            printf("Thread %d: ERROR! Did not find key %s\n", args->thread_id, key);
        } else {
            // Remember to free the value returned by ht_search
            printf("Thread %ld: Searched => key: %s: value :%s\n", (long)pthread_self(), key, found_value);
            free(found_value);
        }
    }
    double end_search = get_time_sec();
    args->search_time_sec = end_search - start_search;

    return NULL;
}

int main(int argc, char* argv[]) {
    if(argc != 3){
        fprintf(stderr, "Usage: %s <num_threads> <items_per_thread>\n", argv[0]);
        return 1;
    }
    const int num_threads = atoi(argv[1]);
    const int items_per_thread = atoi(argv[2]);

    if(num_threads <= 0 || items_per_thread <= 0 ){
        fprintf(stderr,"Number of threads and items must be a positve number.\n");
        return 1;
    }
    a = ht_new();
    pthread_t threads[num_threads];
    worker_args args[num_threads];
    
    printf("--- BENCHMARKING ---\n");
    printf("Threads: %d\n", num_threads);
    printf("Items per Thread: %d\n", items_per_thread);
    printf("Total Items: %d\n", num_threads * items_per_thread);
    printf("----------------------\n");

    double start_time = get_time_sec();
    // Create threads
    for (int i = 0; i < num_threads; i++) {
        args[i].thread_id = i;
        args[i].num_items = items_per_thread;
        args[i].ht = a;
        args[i].insert_time_sec = 0.0;
        args[i].search_time_sec = 0.0;
        if (pthread_create(&threads[i], NULL, worker, &args[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }
    
    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            return 1;
        }
    }
    double end_time = get_time_sec();
    double total_duration = end_time - start_time;

    // --- PRINT RESULTS ---
    printf("\n--- PER-THREAD TIMINGS ---\n");
    for(int i=0;i<num_threads;i++){
        printf("Thread %2d: Insertion took %.4f s, Searches took %.4f s\n",
            args[i].thread_id, args[i].insert_time_sec, args[i].search_time_sec);
    }

    printf("\n--- AGGREGATE RESULTS ---\n");
    const int total_items = num_threads * items_per_thread;
    printf("Final hash table count: %d\n", a->count);
    printf("Total wall-clock time: %.4f s\n", total_duration);

    // Calculate throughput (total operations / total time)
    double throughput = total_items / total_duration;
    printf("Overall insertion throughput: %.2f inserts/sec\n", throughput);
    
    // Clean up
    ht_del_hash_table(a);

    return 0;
}