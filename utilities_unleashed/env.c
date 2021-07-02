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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
extern char** environ;
int main(int argc, char *argv[]) {
    if (argc == 1) {
        while (*environ) {
            printf("%s\n", *environ);
            environ++;
        }
        return 0;
    } else if (argc <= 2) {
        print_env_usage();
        exit(EXIT_FAILURE);
    } else if (argc > 2) {
        pid_t child_fork = fork();
        if (child_fork == -1) {
            print_fork_failed();
        } else if (child_fork > 0) {
            int status;
            waitpid(child_fork, &status, 0);
        } else if (child_fork == 0) {
            //char * arg_1 = argv[1];
            int i;
            int index = -1;
            for (int i = 0; i < argc; i++) {
                if (!strcmp(argv[i], "--")) {
                    index = i;
                    //printf("%s", *argv + child_fork);
                } 
            }
            if (index == -1) {
                print_env_usage();
                exit(1);
            }
            i = 1;
            while(i < index) {
                char *arg = argv[i];
                char* key = strsep(&arg, "=");
                if (key != NULL && arg != NULL) {
                    for(size_t j = 0; j < strlen(key); j++) {
                        if (!isalnum(key[j]) && key[j] != '_' && key[j] != '/') {
                            print_env_usage();
                            exit(1);
                        }
                    }
                    for(size_t j = 0; j < strlen(arg); j++) {
                        if (!isalnum(arg[j]) && arg[j] != '%' && arg[j] != '/' && arg[j] != '_') {
                            print_env_usage();
                            exit(1);
                        }
                    }
                    if(&arg[0] != NULL && arg[0] == '%') {
                        char* arg_val = getenv(&arg[1]);
                        setenv(key, arg_val, 1);
                    } else if(key != NULL && arg != NULL) {
                        setenv(key, arg, 1);
                    } else {
                        print_env_usage();
                    }
                }
                i++;
            }
            if(execvp(argv[index + 1], argv + index + 1)) {
               print_exec_failed();
            }
        }
    }
    return 0;
}