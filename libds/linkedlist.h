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
