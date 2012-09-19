#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"

/* The number of times to compute the first 1000 prime numbers. */
#define JOBS 100000

/* When true, the first 1000 prime numbers will be computed sequentially
 * in a single thread `JOBS` times.
 * This may be useful when comparing a concurrent approach to a sequential
 * approach. */
#define SEQUENTIAL false

/* The size of the thread-safe queue buffer (capacity). It doesn't have much
 * effect when all threads are CPU bound. */
#define BUFFER 10

/* The number of producer threads to start.
 * If `0` is used, a producer thread will be started for each locial
 * CPU available. */
static int num_producers = 1;

/* The number of producer threads to start.
 * If `0` is used, a producer thread will be started for each locial
 * CPU available. */
static int num_consumers = 0;

/* End user configurable parameters. */

/* The number of logical CPUs. If no CPUs are detected, this is set to 1.
 * (The value is retrieved from `sysconf`. */
static int cpus;

/* The producer thread. It simply sends incrementing integers to the
 * job queue. */
static void * producer(void *data);

/* The consumer thread. It reads values from the job queue and computes the
 * first 1000 prime numbers for each value received. */
static void * consumer(void *data);

/* Creates a new job given a thread identification number, and the job and
 * results queues. */
static struct job job_create(int id,
                             struct DSQueue *queue, struct DSQueue *results);

/* Computes the first 1000 prime numbers `JOBS` times sequentially. */
static void sequential();

/* Calculates the first `limit` prime numbers. */
static int calc_primes(int limit);

/* A job holds all the necessary information for the producer and consumer
 * threads to do work. Namely, the producer thread sends values to `job_queue`,
 * while the consumer thread receives values from `job_queue`. The
 * consumer thread also sends values to `results_queue`, which are read in the
 * main thread. */
struct job {
    struct DSQueue *job_queue;
    struct DSQueue *results_queue;
    char *id;
};

int main(void)
{
    struct DSQueue *job_queue, *results_queue;
    pthread_t *producers, *consumers;
    struct job *prod_jobs, *cons_jobs;
    int i;
    int *result, result_sum;

    if (SEQUENTIAL) {
        sequential();
        exit(0);
    }

    /* If there is a bad value from sysconf, just assume 1. */
    cpus = (int) sysconf(_SC_NPROCESSORS_ONLN);
    cpus = cpus == 0 ? 1 : cpus;

    /* Compute the number of producer/consumer threads to start. */
    num_producers = num_producers == 0 ? cpus : num_producers;
    num_consumers = num_consumers == 0 ? cpus : num_consumers;

    /* Initialize thread safe queues. `job_queue` has a capacity equivalent
     * to the buffer size set by the user. `results_queue` always has a capacity
     * equivalent to the number of consumer threads so that every consumer
     * can send a value to `results_queue` without blocking. */
    job_queue = ds_queue_create(BUFFER);
    results_queue = ds_queue_create(num_consumers);

    assert(producers = malloc(num_producers * sizeof(*producers)));
    assert(consumers = malloc(num_consumers * sizeof(*consumers)));
    assert(prod_jobs = malloc(num_producers * sizeof(*prod_jobs)));
    assert(cons_jobs = malloc(num_consumers * sizeof(*cons_jobs)));

    /* Create `num_producers` jobs and threads.
     * Similarly for `num_consumers`. */
    for (i = 0; i < num_producers; i++) {
        prod_jobs[i] = job_create(i, job_queue, results_queue);
        pthread_create(&(producers[i]), NULL, producer,
                      (void*) &(prod_jobs[i]));
    }
    for (i = 0; i < num_consumers; i++) {
        cons_jobs[i] = job_create(i, job_queue, results_queue);
        pthread_create(&(consumers[i]), NULL, consumer,
                       (void*) &(cons_jobs[i]));
    }

    /* Wait for all of the producers to finish producing. */
    for (i = 0; i < num_producers; i++)
        assert(0 == pthread_join(producers[i], NULL));

    /* Now that the producers are done, no more values will be sent to
     * `job_queue`, and therefore, it should be closed. (Values can still
     * be read from a closed queue.) */
    ds_queue_close(job_queue);

    /* Free the producer jobs. */
    for (i = 0; i < num_producers; i++)
        free(prod_jobs[i].id);
    free(prod_jobs);

    /* Now wait for the consumers to finish consuming. */
    for (i = 0; i < num_consumers; i++)
        assert(0 == pthread_join(consumers[i], NULL));

    /* Now that the consumers are done, no more values will be sent to
     * `results_queue`, and therefore, it should be closed. */
    ds_queue_close(results_queue);

    /* Free the consumer jobs. */
    for (i = 0; i < num_consumers; i++)
        free(cons_jobs[i].id);
    free(cons_jobs);

    /* Read all values in `results_queue`, and sum them up to get the total
     * number of consumed items. */
    result_sum = 0;
    while (NULL != (result = (int*) ds_queue_get(results_queue))) {
        result_sum += *result;
        free(result);
    }

    /* Print out the total number of produced and consumed items.
     * In this example, these numbers should always be equal.
     * N.B. Watch out for integer division! */
    printf("Total produced: %d\n", (JOBS / num_producers) * num_producers);
    printf("Total consumed: %d\n", result_sum);

    free(consumers);
    free(producers);
    ds_queue_free(job_queue);
    ds_queue_free(results_queue);

    return 0;
}

void *
producer(void *data)
{
    struct job *job;
    int i;
    int *item;
    int cnt = 0;

    job = (struct job *) data;
    for (i = 0; i < JOBS / num_producers; i++) {
        assert(item = malloc(sizeof(*item)));
        ds_queue_put(job->job_queue, item);
        cnt++;
    }

    printf("Thread %s produced %d items.\n", job->id, cnt);
    return NULL;
}

void *
consumer(void *data)
{
    struct job *job;
    int *cnt;
    int *item;
    int total_primes = 0;

    assert(cnt = malloc(sizeof(*cnt)));
    *cnt = 0;

    job = (struct job *) data;
    while (NULL != (item = (int*) ds_queue_get(job->job_queue))) {
        (*cnt)++;
        total_primes += calc_primes(1000);
        free(item);
    }

    printf("Thread %s consumed %d items (%d primes).\n",
            job->id, *cnt, total_primes);
    ds_queue_put(job->results_queue, (void*) cnt);
    return NULL;
}

void
sequential()
{
    int i;

    for (i = 0; i < JOBS; i++)
        calc_primes(1000);
}

struct job
job_create(int id, struct DSQueue *job_queue, struct DSQueue *results_queue)
{
    struct job job;
    char *idstr;

    assert(idstr = malloc(10 * sizeof(*idstr)));
    sprintf(idstr, "%d", id);

    job.job_queue = job_queue;
    job.results_queue = results_queue;
    job.id = idstr;

    return job;
}

/* Adapted from http://goo.gl/7PxJt */
int
calc_primes(int limit) {
    int num = 1;
    int primes = 0;

    for (num = 1; num <= limit; num++) { 
        int i = 2; 
        while(i <= num) { 
            if(num % i == 0)
                break;
            i++; 
        }
        if(i == num)
            primes++;
    }

    return primes;
}
