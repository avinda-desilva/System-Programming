/**
 * Deadlock Demolition
 * CS 241 - Spring 2020
 */
#include "graph.h"
#include "libdrm.h"
#include "set.h"
#include <pthread.h>
#include <stdio.h>

// You probably will need some global variables here to keep track of the
// resource allocation graph.
static graph* drm_graph = NULL;
struct drm_t {
    // Declare your struct's variables here. Think about what you will need.
    // Hint: You will need at least a synchronization primitive.
    pthread_mutex_t m; 
};
pthread_mutex_t glob_m = PTHREAD_MUTEX_INITIALIZER;
drm_t *drm_init() {
    /* Your code here */
    drm_t* drm = (drm_t*)malloc(sizeof(drm_t));
    pthread_mutex_init(&drm->m, NULL);
    pthread_mutex_lock(&glob_m);
    if (drm_graph == NULL) {
        drm_graph = shallow_graph_create();
    }
    graph_add_vertex(drm_graph, drm);
    pthread_mutex_unlock(&glob_m);
    return drm;
}

int drm_post(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    pthread_mutex_lock(&glob_m);
    if(!graph_contains_vertex(drm_graph, thread_id)) {
        pthread_mutex_unlock(&glob_m);
        return 0;
    } else if (graph_adjacent(drm_graph, drm, thread_id)) {
        graph_remove_edge(drm_graph, drm, thread_id);
        pthread_mutex_unlock(&drm->m);
        pthread_mutex_unlock(&glob_m);
        return 1;
    } else {
        pthread_mutex_unlock(&glob_m);
        return 0;
    }
}

int isCyclic(void* key) {
    if (drm_graph == NULL || vector_get(graph_vertices(drm_graph), 0) == NULL) {
        return 0;
    }
    vector* drm_nodes = shallow_vector_create();
    vector_push_back(drm_nodes, key);
    dictionary* hasVisited = shallow_to_shallow_dictionary_create();
    while(vector_size(drm_nodes) > 0) {
        void* node = *vector_back(drm_nodes);
        vector_pop_back(drm_nodes);
        if (!dictionary_contains(hasVisited, node)) {
            int visited = 1;
            dictionary_set(hasVisited, node, &visited);
            vector* neighbors = graph_neighbors(drm_graph, node);
            for (size_t i = 0; i < vector_size(neighbors); i++) {
                vector_push_back(drm_nodes, vector_get(neighbors, i));
            }
        } else {
            dictionary_destroy(hasVisited);
            return 1;
        }
    }
    dictionary_destroy(hasVisited);
    return 0;
}

int drm_wait(drm_t *drm, pthread_t *thread_id) {
    /* Your code here */
    pthread_mutex_lock(&glob_m);
    if (!graph_contains_vertex(drm_graph, thread_id)) {
        graph_add_vertex(drm_graph, thread_id);
    }
    if (!graph_adjacent(drm_graph, thread_id, drm)) {
        graph_add_edge(drm_graph, thread_id, drm);
    } else {
        pthread_mutex_unlock(&glob_m);
        return 0;
    }
    if (!isCyclic(thread_id)) {
        pthread_mutex_unlock(&glob_m);
        pthread_mutex_lock(&drm->m);
        pthread_mutex_lock(&glob_m);
        graph_add_edge(drm_graph, drm, thread_id);
        graph_remove_edge(drm_graph, thread_id, drm);
        pthread_mutex_unlock(&glob_m);
        return 1;
    } else {
        graph_remove_edge(drm_graph, thread_id, drm);
        pthread_mutex_unlock(&glob_m);
        return 0;
    }
    return 0;
}

void drm_destroy(drm_t *drm) {
    /* Your code here */
    graph_remove_vertex(drm_graph, drm);
    pthread_mutex_destroy(&drm->m);
    free(drm);
    pthread_mutex_destroy(&glob_m);
    return;
}