
#include "hash_table.h"
#include "prime.h"

static ht_item HT_DELETED_ITEM = {NULL, NULL}; //we create an item marked as delete for all the deleted items
const int HT_PRIME_1=151;
const int HT_PRIME_2=163;
const int HT_INITIAL_BASE_SIZE=53;


// Custom implementation of xmalloc
void* xmalloc(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// Custom implementation of xcalloc
void* xcalloc(size_t num, size_t size) {
    void* ptr = calloc(num, size);
    if (ptr == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

//ITEM CREATION
static ht_item* ht_new_item(const char* k, const char* v) 
{
    ht_item* i = xmalloc(sizeof(ht_item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}

//HASH TABLE CREATION
static ht_hash_table* ht_new_sized(const int base_size) //function for resizing the table 
{
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);

    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));

    // Initialize the mutex
    if(pthread_mutex_init(&ht->lock, NULL) != 0){
        fprintf(stderr, "Mutex initialization failed\n");
        free(ht->items);
        free(ht);
        exit(EXIT_FAILURE);
    }
    return ht;
}

ht_hash_table* ht_new()  //funtion to create the initial table with base size
{
    printf("HASH TABLE CREATED!!!\n");
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

//ITEM DELETION
static void ht_del_item(ht_item* i) 
{
    free(i->key);
    free(i->value);
    free(i);
}

//TABLE DELETION
void ht_del_hash_table(ht_hash_table* ht) 
{
    for (int i = 0; i < ht->size; i++) 
    {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) 
        {
            ht_del_item(item);
        }
    }
    //Destroy the mutext before freeing the table memory
    pthread_mutex_destroy(&ht->lock);
    free(ht->items);
    free(ht);
}

//RESIZE FUNCTION
static void ht_resize(ht_hash_table* ht, const int base_size) 
{
    void ht_insert_no_lock(ht_hash_table *ht, const char *key, const char *value);

    if (base_size < HT_INITIAL_BASE_SIZE) 
    {
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->size; i++) 
    {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) 
        {
            ht_insert_no_lock(new_ht, item->key, item->value);
        }
    }
    //SWAPPING OF ATTRIBUTES
    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items 
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    free(new_ht->items);
    pthread_mutex_destroy(&new_ht->lock);
    free(new_ht);
}

//RESIZE UP AND DOWN FUCNITONS
static void ht_resize_up(ht_hash_table* ht) 
{
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) 
{
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}

//HASH FUNCTION USING PRIME NUMBER (a),BUCKET SIZE(table size (m))
static int ht_hash(const char* s, const int a, const int m) 
{
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) 
    {
        long power_of_a = 1;
        for(int j=0; j < len_s - (i + 1); j++){
            power_of_a = (power_of_a * a) % m;
        }
        hash = (hash + (power_of_a * s[i])) % m;
    }
    return (hash + m) % m;
}

//TO GET HASH VALUE CONSIDERING COLLISION 
static int ht_get_hash(const char* s, const int num_buckets, const int attempt) 
{
    const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
    const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);
    const int hash_val =  (hash_a + (attempt * (hash_b + 1))) % num_buckets;
    return (hash_val % num_buckets + num_buckets) % num_buckets;
}

// This is the internal insertion logic without locking or resizing.
// It's used by ht_resize to avoid deadlocks and recursion.
void ht_insert_no_lock(ht_hash_table *ht, const char *key, const char *value){
    ht_item *item = ht_new_item(key, value);
    int index = ht_get_hash(item->key, ht->size,0);
    ht_item *cur_item = ht->items[index];
    int i = 1;

        while (cur_item != NULL && cur_item != &HT_DELETED_ITEM) {
            if (strcmp(cur_item->key, key) == 0) {
                ht_del_item(cur_item);
                ht->items[index] = item;
                // Note: count doesn't change on update
                return;
            }
            index = ht_get_hash(item->key, ht->size, i);
            cur_item = ht->items[index];
            i++;
        }
    ht->items[index] = item;
    ht->count++;
}


//INSERT INTO HASH TBALE OR UPDATE IF KEY ALREADY EXISTS (Thread safe)
void ht_insert(ht_hash_table* ht, const char* key, const char* value) 
{   
    pthread_mutex_lock(&ht->lock);  // <-- Take the lock
    //to resize up if load>0.7
    const int load = ht->count * 100 / ht->size;
    if (load > 70) 
    {
        ht_resize_up(ht);
    }
    
    // Perform the actual insert
    ht_insert_no_lock(ht, key, value);
    printf("\nThread %ld: Item Inserted!!!\n", (long)pthread_self());
    pthread_mutex_unlock(&ht->lock);    // <-- Unlock, release the lock
}

//SEARCH ITEM 
char* ht_search(ht_hash_table* ht, const char* key) 
{
    pthread_mutex_lock(&ht->lock);  // <-- Take the lock

    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1;
    char *found_value = NULL;
    while (item != NULL ) 
    {
        if(item != &HT_DELETED_ITEM)
        {
            if (strcmp(item->key, key) == 0) 
            {
                found_value = strdup(item->value);
                break;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
    pthread_mutex_unlock(&ht->lock);    // <-- Unlock, release the lock

    return found_value; // <-- single point of return
}

//DELETE A ELEMENT FROM HASH TABLE USING IT'S KEY
void ht_delete(ht_hash_table* ht, const char* key) 
{
    pthread_mutex_lock(&ht->lock);  // <-- Take the lock
    //to resize down if load<0.1
    const int load = ht->count * 100 / ht->size;
    if (load < 10) 
    {
        ht_resize_down(ht);
    }

    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];
    int i = 1, found = 0;
    while (item != NULL) 
    {
        if (item != &HT_DELETED_ITEM) 
        {
            if (strcmp(item->key, key) == 0) 
            {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
                ht->count--;
                found = 1;
                break;
            }
        }
        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }

    if(found){
        printf("Thread %ld: Item Deleted!!!\n", (long)pthread_self());
    } else  {
        printf("Thread %ld: Item not in the table!!!\n", (long)pthread_self());
    }
    pthread_mutex_unlock(&ht->lock);
}