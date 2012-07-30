#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashmap.h"

static void
ds_hashmap_put(struct DSHashMap *hash, void *data, char* skey, int32_t ikey,
               int8_t type);

static void
ds_hashmap_remove(struct DSHashMap *hash, char *skey, int32_t ikey, int8_t type,
                  bool free_data, bool free_string_keys);

static void *
ds_hashmap_get(struct DSHashMap *hash, char *skey, int32_t ikey, int8_t type);

static struct DSHashItem *
ds_hashmap_get_item(struct DSHashMap *hash, char *skey,
                    int32_t ikey, int8_t type);

static int32_t
ds_hashmap_compare_keys(void *k1, void *k2);

static bool
is_key_match(struct DSHashKey *key, char *skey, int32_t ikey, int8_t type);

static uint64_t
hash_value(char *skey, int32_t ikey, int8_t type);

static uint64_t
hash_string(char* str);

static uint64_t
hash_int(int32_t integer);

struct DSHashMap *
ds_hashmap_create()
{
    struct DSHashMap *hash;
    int32_t i;

    hash = malloc(sizeof(*hash));
    assert(hash);

    hash->keys = ds_vector_create();
    hash->buckets = malloc(DS_HASHMAP_BUCKETS * sizeof(*hash->buckets));
    assert(hash->buckets);

    for (i = 0; i < DS_HASHMAP_BUCKETS; ++i)
        hash->buckets[i] = NULL;

    return hash;
}

void
ds_hashmap_free(struct DSHashMap *hash, bool free_data, bool free_string_keys)
{
    struct DSHashItem *item, *item2;
    int32_t i;

    for (i = 0; i < DS_HASHMAP_BUCKETS; ++i) {
        item = hash->buckets[i];
        while(item) {
            item2 = item->next;

            if (free_string_keys && item->key->keytype == DS_HASHMAP_KEY_STRING)
                free(item->key->key.s);

            if (free_data)
                free(item->data);

            free(item->key);
            free(item);
            item = item2;
        }
    }

    ds_vector_free_no_data(hash->keys);
    free(hash->buckets);
    free(hash);
}

void
ds_puts(struct DSHashMap *hash, char *key, void *data)
{
    ds_hashmap_put_str(hash, key, data);
}

void
ds_hashmap_put_str(struct DSHashMap *hash, char *key, void *data)
{
    ds_hashmap_put(hash, data, key, 0, DS_HASHMAP_KEY_STRING);
}

void
ds_puti(struct DSHashMap *hash, int32_t key, void *data)
{
    ds_hashmap_put_int(hash, key, data);
}

void
ds_hashmap_put_int(struct DSHashMap *hash, int32_t key, void *data)
{
    ds_hashmap_put(hash, data, NULL, key, DS_HASHMAP_KEY_INT);
}

static void
ds_hashmap_put(struct DSHashMap *hash, void *data, char* skey, int32_t ikey,
               int8_t type)
{
    struct DSHashItem *item, *last;
    uint64_t hashval;

    hashval = hash_value(skey, ikey, type);
    item = hash->buckets[hashval];
    last = NULL;

    while (item != NULL) {
        if (is_key_match(item->key, skey, ikey, type)) {
            if (item->data != NULL)
                free(item->data);

            item->data = data;

            return;
        }

        if (item->next == NULL)
            last = item;

        item = item->next;
    }

    item = malloc(sizeof(*item));
    assert(item);
    item->next = NULL;
    item->data = data;

    item->key = malloc(sizeof(*item->key));
    assert(item->key);
    item->key->hash = hash;
    item->key->keytype = type;

    switch(type) {
    case DS_HASHMAP_KEY_STRING:
        item->key->key.s = skey;
        break;
    case DS_HASHMAP_KEY_INT:
        item->key->key.i = ikey;
        break;
    }

    if (last == NULL)
        hash->buckets[hashval] = item;
    else
        last->next = item;

    ds_vector_append(hash->keys, item->key);
}

void
ds_hashmap_remove_str(struct DSHashMap *hash, char *key, bool free_data,
                      bool free_string_keys)
{
    ds_hashmap_remove(hash, key, 0, DS_HASHMAP_KEY_STRING, free_data,
                      free_string_keys);
}

void
ds_hashmap_remove_int(struct DSHashMap *hash, int32_t key, bool free_data)
{
    ds_hashmap_remove(hash, NULL, key, DS_HASHMAP_KEY_INT, free_data, false);
}

static void
ds_hashmap_remove(struct DSHashMap *hash, char *skey, int32_t ikey, int8_t type,
                  bool free_data, bool free_string_keys)
{
    struct DSHashItem *item, *last;
    uint64_t hashval;

    hashval = hash_value(skey, ikey, type);

    item = hash->buckets[hashval];
    last = NULL;
    while (item != NULL) {
        if (is_key_match(item->key, skey, ikey, type)) {
            int32_t i;

            if (last == NULL)
                hash->buckets[hashval] = NULL;
            else
                last->next = item->next;

            /* find the key in the keys vector */
            for (i = 0; i < hash->keys->size; ++i) {
                struct DSHashKey *k;

                k = (struct DSHashKey*) ds_vector_get(hash->keys, i);
                if (is_key_match(k, skey, ikey, type)) {
                    ds_vector_remove(hash->keys, i);
                    break;
                }
            }

            if (free_data && item->data != NULL)
                free(item->data);
            if (free_string_keys && type == DS_HASHMAP_KEY_STRING)
                free(item->key->key.s);

            free(item->key);
            free(item);

            return;
        }

        last = item;
        item = item->next;
    }
}

void *
ds_hashmap_get_key(struct DSHashKey *key)
{
    switch(key->keytype) {
    case DS_HASHMAP_KEY_STRING:
        return ds_gets(key->hash, key->key.s);
    case DS_HASHMAP_KEY_INT:
        return ds_geti(key->hash, key->key.i);
    }

    assert(false);
}

void *
ds_gets(struct DSHashMap *hash, char *key)
{
    return ds_hashmap_get_str(hash, key);
}

void *
ds_hashmap_get_str(struct DSHashMap *hash, char *key)
{
    return ds_hashmap_get(hash, key, 0, DS_HASHMAP_KEY_STRING);
}

void *
ds_geti(struct DSHashMap *hash, int32_t key)
{
    return ds_hashmap_get_int(hash, key);
}

void *
ds_hashmap_get_int(struct DSHashMap *hash, int32_t key)
{
    return ds_hashmap_get(hash, NULL, key, DS_HASHMAP_KEY_INT);
}

static void *
ds_hashmap_get(struct DSHashMap *hash, char *skey, int32_t ikey, int8_t type)
{
    struct DSHashItem *item;

    if ((item = ds_hashmap_get_item(hash, skey, ikey, type)) != NULL)
        return item->data;

    return NULL;
}

static struct DSHashItem *
ds_hashmap_get_item(struct DSHashMap *hash, char *skey, int32_t ikey,
                    int8_t type)
{
    struct DSHashItem *item;
    uint64_t hashval; 

    if (type == DS_HASHMAP_KEY_STRING && skey == NULL)
        return NULL;

    hashval = hash_value(skey, ikey, type);

    item = hash->buckets[hashval];
    while (item != NULL) {
        if (is_key_match(item->key, skey, ikey, type))
            return item;

        item = item->next;
    }

    return NULL;
}

void
ds_hashmap_print_keys(struct DSHashMap *hash)
{
    int32_t i;

    for (i = 0; i < hash->keys->size; ++i) {
        struct DSHashKey *key;

        key = ds_vector_get(hash->keys, i);

        switch(key->keytype) {
        case DS_HASHMAP_KEY_STRING:
            printf("%s\n", key->key.s);
            break;
        case DS_HASHMAP_KEY_INT:
            printf("%d\n", key->key.i);
            break;
        }
    }
}

void
ds_hashmap_print_keyvals(struct DSHashMap *hash, char* (tostring)(void*))
{
    int32_t i;

    for (i = 0; i < hash->keys->size; ++i) {
        struct DSHashKey *key;

        key = ds_vector_get(hash->keys, i);

        switch(key->keytype) {
        case DS_HASHMAP_KEY_STRING:
            printf("(%s, %s)\n", key->key.s, 
                   tostring(ds_gets(hash, key->key.s)));
            break;
        case DS_HASHMAP_KEY_INT:
            printf("(%d, %s)\n", key->key.i,
                   tostring(ds_geti(hash, key->key.i)));
            break;
        }
    }
}

void
ds_hashmap_sort_keys(struct DSHashMap *hash)
{
    ds_hashmap_sort_by(hash, ds_hashmap_compare_keys);
}

void
ds_hashmap_sort_by(struct DSHashMap *hash, int32_t (compare)(void*, void*))
{
    ds_vector_sort(hash->keys, compare);
}

/* A comparison function for sorting keys by name */
static int32_t
ds_hashmap_compare_keys(void *vk1, void *vk2)
{
    struct DSHashKey *k1, *k2;
    char *s1 = NULL, *s2 = NULL;
    char i1[11], i2[11];

    k1 = (struct DSHashKey*) vk1;
    k2 = (struct DSHashKey*) vk2;

    switch(k1->keytype) {
    case DS_HASHMAP_KEY_STRING:
        s1 = k1->key.s;
        break;
    case DS_HASHMAP_KEY_INT:
        sprintf(i1, "%d", k1->key.i);
        break;
    }

    switch(k2->keytype) {
    case DS_HASHMAP_KEY_STRING:
        s2 = k2->key.s;
        break;
    case DS_HASHMAP_KEY_INT:
        sprintf(i2, "%d", k2->key.i);
        break;
    }

    if (s1 != NULL && s2 != NULL)
        return strcmp(s1, s2);
    else if (s1 == NULL && s2 != NULL)
        return strcmp(i1, s2);
    else if (s1 != NULL && s2 == NULL)
        return strcmp(s1, i2);
    else if (s1 == NULL && s2 == NULL)
        return strcmp(i1, i2);
    
    assert(false);
}

static bool
is_key_match(struct DSHashKey *key, char *skey, int32_t ikey, int8_t type)
{
    return (type == DS_HASHMAP_KEY_STRING &&
            key->keytype == DS_HASHMAP_KEY_STRING &&
            strcmp(skey, key->key.s) == 0)
           ||
           (type == DS_HASHMAP_KEY_INT &&
            key->keytype == DS_HASHMAP_KEY_INT &&
            ikey == key->key.i);
}

static uint64_t
hash_value(char *skey, int32_t ikey, int8_t type)
{
    assert((skey == NULL && type != DS_HASHMAP_KEY_STRING) ||
           (skey != NULL && type == DS_HASHMAP_KEY_STRING));

    switch(type) {
    case DS_HASHMAP_KEY_STRING:
        return hash_string(skey);
        break;
    case DS_HASHMAP_KEY_INT:
        return hash_int(ikey);
        break;
    }

    assert(false);
}

/* djb2 hashing algorithm where k = 33.
 * Taken from http://www.cse.yorku.ca/~oz/hash.html */
static uint64_t
hash_string(char* str)
{
    uint64_t hash = 5381;
    char c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % DS_HASHMAP_BUCKETS;
}

static uint64_t
hash_int(int32_t integer)
{
    return (uint64_t) integer % DS_HASHMAP_BUCKETS;
}
