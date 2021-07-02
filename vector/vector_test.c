/**
 * Vector
 * CS 241 - Spring 2020
 */
#include "vector.h"
#include <stdio.h>
int main(int argc, char *argv[]) {
    // Write your test cases here
    vector *myvector = vector_create(shallow_copy_constructor, shallow_destructor, NULL);
    //char* elem = "test";
    //void* str = (void*)elem;
    for(int i = 0; i < 100; i++) {
        vector_insert(myvector, 0, "test");
        printf("Size and cap: %zu, %zu\n", vector_size(myvector), vector_capacity(myvector));
    }
    for(int i = 0; i < 95; i++) {
        vector_erase(myvector, 0);
        printf("Size and cap: %zu, %zu\n", vector_size(myvector), vector_capacity(myvector));
    }
    //vector_pop_back(myvector);
    vector_set(myvector, 0, "yoyoyoyoyoyoyoyo");
    printf("Size and cap: %zu, %zu\n", vector_size(myvector), vector_capacity(myvector));
    //vector_clear(myvector);
    printf("Size and cap: %s, %s\n", *vector_front(myvector), *vector_back(myvector));
    vector_destroy(myvector);
    return 0;
}
