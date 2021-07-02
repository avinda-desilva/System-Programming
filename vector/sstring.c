/**
 * Vector
 * CS 241 - Spring 2020
 */
#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>
struct sstring {
    // Anything you want
    size_t length;
    char* text;
    char* appended;
    vector *split_string;
    char* subbed_word;
};

sstring *cstr_to_sstring(const char *input) {
    // your code goes here
    sstring *new_string = malloc(strlen(input)+13 + sizeof(vector*));
    new_string->length = strlen(input);
    new_string->text = strdup(input);
    new_string->appended = NULL;
    new_string->subbed_word = NULL;
    new_string->split_string = vector_create(string_copy_constructor, string_destructor, string_default_constructor);
    //printf("The sstring is: %s", new_string->text);
    //free(input);
    return new_string;
}

char *sstring_to_cstr(sstring *input) {
    // your code goes here
    char* cstr = malloc(input->length);
    cstr = strdup(input->text);
    //sstring_destroy(input);
    return cstr;
}

int sstring_append(sstring *this, sstring *addition) {
    // your code goes here
    //this = realloc(this, this->length + addition->length);
    // for (int i = 0; i < this->length; i++) {
    //     this->text++;
    // }
    //char* head = (this->text);
    // while(*this->text) {
    //     this->text++;
    // }
    // printf("%c", *(this->text));
    // while(*(this->text)) {
    //     this->text--;
    // }
    this->appended = malloc(this->length + addition->length + 1); 
    strcpy(this->appended, this->text);
    strcat(this->appended, addition->text);
    return 0;
}

vector *sstring_split(sstring *this, char delimiter) {
    // your code goes here
    char* delim = strtok(this->text, &delimiter);
    //this->split_string = vector_create(string_copy_constructor, string_destructor, string_default_constructor);
    while(delim != NULL) {
        vector_push_back(this->split_string, delim);
        delim = strtok(NULL, &delimiter);
    }
    return this->split_string;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    assert(this->text);                
    // your code goes here
    char substitute[2048] = {0};
    this->subbed_word = malloc(this->length * 3);
    int added_length = 0;
    for(size_t i = offset; i < this->length; i++) {
        substitute[i - offset] = this->text[i]; 
    }
    for(size_t i = 0; i < offset; i++) {
        this->subbed_word[i] = this->text[i]; 
    }
    char* delim = strtok(substitute, target);
    while(delim != NULL) {
        strcat(this->subbed_word, delim);
        strcat(this->subbed_word, substitution);
        delim = strtok(NULL, target);
        // added_length += strlen(substitution);
        // added_length -= strlen(target);
    }
    this->text = malloc(this->length + added_length);
    if(this->text != NULL) {
        this->length = strlen(this->text);
    }
    return this->length;
}

char *sstring_slice(sstring *this, int start, int end) {
    // your code goes here
    return NULL;
}

void sstring_destroy(sstring *this) {
    // your code goes here
    if(this->appended != NULL) {
        free(this->appended);
        this->appended = NULL;
    }
    vector_destroy(this->split_string);
    free(this);
    this = NULL;
}

void print_sstring(sstring *this) {
    printf("%s\n", this->text);
}
