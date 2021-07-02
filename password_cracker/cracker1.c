/**
 * Password Cracker
 * CS 241 - Spring 2020
 */
#include "cracker1.h"
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
int summary[2]; // 0 is correct and 1 is failed
typedef struct p_data {
    char* line;
    char* user_name;
    char* password;
    char* pass_pref;
} p_data;
queue* pass_queue = NULL;

void start_func(void* data) {
    size_t id = (size_t) data;
	struct crypt_data cdata;
	cdata.initialized = 0;
    p_data* p_line = queue_pull(pass_queue);
    while(p_line != NULL) {
		v1_print_thread_start(id, p_line->user_name);
		double start_time = getThreadCPUTime();
        int crypt_begin = getPrefixLength(p_line->pass_pref);
        setStringPosition(p_line->pass_pref + crypt_begin, 0);
        char *encrypt_pass = crypt_r(p_line->pass_pref, "xx", &cdata);
        size_t count = 1;
        int correct = 0;
		while(strcmp(p_line->password, encrypt_pass) != 0){
			if(incrementString(p_line->pass_pref + crypt_begin) == 0){
                correct = 1;
				break;
			}
            encrypt_pass = crypt_r(p_line->pass_pref, "xx", &cdata);
            count++;
		}
        double time_taken = getThreadCPUTime() - start_time;
        v1_print_thread_result(id, p_line->user_name, p_line->pass_pref, count, time_taken, correct);
        pthread_mutex_lock(&glob_m);
        summary[correct]++;
        pthread_mutex_unlock(&glob_m);
        free(p_line->user_name);
        free(p_line);
        p_line = queue_pull(pass_queue);
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
    for (size_t i = 0; i < thread_count; i++) {
        pthread_create(threads+i, NULL, (void*)&start_func, (void*)i+1);
        queue_push(pass_queue, NULL);
    }
    for (size_t i = 0; i < thread_count; i++) {
        void** retval = NULL;
        pthread_join(threads[i], retval);
    }
    v1_print_summary(summary[0], summary[1]);
    free(user_line);
    free(threads);
    queue_destroy(pass_queue);
    pthread_mutex_destroy(&glob_m);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
