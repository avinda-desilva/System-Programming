/**
 * Mini Memcheck
 * CS 241 - Spring 2020
 */
#include "mini_memcheck.h"
#include <stdio.h>
#include <string.h>

size_t total_memory_requested;
size_t total_memory_freed;
size_t invalid_addresses;
meta_data* head;
void *mini_malloc(size_t request_size, const char *filename,
                  void *instruction) {
    // your code here
    if (request_size <= 0) {
        return NULL;
    }
    void* memory_block = malloc(sizeof(meta_data) + request_size);
    if (memory_block == NULL) {
        invalid_addresses++;
        return NULL;
    }
    meta_data* new_mem_data = (meta_data*)memory_block;
    new_mem_data->request_size = request_size;
    new_mem_data->filename = filename;
    new_mem_data->instruction = instruction;
    new_mem_data->next = NULL;
    new_mem_data->next = head;
    head = new_mem_data;
    
    total_memory_requested += request_size;
    return (void*)memory_block+sizeof(meta_data);

}

void *mini_calloc(size_t num_elements, size_t element_size,
                  const char *filename, void *instruction) {
    // your code here
    if (num_elements <= 0 || element_size <= 0) {
        return NULL;
    }
    size_t req_size = num_elements * element_size;
    void* memory_block = malloc(sizeof(meta_data) + req_size);
    if (memory_block == NULL) {
        invalid_addresses++;
        return NULL;
    }
    meta_data* new_mem_data = (meta_data *) memory_block;
    new_mem_data->request_size = req_size;
    new_mem_data->filename = (filename);
    new_mem_data->instruction = instruction;
    new_mem_data->next = NULL;
    new_mem_data->next = head;
    head = new_mem_data;

    total_memory_requested += req_size;
    return (void*)memory_block+sizeof(meta_data);
}

void *mini_realloc(void *payload, size_t request_size, const char *filename,
                   void *instruction) {
    // your code here
    meta_data* old_meta_data = (meta_data*)payload;
    total_memory_requested -= old_meta_data->request_size;
    printf("%zu", old_meta_data->request_size);
    void* memory_block = malloc(sizeof(meta_data) + request_size);
    if (memory_block == NULL) {
        invalid_addresses++;
        return NULL;
    }
    old_meta_data = (meta_data*)memory_block;
    old_meta_data->request_size = request_size;
    old_meta_data->filename = (filename);
    old_meta_data->instruction = instruction;
    old_meta_data->next = NULL;
    old_meta_data->next = head;
    head = old_meta_data;
    total_memory_requested += request_size;
    return (void*)memory_block+sizeof(meta_data);
}

void mini_free(void *payload) {
    // your code here
    //free(payload);
    if (payload == NULL) {
        return;
    }
    if(head == NULL){
		//invalid_addresses++;
		return;
	}
	meta_data *free_ptr = (meta_data *)payload;
	meta_data *curr = head;
	while(curr != NULL){
        void* memory = (void*)curr + sizeof(meta_data);
		if(memory == payload){
			curr -> next = free_ptr -> next;
			total_memory_freed += free_ptr -> request_size;
			free(free_ptr);
			return;
		}
		curr = curr -> next;
	}
	invalid_addresses++;
}
