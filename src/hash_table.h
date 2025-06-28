#ifndef HASH_TABLE_H
#define HASH_TABLE_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h> // <-- For multi-threading

typedef struct {
    char* key;
    char* value;
} ht_item;
typedef struct {
    int base_size;
    int size;
    int count;
    ht_item** items;
    pthread_mutex_t lock;   // <-- Mutex (Mutual Exclusion) when a thread takes the lock
                            // only it can access the hash table (critical section) other
                            // thread have to wait for the thread to release the lock
} ht_hash_table;

void* xmalloc(size_t size);
void* xcalloc(size_t num, size_t size);

ht_hash_table* ht_new();
void ht_insert(ht_hash_table* ht, const char* key, const char* value);
char* ht_search(ht_hash_table* ht, const char* key);
void ht_delete(ht_hash_table* h, const char* key);
void ht_del_hash_table(ht_hash_table* ht);

#endif