/**
 * Teaching Threads
 * CS 241 - Spring 2020
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

/* You might need a struct for each task ... */
static reducer* used_func = NULL;
typedef struct thread_list {
    reducer reduce_func;
    size_t start;
    size_t end;
    int* num_list;
    int result;
} thread_list;
/* You should create a start routine for your threads. */
void start_routine(thread_list *data) {
    int result = (data->result);
    data->reduce_func = *used_func;
    for(size_t i = data->start; i < data->end; i++) {
        result = data->reduce_func(result, data->num_list[i]);
    } 
    (data->result) = result;
}

int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */
    used_func = &reduce_func;
    if (num_threads > list_len) {
        num_threads = list_len;
    }
    size_t num_indices_in_thread = list_len/num_threads;
    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    thread_list* t_data = malloc(sizeof(thread_list) * num_threads);
    for (size_t i = 0; i < num_threads; i++) {
        if (i == num_threads - 1) {
            t_data[i].start = i * num_indices_in_thread;
            t_data[i].end = list_len;
            t_data[i].result = base_case;
            t_data[i].num_list = list;
        } else {
            t_data[i].start = i * num_indices_in_thread;
            t_data[i].end = (i+1) * num_indices_in_thread;
            t_data[i].result = base_case;
            t_data[i].num_list = list;
        }
        pthread_create(threads+i, NULL, (void*) &start_routine, t_data+i);
    }
    int return_res = base_case;
    for (size_t i = 0; i < num_threads; i++) {
        void** retval = NULL;
        pthread_join(threads[i], retval);
        return_res = reduce_func(return_res, t_data[i].result); 
    }

    free(threads);
    free(t_data);
    return return_res;
}
