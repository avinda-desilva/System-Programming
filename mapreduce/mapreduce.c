/**
 * mapreduce
 * CS 241 - Spring 2020
 */
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h> 
#include <errno.h> 
#include <sys/wait.h>

int main(int argc, char **argv) {
    char* input_file = NULL;
    char* output_file = NULL;
    char* mapper = NULL;
    char* reducer = NULL;
    int map_count = -1;
    if (argc == 6 && atoi(argv[5]) >= 1) {
        input_file = argv[1];
        output_file = argv[2];
        mapper = argv[3];
        reducer = argv[4];
        map_count = atoi(argv[5]);
    } else {
        print_usage();
        return 2;
    }
    if (input_file == NULL || output_file == NULL || mapper == NULL || reducer == NULL || map_count == -1) {
        print_usage();
        return 3;
    }
    // Create an input pipe for each mapper.
    int* map_pipe[map_count];
    for (int i = 0; i < map_count; i++) {
        map_pipe[i] = malloc(sizeof(int) * 2);
        pipe2(map_pipe[i], O_CLOEXEC);
        descriptors_add(map_pipe[i][0]);
        descriptors_add(map_pipe[i][1]);
    }
    // Create one input pipe for the reducer.
    int reduce_pipe[2];
    pipe2(reduce_pipe, O_CLOEXEC);
    descriptors_add(reduce_pipe[0]);
    descriptors_add(reduce_pipe[1]);
    // Open the output file.
    int output =  open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
    // Start a splitter process for each mapper.
    pid_t split_child[map_count];
    for (int i = 0; i < map_count; i++) {
        int* curr_pipe = map_pipe[i];
        split_child[i] = fork();
        if (split_child[i] == 0) {
            char i_to_str[5];
            sprintf(i_to_str, "%d", i);
            dup2(curr_pipe[1], 1);
            descriptors_closeall();
            execlp("./splitter", "./splitter", input_file, argv[5], i_to_str, NULL);
            exit(1);
        }
    }
    pid_t map_child[map_count];
    for (int i = 0; i < map_count; i++) {
        int* curr_pipe = map_pipe[i];
       map_child[i] = fork();
        if (map_child[i] == 0) {
            dup2(curr_pipe[0], 0);
            dup2(reduce_pipe[1], 1);
            descriptors_closeall();
            execl(mapper, mapper, NULL);
            exit(1);
        }
    }
    pid_t reduce_child = fork();
    if (reduce_child == 0) {
        dup2(reduce_pipe[0], 0);
        dup2(output, 1);
        execl(reducer, reducer, NULL);
        exit(1);
    }
    descriptors_closeall();
    close(output);
    // Wait for the reducer to finish.
    // Print nonzero subprocess exit codes.
    for (int i = 0; i < map_count; i++) {
        int split_status;
        waitpid(split_child[i], &split_status, 0);
        if (split_status) {
            print_nonzero_exit_status("./splitter", split_status);
        }
    }
    for (int i = 0; i < map_count; i++) {
        int map_status;
        waitpid(map_child[i], &map_status, 0);
        if (map_status) {
            print_nonzero_exit_status(mapper, map_status);
        }
    }
    int red_status;
    waitpid(reduce_child, &red_status, 0);
    if (red_status) {
        print_nonzero_exit_status(reducer, red_status);
    }

    // Count the number of lines in the output file.
    descriptors_closeall();
    descriptors_destroy();
    print_num_lines(output_file);
    for (int i = 0; i < map_count; i++) {
        free(map_pipe[i]);
    }
    return 0;
}
