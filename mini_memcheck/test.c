/**
 * Mini Memcheck
 * CS 241 - Spring 2020
 */
#include <stdio.h>
#include <stdlib.h>
#include "mini_memcheck.h"
int main() {
	setvbuf(stdout, NULL, _IONBF, 0);
    // Your tests here using malloc and free
    char* str = malloc (10);
	str = realloc(str, 20);
	str = "HEllo";
	printf("%s", str);
    free(str);
	char* str2 = calloc(sizeof(int), 5);
  return 0;
}
