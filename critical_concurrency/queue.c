/**
 * Critical Concurrency
 * CS 241 - Spring 2020
 */
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This queue is implemented with a linked list of queue_nodes.
 */
typedef struct queue_node {
    void *data;
    struct queue_node *next;
} queue_node;

struct queue {
    /* queue_node pointers to the head and tail of the queue */
    queue_node *head, *tail;

    /* The number of elements in the queue */
    ssize_t size;

    /**
     * The maximum number of elements the queue can hold.
     * max_size is non-positive if the queue does not have a max size.
     */
    ssize_t max_size;

    /* Mutex and Condition Variable for thread-safety */
    pthread_cond_t cv;
    pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size) {
    /* Your code here */
    struct queue* new_queue = (struct queue *)malloc(sizeof(struct queue));
    new_queue->max_size = max_size;
    new_queue->size = 0;
    new_queue->head = NULL;
    new_queue->tail = NULL;
    pthread_mutex_init(&new_queue->m, NULL);
    pthread_cond_init(&new_queue->cv, NULL);
    return new_queue;
}

void queue_destroy(queue *this) {
    /* Your code here */
    struct queue_node* curr = this->head;
    struct queue_node* temp = NULL;
    while (curr != NULL) {
        temp = curr;
        curr = curr->next;
        free(temp);
        //curr = next;
    }
    pthread_mutex_destroy(&this->m);
    pthread_cond_destroy(&this->cv);
    free(this);
}

void queue_push(queue *this, void *data) {
    /* Your code here */
    pthread_mutex_lock(&this->m);
    while(this->size == this->max_size) {
        pthread_cond_wait(&this->cv, &this->m);
    }
    struct queue_node* new_node = malloc(sizeof(struct queue_node));
    if (this->head != NULL) {
        this->tail->next = new_node;
        new_node->next = NULL;
        this->tail = new_node;
    } else {
        this->head = new_node;
        this->tail = new_node;
        new_node->next = NULL;
    }
    new_node->data = data; 
    this->size++;
    pthread_cond_broadcast(&this->cv);
    pthread_mutex_unlock(&this->m);
}

void *queue_pull(queue *this) {
    /* Your code here */
    if (this->size < 0) {
        this->size = 0;
        return NULL;
    }
    pthread_mutex_lock(&this->m);
    while(this->size == 0) {
        pthread_cond_wait(&this->cv, &this->m);
    }
    struct queue_node* curr = this->head;
    void* data;
    if (curr != this->tail) {
        this->head = curr->next;
        data = curr->data;
        free(curr);
    } else {
        this->head = NULL;
        this->tail = NULL;
        data = curr->data;
        free(curr);
    }
    this->size--;
    pthread_cond_broadcast(&this->cv);
    pthread_mutex_unlock(&this->m);
    if (data == NULL) {
        return NULL;
    }
    return data;
}
