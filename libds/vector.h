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

