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
