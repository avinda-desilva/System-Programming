/**
 * Critical Concurrency
 * CS 241 - Spring 2020
 */
#include "barrier.h"
#include <stdlib.h>
#include <stdbool.h>
static bool atLimit = false;
// The returns are just for errors if you want to check for them.
int barrier_destroy(barrier_t *barrier) {
    int error = 0;
    pthread_mutex_destroy(&barrier->mtx);
    pthread_cond_destroy(&barrier->cv);
    return error;
}

int barrier_init(barrier_t *barrier, unsigned int num_threads) {
    int error = 0;
    //barrier = (barrier_t *)malloc(sizeof(barrier_t));
    barrier->n_threads = num_threads;
    barrier->times_used = 0;
    barrier->count = 0;
    pthread_mutex_init(&barrier->mtx, NULL);
    pthread_cond_init(&barrier->cv, NULL);
    atLimit = true;
    return error;
}

int barrier_wait(barrier_t *barrier) {
    pthread_mutex_lock(&barrier->mtx);
    while (atLimit == false) {
        pthread_cond_wait(&barrier->cv, &barrier->mtx);
    }
    barrier->count++;
    if (barrier->count == barrier->n_threads) {
        barrier->count--;
        barrier->times_used++;
        atLimit = false;
        pthread_cond_broadcast(&barrier->cv);
    } else {
        while (atLimit == true) {
            pthread_cond_wait(&barrier->cv, &barrier->mtx);
        }
        barrier->count--;
        if (barrier->count == 0) {
            atLimit = true;
            pthread_cond_broadcast(&barrier->cv);
        }
    }
    pthread_mutex_unlock(&barrier->mtx);
    return 0;
}
