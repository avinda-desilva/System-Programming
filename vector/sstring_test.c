/**
 * Vector
 * CS 241 - Spring 2020
 */
#include "sstring.h"

int main(int argc, char *argv[]) {
    // TODO create some tests
    char* input = "Hello how are youuuuuuu!";
    char* add = "Im doing goooooood.";
    sstring *mystring = cstr_to_sstring(input);
    sstring *my2string = cstr_to_sstring(add);
    sstring_split(mystring, 'a');
    printf("input: %s\n", input);
    sstring_append(mystring, my2string);
    print_sstring(mystring);
    input = sstring_to_cstr(mystring);
    add = sstring_to_cstr(my2string);
    printf("input: %s\n", input);
    sstring_substitute(mystring, 10, " " , "lol");
    print_sstring(mystring);
    free(input);
    free(add);
    sstring_destroy(my2string);
    sstring_destroy(mystring);
    return 0;
}
