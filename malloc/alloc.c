/**
 * Malloc
 * CS 241 - Spring 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
typedef struct _meta_data {
    // Number of bytes of heap memory the user requested from malloc
    size_t request_size;

    void* ptr;

    bool isfree;

    struct _meta_data *next;
} meta_data;

static meta_data* head = NULL;
static int num_free;
/**
 * Allocate space for array in memory
 *
 * Allocates a block of memory for an array of num elements, each of them size
 * bytes long, and initializes all its bits to zero. The effective result is
 * the allocation of an zero-initialized memory block of (num * size) bytes.
 *
 * @param num
 *    Number of elements to be allocated.
 * @param size
 *    Size of elements.
 *
 * @return
 *    A pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory, a
 *    NULL pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/
 */
void *calloc(size_t num, size_t size) {
    // implement calloc!
    size_t total_space = num*size;
    void* ret = malloc(total_space);
    memset(ret, 0, total_space);
    return ret;
}

/**
 * Allocate memory block
 *
 * Allocates a block of size bytes of memory, returning a pointer to the
 * beginning of the block.  The content of the newly allocated block of
 * memory is not initialized, remaining with indeterminate values.
 *
 * @param size
 *    Size of the memory block, in bytes.
 *
 * @return
 *    On success, a pointer to the memory block allocated by the function.
 *
 *    The type of this pointer is always void*, which can be cast to the
 *    desired type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a null pointer is returned.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/
 */
meta_data* get_free_block(size_t size) {
    meta_data* curr = head;
    while(curr != NULL) {
        if (curr->isfree && curr->request_size >= size) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void *malloc(size_t size) {
    meta_data *curr_data = head;
	meta_data *prev_data = NULL;
	meta_data *next_data = NULL;
    meta_data *ret_block = sbrk(0);
	if (size <= 0) {
        return NULL;
    }
    if (num_free > 0){
		while (curr_data != NULL) {
			if (curr_data->isfree && curr_data->request_size >= size){
                if (next_data && next_data->isfree && curr_data->isfree) {
                    if (next_data->request_size + curr_data->request_size + sizeof(meta_data) >= size) {
                        curr_data->request_size += next_data->request_size + sizeof(meta_data);
                        if(head != NULL) { 
                            next_data->next = curr_data;
                        } else{
                            head = curr_data;
                        }
                        num_free -= 2;
                        break;
                    } 
			    } else if (next_data && prev_data && next_data->isfree && prev_data->isfree && curr_data->isfree){
                    if (next_data->request_size + prev_data->request_size + curr_data->request_size + 2 * sizeof(meta_data) >= size) {
                        curr_data->request_size += next_data->request_size + prev_data->request_size + 2 * sizeof(meta_data);
                        if(head == NULL){
                            head = curr_data;
                        }
                        else{
                            prev_data->next = curr_data;
                        }
                        num_free -= 3;
                        break;
                    }
                }
				num_free--;
				break;
			}
			next_data = curr_data;
			curr_data = curr_data->next;
		}
		if (curr_data) {
			curr_data->isfree = false;
			return curr_data->ptr;
		}
	}
    sbrk(sizeof(meta_data));
	ret_block->ptr = sbrk(0);
    ret_block->isfree = false;
    ret_block->request_size = size;
	ret_block->next = head;
	head = ret_block;
	if (sbrk(size) == (void*)-1) {
        return NULL;
    }
	return ret_block->ptr;
}

/**
 * Deallocate space in memory
 *
 * A block of memory previously allocated using a call to malloc(),
 * calloc() or realloc() is deallocated, making it available again for
 * further allocations.
 *
 * Notice that this function leaves the value of ptr unchanged, hence
 * it still points to the same (now invalid) location, and not to the
 * null pointer.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(),
 *    calloc() or realloc() to be deallocated.  If a null pointer is
 *    passed as argument, no action occurs.
 */
void free(void *ptr) {
    // implement free!
    if (ptr != NULL) {
        num_free++;
        meta_data* free_ptr = (meta_data*)ptr - 1;
        free_ptr->isfree = true;
    }
}

/**
 * Reallocate memory block
 *
 * The size of the memory block pointed to by the ptr parameter is changed
 * to the size bytes, expanding or reducing the amount of memory available
 * in the block.
 *
 * The function may move the memory block to a new location, in which case
 * the new location is returned. The content of the memory block is preserved
 * up to the lesser of the new and old sizes, even if the block is moved. If
 * the new size is larger, the value of the newly allocated portion is
 * indeterminate.
 *
 * In case that ptr is NULL, the function behaves exactly as malloc, assigning
 * a new block of size bytes and returning a pointer to the beginning of it.
 *
 * In case that the size is 0, the memory previously allocated in ptr is
 * deallocated as if a call to free was made, and a NULL pointer is returned.
 *
 * @param ptr
 *    Pointer to a memory block previously allocated with malloc(), calloc()
 *    or realloc() to be reallocated.
 *
 *    If this is NULL, a new block is allocated and a pointer to it is
 *    returned by the function.
 *
 * @param size
 *    New size for the memory block, in bytes.
 *
 *    If it is 0 and ptr points to an existing block of memory, the memory
 *    block pointed by ptr is deallocated and a NULL pointer is returned.
 *
 * @return
 *    A pointer to the reallocated memory block, which may be either the
 *    same as the ptr argument or a new location.
 *
 *    The type of this pointer is void*, which can be cast to the desired
 *    type of data pointer in order to be dereferenceable.
 *
 *    If the function failed to allocate the requested block of memory,
 *    a NULL pointer is returned, and the memory block pointed to by
 *    argument ptr is left unchanged.
 *
 * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/
 */
void *realloc(void *ptr, size_t size) {
    // implement realloc!
    if (size == 0) {
        if (ptr != NULL) {
            free(ptr);
        }
        return NULL;
    }
    if (ptr == NULL) {
        return malloc(size);
    }
    meta_data* meta_p = (meta_data*) ptr - 1;
    if (meta_p->request_size >= size) {
        return ptr;
    } else {
        void* ret_mem = malloc(size);
        memcpy(ret_mem, ptr, meta_p->request_size);
        free(ptr);
        return ret_mem;
    }
}
