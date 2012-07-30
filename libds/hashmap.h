#ifndef __LIBDS_HASHMAP_H__
#define __LIBDS_HASHMAP_H__

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "vector.h"

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

