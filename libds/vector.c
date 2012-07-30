#include <stdlib.h>
#include <string.h>

#include "vector.h"

/* private function to check and possibly expand a vector's capacity */
static void
ds_vector_maybe_expand(struct DSVector *vec);

/* private helper functions for quicksort */
static void
ds_vector_quicksort(struct DSVector *vec, int32_t left, int32_t right,
                    int32_t (compare)(void *, void*));

static int32_t
ds_vector_partition(struct DSVector *vec, int32_t left, int32_t right,
                    int32_t pivot,
                    int32_t (compare)(void *, void*));

struct DSVector *
ds_vector_create()
{
    return ds_vector_create_capacity(DS_VECTOR_BASE_CAPACITY);
}

struct DSVector *
ds_vector_create_capacity(int32_t capacity)
{
    struct DSVector *vec;

    vec = malloc(sizeof(*vec));
    assert(vec);

    vec->size = 0;
    vec->capacity = capacity;
    vec->data = malloc(vec->capacity * sizeof(*vec->data));
    assert(vec->data);

    return vec;
}

void
ds_vector_free(struct DSVector *vec)
{
    int32_t i;

    for (i = 0; i < vec->size; ++i)
        free(vec->data[i]);

    free(vec->data);
    free(vec);
}

void 
ds_vector_free_no_data(struct DSVector *vec)
{
    free(vec->data);
    free(vec);
}

struct DSVector *
ds_vector_copy(struct DSVector *vec)
{
    struct DSVector *copy;
    int32_t i;

    copy = ds_vector_create_capacity(vec->capacity);
    for (i = 0; i < vec->size; ++i)
        ds_vector_append(copy, vec->data[i]);

    return copy;
}

void
ds_vector_append(struct DSVector *vec, void* data)
{
    ds_vector_maybe_expand(vec);
    vec->data[vec->size++] = data;
}

void
ds_vector_insert(struct DSVector *vec, void* data, int32_t index)
{
    int32_t i;

    /* the index is allowed to be vec->size */
    if (index > vec->size)
        return;

    ds_vector_maybe_expand(vec);

    for (i = vec->size - 1; i >= index && i < (i + 1); --i)
        vec->data[i + 1] = vec->data[i];

    vec->data[index] = data;

    ++vec->size;
}

void
ds_vector_remove(struct DSVector *vec, int32_t index)
{
    int32_t i;

    if (index >= vec->size)
        return;

    --vec->size;
    for (i = index; i < vec->size; ++i)
        vec->data[i] = vec->data[i + 1];
}

void *
ds_vector_get(struct DSVector *vec, int32_t index)
{
    if (index >= vec->size)
        return NULL;

    return vec->data[index];
}

void
ds_vector_set(struct DSVector *vec, void* data, int32_t index)
{
    if (index >= vec->size)
        return;

    vec->data[index] = data;
}

void
ds_vector_swap(struct DSVector *vec, int32_t i, int32_t j)
{
    void *temp;

    if (i >= vec->size || j >= vec->size)
        return;

    temp = vec->data[i];
    vec->data[i] = vec->data[j];
    vec->data[j] = temp;
}

void
ds_vector_map(struct DSVector *vec, void (func)(void*))
{
    int32_t i;

    for (i = 0; i < vec->size; ++i)
        func(vec->data[i]);
}

int32_t
ds_vector_find(struct DSVector *vec, void* needle,
               int32_t (compare)(void*, void*))
{
    int32_t i;

    for (i = 0; i < vec->size; ++i)
        if (compare(needle, vec->data[i]) == 0)
            return i;

    return -1;
}

/** 
 * I've implemented my own Quicksort (based on the Wikipedia entry on
 * Quicksort) because I didn't like how the standard library's version
 * of qsort forces you to define the comparison function. (Namely, that
 * the arguments are void** instead of void*.)
 */
void
ds_vector_sort(struct DSVector *vec, int32_t (compare)(void*, void*))
{
    ds_vector_quicksort(vec, 0, vec->size - 1, compare);
}

static void
ds_vector_quicksort(struct DSVector *vec, int32_t left, int32_t right,
                    int32_t (compare)(void *, void*))
{
    int32_t pivot;

    if (left >= right)
        return;

    pivot = (left + right) / 2;
    pivot = ds_vector_partition(vec, left, right, pivot, compare);

    ds_vector_quicksort(vec, left, pivot - 1, compare);
    ds_vector_quicksort(vec, pivot + 1, right, compare);
}

static int32_t
ds_vector_partition(struct DSVector *vec, int32_t left, int32_t right,
                    int32_t pivot,
                    int32_t (compare)(void *, void*))
{
    void *pivot_val, *temp;
    int32_t store_ind, i;

    pivot_val = vec->data[pivot];
    vec->data[pivot] = vec->data[right];
    vec->data[right] = pivot_val;

    store_ind = left;

    for (i = left; i < right; ++i)
        if (compare(vec->data[i], pivot_val) < 0) {
            temp = vec->data[i];
            vec->data[i] = vec->data[store_ind];
            vec->data[store_ind] = temp;
            ++store_ind;
        }

    temp = vec->data[store_ind];
    vec->data[store_ind] = vec->data[right];
    vec->data[right] = temp;

    return store_ind;
}

static void
ds_vector_maybe_expand(struct DSVector *vec)
{
    if (vec->size < vec->capacity)
        return;

    vec->capacity *= DS_VECTOR_EXPAND_RATIO;
    vec->data = realloc(vec->data, vec->capacity * sizeof(vec->data));
    assert(vec->data);
}

