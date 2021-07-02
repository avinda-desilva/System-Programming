/**
 * Parallel Make
 * CS 241 - Spring 2020
 */
#include "format.h"
#include "graph.h"
#include "vector.h"
#include "queue.h"
#include "dictionary.h"
#include "parmake.h"
#include "parser.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <pthread.h>
pthread_mutex_t glob_m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t glob_cv = PTHREAD_COND_INITIALIZER;
static graph* rule_graph = NULL;
static dictionary* hasVisited = NULL;
static struct stat curr, neighb;
typedef struct thread_data {
    size_t id;
} thread_data;
static queue* rule_queue;

int isCyclic(char* key) {
    if (rule_graph == NULL || key == NULL) {
        return 0;
    }
    if (hasVisited == NULL) {
        hasVisited = shallow_to_shallow_dictionary_create();
    }
        char* node = key;
        if (!dictionary_contains(hasVisited, node)) {
            int visited = 1;
            dictionary_set(hasVisited, node, &visited);
            vector* neighbors = graph_neighbors(rule_graph, node);
            for (size_t i = 0; i < vector_size(neighbors); i++) {
                if (isCyclic(vector_get(neighbors, i))) {
                    dictionary_clear(hasVisited);
                    vector_destroy(neighbors);
                    return 1;
                }
            }
            vector_destroy(neighbors);
        } else {
            dictionary_clear(hasVisited);
            return 1;
        }
    dictionary_clear(hasVisited);
    return 0;
}

int isSatisfied(char* curr_targ) {
    bool canExecute = false;
    if (access(curr_targ, F_OK) < 0) {
        canExecute = true;
    }
    vector *neighbors = graph_neighbors(rule_graph, curr_targ);
    // rule_t* curr_rule = (rule_t *) graph_get_vertex_value(rule_graph, curr_targ);
    // printf("CURR RULE %s\n", curr_targ);
    for (size_t i = 0; i < vector_size(neighbors); i++) {
        char* neighbor = vector_get(neighbors, i);
        rule_t *curr_rule = (rule_t *) graph_get_vertex_value(rule_graph, neighbor);

        if (canExecute == false && access(curr_targ, F_OK) == 0 && access(curr_rule->target, F_OK) == 0) {
            stat(curr_targ, &curr);
            stat(curr_rule->target, &neighb);
            if (difftime(curr.st_mtime, neighb.st_mtime) < 0) {
                canExecute = true;
            }
        }
        // printf("RUUUUUULEEEEEEEE: %s %d\n", curr_rule->target, curr_rule->state);
        if (curr_rule->state == 2) {
            return 1;
        }
        if (isSatisfied(curr_rule->target)) {
            return 1;
        } else {
            return 0;
        }
        // if (canExecute && !(curr_rule->state)) {
        // rule_t *test = (rule_t *) graph_get_vertex_value(rule_graph, curr_targ);
        // if (!(test->state)) {
        //     printf("PUSHING RULE: %s\n", test->target);
        //     queue_push(rule_queue, test);
        //     test->state = 1;
        // }
        // if (!(curr_rule->state)) {
        //     printf("PUSHING RULE: %s\n", curr_rule->target);
        //     queue_push(rule_queue, curr_rule);
        //     curr_rule->state = 1;
        // }
        // }
    }
    return 0;
}

int onethread(char* curr_targ) {
    bool canExecute = false;
    if (access(curr_targ, F_OK) < 0) {
        canExecute = true;
    }
    vector *neighbors = graph_neighbors(rule_graph, curr_targ);
    for (size_t i = 0; i < vector_size(neighbors); i++) {
        char* neighbor = vector_get(neighbors, i);
        rule_t *curr_rule = (rule_t *) graph_get_vertex_value(rule_graph, neighbor);
        if (canExecute == false && access(neighbor, F_OK) < 0) {
            canExecute = true;
        } else if (canExecute == false && access(curr_targ, F_OK) == 0 && access(neighbor, F_OK) == 0) {
            stat(curr_targ, &curr);
            stat(neighbor, &neighb);
            if (difftime(curr.st_mtime, neighb.st_mtime) < 0) {
                canExecute = true;
            }
        }
        if (!(curr_rule->state) && onethread(neighbor)) {
            // rule_destroy(curr_rule);
            return 1;
        }
        if (curr_rule->state == 2) {
            // printf("CFGOOOOOO\n");
        }
        // rule_destroy(curr_rule);
    }
    // printf("%s\n", curr_targ);
    rule_t* curr_rule = (rule_t *) graph_get_vertex_value(rule_graph, curr_targ);
    if (canExecute && !(curr_rule->state)) {
        for (size_t i = 0; i < vector_size(curr_rule->commands); i++) {
            if (system(vector_get(curr_rule->commands, i)) != 0) {
                vector* dependent = graph_antineighbors(rule_graph, curr_targ);
                for (size_t i = 0; i < vector_size(dependent); i++) {
                    rule_t* dep_rule = (rule_t *) graph_get_vertex_value(rule_graph, (char*) vector_get(dependent, i));
                    dep_rule->state = 2;
                }
                vector_destroy(dependent);
                // printf("%s\n", (char *) vector_get(graph_antineighbors(rule_graph, curr_targ), 0));
                break;
            }
        }
        curr_rule->state = 1;
    } else {
        curr_rule->state = 1;
    }
    // rule_destroy(curr_rule);
    return 0;
}

int canRun(char* curr_targ) {
    bool canExecute = false;
    if (access(curr_targ, F_OK) < 0) {
        canExecute = true;
    }
    vector *neighbors = graph_neighbors(rule_graph, curr_targ);
    rule_t* curr_rule = (rule_t *) graph_get_vertex_value(rule_graph, curr_targ);
    // printf("CURR RULE %s\n", curr_targ);
    pthread_mutex_lock(&glob_m);
    if (vector_size(neighbors) < 1 && curr_rule->state == 0) {
        // printf("NO DEPENDENCIES FOR: %s\n", curr_targ);
        queue_push(rule_queue, curr_rule);
        curr_rule->state = 1;
        pthread_mutex_unlock(&glob_m);
    } else {
        pthread_mutex_unlock(&glob_m);
    }
    for (size_t i = 0; i < vector_size(neighbors); i++) {
        char* neighbor = vector_get(neighbors, i);
        rule_t *curr_rule = (rule_t *) graph_get_vertex_value(rule_graph, neighbor);
        if (canExecute == false && access(curr_targ, F_OK) == 0 && access(curr_rule->target, F_OK) == 0) {
            stat(curr_targ, &curr);
            stat(curr_rule->target, &neighb);
            if (difftime(curr.st_mtime, neighb.st_mtime) < 0) {
                canExecute = true;
            }
        }
        canRun(curr_rule->target);
    }
    // bool quit_loop = false;
    // while (!quit_loop) {
    //     quit_loop = true;
    // for (int i = vector_size(neighbors) - 1; i >= 0; i--) {
    //     char* neighbor = vector_get(neighbors, i);
    //     rule_t *curr_rule = (rule_t *) graph_get_vertex_value(rule_graph, neighbor);
    //     pthread_mutex_lock(&glob_m);
    //     if ((curr_rule->state) != 2) {
    //         printf("WAITING ON: %s\n", curr_rule->target);
    //         // quit_loop = false;
    //         pthread_cond_wait(&glob_cv, &glob_m);
    //     }
    //     pthread_mutex_unlock(&glob_m);
    // }
    // }
    // }
    vector_destroy(neighbors);
    // // rule_t* curr_rule = (rule_t *) graph_get_vertex_value(rule_graph, curr_targ);
    // if (canExecute && !(curr_rule->state)) {
    //     if (!(curr_rule->state)) {
    //         // printf("CURRENT RULE: %s\n", curr_rule->target);
    //         // queue_push(rule_queue, curr_rule);
    //         curr_rule->state = 1;
    //     }
    //     pthread_mutex_lock(&glob_m);
    //     if (!(curr_rule->state)) {
    //         vector* neighbors = graph_neighbors(rule_graph, curr_rule->target);
    //         for (size_t i = 0; i < vector_size(neighbors); i++) {
    //             char* neighbor = vector_get(neighbors, i);
    //             rule_t* neighb_rule = (rule_t *) graph_get_vertex_value(rule_graph, neighbor);
    //             if (!neighb_rule->state) {
    //                 neighb_rule->state = 1;
    //             }
    //         }
    //         vector_destroy(neighbors);
    //     }
    //     pthread_mutex_unlock(&glob_m);
    // } else if (curr_rule->state == 2) {
    //     curr_rule->state = 2;
    // } else {
    //     curr_rule->state = 1;
    return 0;
}

void start_func(void* data) {
    thread_data* t_data = (thread_data* ) data;
    size_t id = t_data->id;
    if (id <= 0) {
        // printf("THREAD ID IS: %lu\n", id);
        return;
    }
    rule_t* curr_rule = (rule_t *) queue_pull(rule_queue);
    while(curr_rule != NULL) {
        // if (curr_rule == NULL) {
        //     printf("ITS EMPTY YOOOOOO!\n");
        // }
        vector* goal_rules = graph_neighbors(rule_graph, "");
        for (size_t i = 0; i < vector_size(goal_rules); i++) {
            char* goal_rule = vector_get(goal_rules, i);
            if (strcmp(curr_rule->target, goal_rule) == 0) {
                printf("CURR TARG: %s\n", curr_rule->target);
                // queue_push(rule_queue, curr_rule);
                queue_push(rule_queue, NULL);
            }
        }
        pthread_mutex_lock(&glob_m);
        if ((curr_rule->state) != 2) {
        pthread_mutex_unlock(&glob_m);
            // printf("CURRENT RULE: %s\n", curr_rule->target);
            bool quit_loop = false;
             while (!quit_loop) {
            quit_loop = true;
            vector* neighbors = graph_neighbors(rule_graph, curr_rule->target);
            for (size_t i = 0; i < vector_size(neighbors); i++) {
            char* neighbor = vector_get(neighbors, i);
            rule_t *nbr_rule = (rule_t *) graph_get_vertex_value(rule_graph, neighbor);
            pthread_mutex_lock(&glob_m);
            if ((nbr_rule->state) != 2) {
                printf("WAITING ON: %s\n", curr_rule->target);
                quit_loop = false;
                pthread_cond_wait(&glob_cv, &glob_m);
                // queue_push(rule_queue, NULL);
            }
            pthread_mutex_unlock(&glob_m);
        }
             }
            for (size_t i = 0; i < vector_size(curr_rule->commands); i++) {
                if (system(vector_get(curr_rule->commands, i)) != 0) {
                    vector* dependent = graph_antineighbors(rule_graph, curr_rule->target);
                    for (size_t i = 0; i < vector_size(dependent); i++) {
                        rule_t* dep_rule = (rule_t *) graph_get_vertex_value(rule_graph, (char*) vector_get(dependent, i));
                        printf("DEPENDENTS: %s\n", dep_rule->target);
                        dep_rule->state = 3;
                    }
                    vector_destroy(dependent);
                    break;
                }
            }
            pthread_mutex_lock(&glob_m);
            curr_rule->state = 2;
            pthread_mutex_unlock(&glob_m);
            vector* dependent = graph_antineighbors(rule_graph, curr_rule->target);
            for (size_t i = 0; i < vector_size(dependent); i++) {
                pthread_mutex_lock(&glob_m);
                rule_t* dep_rule = (rule_t *) graph_get_vertex_value(rule_graph, (char*) vector_get(dependent, i));
                if (isSatisfied(dep_rule->target) && !dep_rule->state) {
                    // printf("DEPENDENTS: %s\n", dep_rule->target);
                    queue_push(rule_queue, dep_rule);
                    dep_rule->state = 1;
                    pthread_cond_signal(&glob_cv);
                } 
                // dep_rule->state = 2;
                pthread_mutex_unlock(&glob_m);
            }
            // queue_push(rule_queue, NULL);

        }
        pthread_cond_signal(&glob_cv);
        curr_rule = queue_pull(rule_queue);
        
    }
    queue_push(rule_queue, NULL);
    return;
}

int parmake(char *makefile, size_t num_threads, char **targets) {
    // good luck!
    if (num_threads > 1) {
    pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
    thread_data* t_data = malloc(sizeof(thread_data) * num_threads);
    rule_queue = queue_create(-1);
    rule_graph = parser_parse_makefile(makefile, targets);
    vector* goal_rules = graph_neighbors(rule_graph, "");
    for (size_t i = 0; i < num_threads; i++) {
        // &t_data[i].id = malloc(sizeof(size_t));
        t_data[i].id = i + 1;
        pthread_create(threads+i, NULL, (void *)&start_func, (void *)&t_data[i]);
    }

    for (size_t i = 0; i < vector_size(goal_rules); i++) {
        char* curr_targ = (char*)vector_get(goal_rules, i);
        // printf("%s\n", curr_targ);
        if (isCyclic(curr_targ)) {
            print_cycle_failure(curr_targ);
        } else {
            canRun(curr_targ);
        }
    }
    for (size_t i = 0; i < num_threads; i++) {
        void** retval = NULL;
        // queue_push(rule_queue, NULL);
        pthread_join(threads[i], retval);
        // queue_push(rule_queue, NULL);
    }
    free(threads);
    free(t_data);
    pthread_mutex_destroy(&glob_m);
    pthread_cond_destroy(&glob_cv);
    graph_destroy(rule_graph);
    vector_destroy(goal_rules);
    dictionary_destroy(hasVisited);
    queue_destroy(rule_queue);
    return 0;
    } else {
        rule_graph = parser_parse_makefile(makefile, targets);
        vector* goal_rules = graph_neighbors(rule_graph, "");
        for (size_t i = 0; i < vector_size(goal_rules); i++) {
            char* curr_targ = (char*)vector_get(goal_rules, i);
            if (isCyclic(curr_targ)) {
                // rule_t *curr_rule = (rule_t *) graph_get_vertex_value(rule_graph, curr_targ);
                // curr_rule->state = 1;
                // rule_destroy(curr_rule);
                print_cycle_failure(curr_targ);
            } else {
                onethread(curr_targ);
            }
        }
        graph_destroy(rule_graph);
        vector_destroy(goal_rules);
        return 0;
    }
}