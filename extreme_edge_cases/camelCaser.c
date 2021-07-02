/**
 * Extreme Edge Cases
 * CS 241 - Spring 2020
 */
#include "camelCaser.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

char **camel_caser(const char *input_str) {
    // TODO: Implement me!
    if (input_str == NULL) {
        return NULL;
    }
    int count = strlen(input_str);
    int i;
    int num_sentences = 0;
    int index = 0;
    char *outstring = (char*)malloc(2048*sizeof(char));
    int char_index = 0;
    int word_index = 0;
    bool new_line = true;
    for (i = 0; i < count; i++) {
        if (ispunct(input_str[i])) {
            num_sentences++;
        }
    }
    char ** output = (char**)malloc((num_sentences+10)*sizeof(outstring));

    for (i = 0; i < count; i++) {
        if ispunct(input_str[i]) {
            *(output+index) = (char*)malloc(strlen(outstring));
            //outstring[strlen(outstring)] = '\0';
            *(output+index) = strdup(outstring);
            //printf("%s\n", *(output+index));
            memset(outstring, 0, 2048);
            word_index = 0;
            char_index = 0;
            index++;
            new_line = true; 
            *(output+index) = NULL;
            //printf("This char is a punct %c\n", input_str[i]);
        } else if (isalpha(input_str[i]) && output[index] == NULL) {
            //printf("Char index %d", char_index);
            outstring[char_index] = tolower(input_str[i]);
            char_index++;
            new_line = false; 
        } else if (isalpha(input_str[i]) && output[index] != NULL) {
            new_line = false; 
            if (word_index == 0) {
                outstring[char_index] = tolower(input_str[i]);
                char_index++; 
            } else {
                outstring[char_index] = toupper(input_str[i]);
                char_index++; 
            }
        } 
        else if (isspace(input_str[i])) {
            if (&input_str[i+1] != NULL && isalpha(input_str[i+1])) {
                if (new_line) {
                    outstring[char_index] = tolower(input_str[i+1]);
                    i++;
                    char_index++;
                    word_index++;
                    new_line = false;
                } else {
                    outstring[char_index] = toupper(input_str[i+1]);
                    i++;
                    char_index++;
                    word_index++;                    
                } 
      
            }
        } 
        else if (isspace(input_str[i]) || isblank(input_str[i]))  {
            continue;
        } else if (isascii(input_str[i]) && !(isspace(input_str[i]) || isblank(input_str[i]))) {
            outstring[char_index] = input_str[i];
            char_index++;
        }
        // } else if (isalpha(input_str) && char_index != 0) {
        //     outstring[char_index] = input_str[i];
        //     char_index++;
        // }
    }
            *(output+(index+1)) = NULL;
        
    for (i = 0; i < index; i++) {
     
        printf("%s\n", *(output+i));
        
    }
    free(outstring);
    return output;
}

void destroy(char **result) {
    // TODO: Implement me!
    int i = 0;
    while(*(result+i) != NULL) {
        free(*(result+i));
        i++;
    }
    //free(result);
    result = NULL;
    return;
}
