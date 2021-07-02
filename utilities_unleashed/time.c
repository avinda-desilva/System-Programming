/**
 * Utilities Unleashed
 * CS 241 - Spring 2020
 */
#include "format.h"
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_time_usage();
    }
    //int status;
    struct timespec beg, end;
    clock_gettime(CLOCK_MONOTONIC, &beg);
    pid_t child_fork = fork();
    if (child_fork == -1) {
        print_fork_failed();
    } else if (child_fork > 0) {
        int status;
        waitpid(child_fork, &status, 0);
        clock_gettime(CLOCK_MONOTONIC, &end);
    } else if (child_fork == 0) {
        int id = execvp(argv[1], argv + 1);
        if (id) {
            print_exec_failed();
        }
    }
    //printf("%ld", (end.tv_nsec - beg.tv_nsec) / 100000);
    double duration = (double)(end.tv_sec - beg.tv_sec) + (double)(((end.tv_nsec - beg.tv_nsec) / 1000000000.0));
    display_results(argv, duration);
    return 0;
}
