/**
 * Password Cracker
 * CS 241 - Spring 2020
 */
#include "cracker2.h"
#include "format.h"
#include "utils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "includes/queue.h"
#include <crypt.h>
#include <stdbool.h>
pthread_mutex_t glob_m = PTHREAD_MUTEX_INITIALIZER;
int correct = 1;
typedef struct p_data {
    char* line;
    char* user_name;
    char* password;
    char* pass_pref;
} p_data;
typedef struct thread_data {
    size_t id;
    size_t num_hashes;
    double elapsed_time;
    size_t t_count;
    char* ret_pass;
    p_data* user_data;
} thread_data;
queue* pass_queue = NULL;

void start_func(void* data) {
    thread_data* t_data = (thread_data*) data;
    size_t id = t_data->id;
	struct crypt_data cdata;
	cdata.initialized = 0;
    p_data* user_data = t_data->user_data;
    if(user_data != NULL) {
		double start_time = getThreadCPUTime();
        long start_ind = 0;
        long count = 0;
        int unknown = strlen(user_data->pass_pref) - getPrefixLength(user_data->pass_pref);
        getSubrange(unknown, t_data->t_count, id, &start_ind, &count);
        // printf("%d, %lu, %lu, %lu, %lu\n", unknown, t_data->t_count, id, start_ind, count);
        char* pass_pref = strdup(user_data->pass_pref);
        int crypt_begin = getPrefixLength(pass_pref);
        setStringPosition(pass_pref + crypt_begin, start_ind);
        v2_print_thread_start(id, user_data->user_name, start_ind, pass_pref);
        // printf("%s\n", pass_pref);
        long index = 0;
        // printf("%lu, %s\n", start_ind, user_data->pass_pref);
        // printf("INSIDE! Thread ID: %lu ID: %lu\n", pthread_self(), id);
        for (index = 0; index < count; index++) {
            char *encrypt_pass = crypt_r(pass_pref, "xx", &cdata);
            if (strcmp(user_data->password, encrypt_pass) == 0) {
                pthread_mutex_lock(&glob_m);
                correct = 0;
                pthread_mutex_unlock(&glob_m);
                t_data->num_hashes = index;
                t_data->elapsed_time = getThreadCPUTime() - start_time;
                v2_print_thread_result(id, index, 0);
                // free(pass_pref);
                t_data->ret_pass = strdup(pass_pref);
                free(pass_pref);
                // printf("%s", pass_pref);
                return;
            }
            pthread_mutex_lock(&glob_m);
            if (correct == 0) {
                // printf("THIS IS THREAD %lu and it is already finished!\n", id);
                t_data->num_hashes = index;
                t_data->elapsed_time = getThreadCPUTime() - start_time;
                // printf("%lf\n", t_data->elapsed_time);
                v2_print_thread_result(id, index, 1);
                free(pass_pref);
                t_data->ret_pass = NULL;
                pthread_mutex_unlock(&glob_m);
                return;
            }
            pthread_mutex_unlock(&glob_m);
            incrementString(pass_pref);
        }
        // user_data->pass_pref = strdup(pass_pref)
        t_data->num_hashes = index;
        t_data->elapsed_time = getThreadCPUTime() - start_time;
        // printf("%lf\n", t_data->elapsed_time); //Check if in sync
        v2_print_thread_result(id, index, 2);
        t_data->ret_pass = NULL;
        free(pass_pref);
    }
}

p_data* get_user_data(char* line) {
        p_data* p_line = malloc(sizeof(p_data));
        p_line->line = strdup(line);
        p_line->user_name = strsep(&p_line->line, " ");
        p_line->password = strsep(&p_line->line, " ");
        p_line->pass_pref = p_line->line;
        return p_line;
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    pthread_t *threads = malloc(sizeof(pthread_t) * thread_count);
    thread_data *t_data = malloc(sizeof(thread_data) * thread_count);
    pass_queue = queue_create(-1);
    char* user_line = NULL;
    size_t len_of_str = 0;
    while (getline(&user_line, &len_of_str, stdin) != -1) {
        int num_char = strlen(user_line);
        if (user_line[num_char - 1] == '\n') {
            user_line[num_char - 1] = '\0';
        }
        p_data* user_data = get_user_data(user_line);
        queue_push(pass_queue, user_data);
    }
    queue_push(pass_queue, NULL);
    p_data* user_data = queue_pull(pass_queue);
    while(user_data != NULL) {
        double start_time = getTime();
        v2_print_start_user(user_data->user_name);
        for (size_t i = 0; i < thread_count; i++) {
            t_data[i].id = i + 1;
            t_data[i].user_data = user_data;
            t_data[i].num_hashes = 0;
            t_data[i].elapsed_time = 0;
            t_data[i].t_count = thread_count;
            t_data[i].ret_pass = NULL;
            pthread_create(threads+i, NULL, (void*)&start_func, &t_data[i]);
            // printf("START! Thread ID: %lu ID: %lu\n", threads[i], t_data[i].id);
        }
        int hash_count = 0;
        double thread_cpu_time = 0;
        char* correct_pass = NULL;
        for (size_t i = 0; i < thread_count; i++) {
            void** retval = NULL;
            pthread_join(threads[i], retval);
            // printf("END! Thread ID: %lu ID: %lu\n", threads[i], t_data[i].id);
            hash_count += t_data[i].num_hashes;
            thread_cpu_time += t_data[i].elapsed_time;
            if (t_data[i].ret_pass != NULL) {
                correct_pass = strdup(t_data[i].ret_pass);
                free(t_data[i].ret_pass);
            }
        }
        double time_taken = getTime() - start_time;
        v2_print_summary(user_data->user_name, correct_pass, hash_count, time_taken, thread_cpu_time, correct);
        free(user_data->user_name);
        if (correct_pass != NULL) {
            free(correct_pass);
        }
        free(user_data);
        user_data = queue_pull(pass_queue);
        correct = 1;
    }
    free(user_line);
    free(threads);
    free(t_data);
    queue_destroy(pass_queue);
    pthread_mutex_destroy(&glob_m);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}