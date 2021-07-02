/**
 * Perilous Pointers
 * CS 241 - Spring 2020
 */
#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // your code here
    //First Step
    int first_num = 81;
    first_step(first_num);

    //Second Step
    int val = 132;
    int *second_num = &val;
    second_step(second_num);

    //Double Step
    val = 8942;
    int *second_val = &val;
    int **double_num = &second_val;
    double_step(double_num);

    //Strange Step
    char strange_val[6];
    int i;
    for (i = 0; i < 6; i++) {
        strange_val[i] = 's';
    }
    *(int *)(strange_val + 5) = 15;
    strange_step(strange_val);

    //Empty Step
    void* empty_arr[4] = {0};
    empty_arr[3] = 0;
    empty_step(empty_arr);

    //Two Step
    char *two_arr = "uuuu";
    void* two_val = two_arr;
    two_step(two_val, two_arr);

    //Three Step
    char *s_1 = "ccc";
    char *s_2 = s_1 + 2;
    char *s_3 = s_2 + 2;
    three_step(s_1, s_2, s_3);

    //Step Step Step
    char sts_1[4];
    char sts_2[4];
    char sts_3[4];
    for (i = 0; i < 4; i++) {
        sts_1[i] = 0;
        sts_2[i] = 0;
        sts_3[i] = 0;
    }
    sts_2[2] = sts_1[1] + 8;
    sts_3[3] = sts_2[2] + 8;
    step_step_step(sts_1, sts_2, sts_3);

    //It May Be Odd
    char odd_char = 'A';
    char* char_ptr = &odd_char;
    int char_to_int = 65;
    it_may_be_odd(char_ptr, char_to_int);

    //Tok Step
    char tok_str[] = "CS241,CS241,CS241";
    tok_step(tok_str);

    //The End
    void* orange;
    void* blue;
    char end_val = 1;
    blue = &end_val;
    orange = blue;
    the_end(orange, blue);
    return 0;
}
