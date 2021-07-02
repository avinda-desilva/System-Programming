/**
 * Savvy Scheduler
 * CS 241 - Spring 2020
 * avindad2
 * ajariw2
 * janaks2
 */
#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"

/**
 * The struct to hold the information about a given job
 */
typedef struct _job_info {
    int id;
    double arrive_time;
    double running_time;
    int priority; 
    double recent_time;
    bool has_started;
    double expire_time;
    double start_time;
    double elapsed_time;
    /* TODO: Add any other information and bookkeeping you need into this
     * struct. */
} job_info;

static int job_ct;
static double total_wait;
static double total_turnaround;
static double total_response; 

void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    // Put any additional set up code you may need here
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
    // TODO: Implement me!
    job* job_a = (job *)a;
    job* job_b = (job *)b;
    job_info *info_a = (job_info *)job_a->metadata;
    job_info *info_b = (job_info *)job_b->metadata;
    if (info_a->arrive_time >= info_b->arrive_time) {
        return 1;
    } else if (info_a->arrive_time < info_b->arrive_time) {
        return -1;
    }
    return 0;
}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
    // TODO: Implement me!
    job* job_a = (job *)a;
    job* job_b = (job *)b;
    job_info *info_a = (job_info *)job_a->metadata;
    job_info *info_b = (job_info *)job_b->metadata;
    if (info_a->priority > info_b->priority) {
        return 1;
    } else if (info_a->priority < info_b->priority) {
        return -1;
    } else if (info_a->priority == info_b->priority) {
        int ret_num = break_tie(a, b);
        return ret_num;
    }
    return 0;
}

int comparer_psrtf(const void *a, const void *b) {
    // TODO: Implement me!
    job* job_a = (job *)a;
    job* job_b = (job *)b;
    job_info *info_a = (job_info *)job_a->metadata;
    job_info *info_b = (job_info *)job_b->metadata;
    if (info_a->running_time - info_a->elapsed_time > info_b->running_time - info_b->elapsed_time) {
        return 1;
    } else if (info_a->running_time - info_a->elapsed_time < info_b->running_time - info_b->elapsed_time) {
        return -1;
    } else if (info_a->running_time - info_a->elapsed_time == info_b->running_time - info_b->elapsed_time) {
        int ret_num = break_tie(a, b);
        return ret_num;
    }
    return 0;
}

int comparer_rr(const void *a, const void *b) {
    // TODO: Implement me!
    job* job_a = (job *)a;
    job* job_b = (job *)b;
    job_info *info_a = (job_info *)job_a->metadata;
    job_info *info_b = (job_info *)job_b->metadata;
    if (info_a->recent_time > info_b->recent_time) {
        return 1;
    } else if (info_a->recent_time < info_b->recent_time) {
        return -1;
    } else if (info_a->recent_time == info_b->recent_time) {
        int ret_num = break_tie(a, b);
        return ret_num;
    }
    return 0;
}

int comparer_sjf(const void *a, const void *b) {
    // TODO: Implement me!
    job* job_a = (job *)a;
    job* job_b = (job *)b;
    job_info *info_a = (job_info *)job_a->metadata;
    job_info *info_b = (job_info *)job_b->metadata;
    if (info_a->running_time > info_b->running_time) {
        return 1;
    } else if (info_a->running_time < info_b->running_time) {
        return -1;
    } else if (info_a->running_time == info_b->running_time) {
        int ret_num = break_tie(a, b);
        return ret_num;
    }
    return 0;
}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int job_number, double time,
                       scheduler_info *sched_data) {
    job_ct++;
    job_info* new_info = malloc(sizeof(job_info));
    new_info->arrive_time = time;
    new_info->start_time = time;
    new_info->expire_time = time;
    new_info->has_started = false;
    new_info->running_time = sched_data->running_time;
    new_info->priority = sched_data->priority;
    new_info->id = job_number;
    new_info->recent_time = 0;
    new_info->elapsed_time = 0;
    newjob->metadata = new_info;
    priqueue_offer(&pqueue, newjob);
    // TODO: Implement me!
}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    // TODO: Implement me!
    job* curr_job = priqueue_peek(&pqueue);
    if (curr_job == NULL) {
        return NULL;
    }
    if (job_evicted == NULL) {
        return curr_job;
    }
    job_info* evict_info = job_evicted->metadata;
    evict_info->elapsed_time += time - evict_info->recent_time;
    evict_info->recent_time = time;
    if (evict_info->has_started == false) {
        evict_info->expire_time = time - 1;
        evict_info->has_started = true;
    }
    switch (pqueue_scheme) {
        case PPRI:
            curr_job = (job *)priqueue_poll(&pqueue);
            priqueue_offer(&pqueue, curr_job);
            return priqueue_peek(&pqueue);
        case PSRTF:
            curr_job = (job *)priqueue_poll(&pqueue);
            priqueue_offer(&pqueue, curr_job);
            return priqueue_peek(&pqueue);
        case RR:
            curr_job = (job *)priqueue_poll(&pqueue);
            priqueue_offer(&pqueue, curr_job);
            return priqueue_peek(&pqueue);
        default:
            return job_evicted;
        }
}

void scheduler_job_finished(job *job_done, double time) {
    // TODO: Implement me!
    job_info* done_info = job_done->metadata;
    total_response += done_info->expire_time - done_info->arrive_time;
    total_wait += time - done_info->running_time - done_info->arrive_time;
    total_turnaround += time - done_info->arrive_time;
    free(done_info);
    priqueue_poll(&pqueue);

}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    // TODO: Implement me!
    return total_wait / job_ct;
}

double scheduler_average_turnaround_time() {
    // TODO: Implement me!
    return total_turnaround / job_ct;
}

double scheduler_average_response_time() {
    // TODO: Implement me!
    return total_response / job_ct;
}

void scheduler_show_queue() {
    // OPTIONAL: Implement this if you need it!
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}
