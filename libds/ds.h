#ifndef __LIBDS_HASHMAP_H__
#define __LIBDS_HASHMAP_H__

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

/* The number of buckets to allocate */
static const int32_t DS_HASHMAP_BUCKETS = 1000000;

/* key types */
#define DS_HASHMAP_KEY_INT 1
#define DS_HASHMAP_KEY_STRING 2

struct DSHashMap {
    /* storing the keys isn't strictly necessary for a hash map, but it makes
     * iterating over the elements in a hash map much more efficient. */
    struct DSVector *keys;
    struct DSHashItem **buckets;
};

struct DSHashItem {
    struct DSHashKey *key;
    void *data;
    struct DSHashItem *next;
};

struct DSHashKey {
    struct DSHashMap *hash; /* useful to avoid global scoping a hash */
    int8_t keytype;
    union {
        int32_t i;
        char* s;
    } key;
};

/**
 * Some shortcut functions:
 * ds_puts -> ds_hashmap_put_str
 * ds_puti -> ds_hashmap_put_int
 * ds_gets -> ds_hashmap_get_str
 * ds_geti -> ds_hashmap_get_int
 */
void
ds_puts(struct DSHashMap *hash, char *key, void *data);

void
ds_puti(struct DSHashMap *hash, int32_t key, void *data);

void *
ds_gets(struct DSHashMap *hash, char *key);

void *
ds_geti(struct DSHashMap *hash, int32_t key);

/**
 * Initializes a HashMap.
 */
struct DSHashMap *
ds_hashmap_create();

/**
 * Frees all memory associated with the hash map.
 * If 'free_data' is true, user data will be freed too.
 * If 'free_string_keys' is true, then string keys will be freed too.
 */
void
ds_hashmap_free(struct DSHashMap *hash, bool free_data, bool free_string_keys);

/**
 * Adds an element with string key to hash map.
 */
void
ds_hashmap_put_str(struct DSHashMap *hash, char *key, void *data);

/**
 * Adds an element with integer key to hash map.
 */
void
ds_hashmap_put_int(struct DSHashMap *hash, int32_t key, void *data);

/**
 * Removes an element using a string key.
 * If 'free_data' is true, then all user data will be freed.
 * If 'free_string_keys' is true, then all string will be freed.
 */
void
ds_hashmap_remove_str(struct DSHashMap *hash, char *key, bool free_data,
                      bool free_string_keys);

/**
 * Similarly as 'ds_hashmap_remove_str' but with an integer as a key.
 * Since an integer is a key, there is never a need to free a string key.
 */
void
ds_hashmap_remove_int(struct DSHashMap *hash, int32_t key, bool free_data);

/**
 * Gets an element with string key from hash map.
 */
void *
ds_hashmap_get_str(struct DSHashMap *hash, char *key);

/**
 * Gets an element with integer key from hash map.
 */
void *
ds_hashmap_get_int(struct DSHashMap *hash, int32_t key);

/**
 * Gets an element if you have a DSHashKey struct.
 */
void *
ds_hashmap_get_key(struct DSHashKey *key);

/**
 * Sorts the keys in alphanumeric order.
 */
void
ds_hashmap_sort_keys(struct DSHashMap *hash);

/**
 * Sorts the keys using the provided compare function.
 * The compare function is defined as follows:
 *      compare(k1, k2) < 0 when k1 < k2
 *      compare(k1, k2) = 0 when k1 = k2
 *      compare(k1, k2) > 0 when k1 > k2
 *
 * NOTE: The comparison doesn't have to be on the keys directly. One could
 * use ds_get and compare those values.
 */
void
ds_hashmap_sort_by(struct DSHashMap *hash, int32_t (compare)(void*, void*));

/**
 * Prints a list of key names/numbers for debugging purposes.
 */
void
ds_hashmap_print_keys(struct DSHashMap *hash);

/**
 * Prints a list of key,value pairs for debugging purposes.
 * This needs a function that turns values into strings.
 */
void
ds_hashmap_print_keyvals(struct DSHashMap *hash, char* (tostring)(void*));

#endif

#ifndef __LIBDS_LINKEDLIST_H__
#define __LIBDS_LINKEDLIST_H__

/* #define NDEBUG */

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

struct DSLinkedList {
    uint32_t length;
    struct DSListNode *first;
    struct DSListNode *last;
};

struct DSListNode {
    void* data;
    struct DSListNode *prev;
    struct DSListNode *next;
};

struct DSListIter {
    struct DSListNode *current;
    bool reverse;
};

/**
 * Initializes memory for a DSLinkedList.
 * 'ds_list_free' should be called when done with the list.
 */
struct DSLinkedList * 
ds_list_create();

/**
 * Frees all memory associated with the list and its nodes.
 */
void 
ds_list_free(struct DSLinkedList *lst);

/**
 * Frees all memory associated with just the list.
 * Does not free data memory.
 */
void 
ds_list_free_no_data(struct DSLinkedList *lst);

/**
 * Prepends a new node with 'data' to the beginning of the list.
 * Runs in constant time.
 */
void
ds_list_prepend(struct DSLinkedList *lst, void *data);

/**
 * Appends a new node with 'data' to the end of the list.
 * Runs in constant time.
 */
void
ds_list_append(struct DSLinkedList *lst, void *data);

/**
 * Creates a new node with 'data' after 'after'. If 'after' is NULL,
 * then 'data' will be pushed on to the beginning of the list.
 * Runs in constant time.
 */
void 
ds_list_insert(struct DSLinkedList *lst, struct DSListNode *after, void *data);

/**
 * Copies a list's structure. Data is not copied.
 */
struct DSLinkedList *
ds_list_copy(struct DSLinkedList *lst);

/**
 * Removes an element from a list but does NOT free the data's memory.
 * Runs in constant time.
 */
void
ds_list_remove(struct DSLinkedList *lst, struct DSListNode *remove);

/**
 * Removes an element from a list but DOES free's the data's memory.
 * Runs in constant time.
 */
void
ds_list_destroy(struct DSLinkedList *lst, struct DSListNode *remove);

/**
 * Convenience function to map a funcion across all elements in the list.
 */
void
ds_list_map(struct DSLinkedList *lst, void (func)(void*));

/**
 * A function that sort the given list in place.
 */
void
ds_list_sort(struct DSLinkedList *lst, int32_t (compare)(void*, void*));

#endif
#ifndef __LIBDS_QUEUE_H__
#define __LIBDS_QUEUE_H__

/*
 * DSQueue is a thread-safe queue that has no limitation on the number of
 * threads that can call ds_queue_push and ds_queue_pop simultaneously.
 * That is, it supports a multiple producer and multiple consumer model.
 */

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>

/* DSQueue implements a thread-safe queue using a fairly standard
 * circulur buffer.
 *
 * DSQueue is an opaque type. None of its members should be accessed
 * by the user. */
struct DSQueue {
    /* An array of elements in the queue. */
    void **buf;

    /* The position of the first element in the queue. */
    uint32_t pos;

    /* The number of items currently in the queue.
     * When `length` = 0, ds_queue_pop will block.
     * When `length` = `capacity`, ds_queue_push will block. */
    uint32_t length;

    /* The total number of allowable items in the queue */
    uint32_t capacity;

    /* When true, the queue has been closed. A run-time error will occur
     * if a value is sent to a closed queue. */
    bool closed;

    /* Guards the modification of `length` (a condition variable) and `pos`. */
    pthread_mutex_t mutate;

    /* A condition variable that is pinged whenever `length` has changed or
     * when the queue has been closed. */
    pthread_cond_t cond_length;
};

/* Allocates a new DSQueue with a buffer size of the capacity given. */
struct DSQueue *
ds_queue_create(uint32_t buffer_capacity);

/* Frees all data used to create a DSQueue. It should only be called after
 * a call to ds_queue_close to make sure all 'pops' are terminated before
 * destroying mutexes/condition variables.
 *
 * Note that the data inside the buffer is not freed. */
void
ds_queue_free(struct DSQueue *queue);

/* Returns the current length (number of items) in the queue. */
int
ds_queue_length(struct DSQueue *queue);

/* Returns the capacity of the queue. This is always equivalent to the
 * size of the initial buffer capacity. */
int
ds_queue_capacity(struct DSQueue *queue);

/* Closes a queue. A closed queue cannot add any new values.
 *
 * When a queue is closed, an empty queue will always be empty.
 * Therefore, `ds_queue_pop` will return NULL and not block when
 * the queue is empty. Therefore, one can traverse the items in a queue
 * in a thread-safe manner with something like:
 *
 *  void *queue_item;
 *  while (NULL != (queue_item = ds_queue_pop(queue)))
 *      do_something_with(queue_item);
 */
void
ds_queue_close(struct DSQueue *queue);

/* Adds new values to a queue (or "sends values to a consumer").
 * `ds_queue_push` cannot be called with a queue that has been closed. If
 * it is, an assertion error will occur. 
 * If the queue is full, `ds_queue_push` will block until it is not full,
 * in which case the value will be added to the queue. */
void
ds_queue_push(struct DSQueue *queue, void *item);

/* Reads new values from a queue (or "receives values from a producer").
 * `ds_queue_pop` will block if the queue is empty until a new value has been
 * added to the queue with `ds_queue_push`. In which case, `ds_queue_pop` will
 * return the next item in the queue.
 * `ds_queue_pop` can be safely called on a queue that has been closed (indeed,
 * this is probably necessary). If the queue is closed and not empty, the next
 * item in the queue is returned. If the queue is closed and empty, it will
 * always be empty, and therefore NULL will be returned immediately. */
void *
ds_queue_pop(struct DSQueue *queue);

#endif
#ifndef __LIBDS_VECTOR_H__
#define __LIBDS_VECTOR_H__

#include <assert.h>
#include <stdint.h>

/* some private constants for vector tuning */
static const int32_t DS_VECTOR_BASE_CAPACITY = 10;
static const float DS_VECTOR_EXPAND_RATIO = 1.5;

struct DSVector {
    int32_t size;
    int32_t capacity;
    void** data;
};

/**
 * Creates a vector with DS_VECTOR_BASE_CAPACITY.
 * ds_vector_free or ds_vector_free_no_data will need to be called
 * when done with the vector to avoid memory leaks.
 */
struct DSVector *
ds_vector_create();

/**
 * Creates a vector with the given capacity.
 * (N.B. This vector will still automatically increase in size if necessary.)
 */
struct DSVector *
ds_vector_create_capacity(int32_t capacity);

/**
 * Free's a vector AND its data.
 */
void
ds_vector_free(struct DSVector *vec);

/**
 * Free's just the vector's representation. Data is NOT freed.
 */
void
ds_vector_free_no_data(struct DSVector *vec);

/**
 * Copies a vector, but not the data.
 * Make sure you free it!
 */
struct DSVector *
ds_vector_copy(struct DSVector *vec);

/**
 * Adds an element to the end of a vector.
 * Runs in constant time.
 */
void
ds_vector_append(struct DSVector *vec, void* data);

/**
 * Places an element at index i, and shifts the rest of the vector
 * to the right by one. If index == size of vector, then the element
 * will be appended to the end of the vector.
 */
void
ds_vector_insert(struct DSVector *vec, void* data, int32_t index);

/**
 * Removes an element from the vector at some index.
 * Also shifts everything to right of index to the left.
 * Does *NOT* free the data.
 */
void
ds_vector_remove(struct DSVector *vec, int32_t index);

/**
 * Gets an element at index i from a vector.
 */
void *
ds_vector_get(struct DSVector *vec, int32_t index);

/**
 * Sets an elements at index to data.
 * No data is freed.
 */
void
ds_vector_set(struct DSVector *vec, void* data, int32_t index);

/**
 * Swaps the data from vector element i to vector element j.
 */
void
ds_vector_swap(struct DSVector *vec, int32_t i, int32_t j);

/**
 * Higher-order function to map func over every element in vector.
 */
void
ds_vector_map(struct DSVector *vec, void (func)(void*));

/**
 * Higher-order function to find the first element in the vector
 * such that compare(needle, element) == 0.
 * Where compare is defined as:
 *      compare(a, b) < 0 when a < b
 *      compare(a, b) = 0 when a = b
 *      compare(a, b) > 0 when a > b
 *
 * Returns an unsigned -1 if needle is not found.
 */
int32_t 
ds_vector_find(struct DSVector *vec, void* needle,
               int32_t (compare)(void*, void*));

/**
 * Runs quicksort on the vector in place.
 */
void
ds_vector_sort(struct DSVector *vec, int32_t (compare)(void*, void*));

#endif

