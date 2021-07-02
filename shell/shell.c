/**
 * Shell
 * CS 241 - Spring 2020
 */
#include "format.h"
#include "shell.h"
#include "vector.h"
#include "string.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "sstring.h"
#include <sys/wait.h>
#include <signal.h>
typedef struct process {
    char *command;
    pid_t pid;
} process;
static int logic_flag = 0;
static vector* log_vector;
static char buff[1024];
static FILE* history_log;
static FILE* file_log;
static char* history_log_name;
static char* curr_dir;
static char cwd[PATH_MAX];
static size_t log_index = 0;

int run_logic_op(char* command);
int contains_logic_op(char* command);

void file_to_vector() {
    char test = 0;
    char line[2048];
    int str_index = 0;
    history_log = fopen(history_log_name, "r");
    vector_clear(log_vector);
    while (test != EOF) {
        test = getc(history_log);
        if(test == '\n' || test == EOF) {
            line[str_index] = '\0';
            char* pass = sstring_slice(cstr_to_sstring(line), 0, str_index);
            vector_push_back(log_vector, pass);
            str_index = 0;
            memset(line, 0, 2048);
        } else {
            line[str_index] = test;
            str_index++;
        }
    }
    fclose(history_log);
}

void save_to_file() {
    history_log = fopen(history_log_name, "a+");
    if (vector_empty(log_vector)) {
        return;
    }
    for(size_t i = 0; i < vector_size(log_vector); i++) {
        fprintf(history_log, "%s\n", vector_get(log_vector, i));
        log_index++;
    }
    vector_clear(log_vector);
    fclose(history_log);
}


int is_build_in(char* command) {
    if (strncmp(command, "cd", 2) == 0) {
        return 1;
    } else if (strcmp(command, "!history") == 0) {
        return 2;
    } else if (command[0] == '#') {
        return 3;
    } else if (command[0] == '!') {
        return 4;
    } else {
        return 0;
    }
}

int run_cd(char* command) {
    if (is_build_in(command) == 1) {
        int new_dir = chdir(command + 3);
        if (new_dir == -1) {
            print_no_directory(command + 3);
            vector_push_back(log_vector, command);
            return -1;
        } else if (logic_flag == 1) {
            return 1;
        } else {
            vector_push_back(log_vector, command);
            return 1;
        }
    } else {
        return -1;
    }
}

int run_extern_comm(char* command) {
    int background_process = 0;
    if (is_build_in(command) != 0) {
        return 0;
    }
    if (command[strlen(command) - 1] == '&') {
        background_process = 1;
        command = strndup(command, strlen(command) - 1);
    }
    if (strcmp(command, "\0") == 0) {
        vector_push_back(log_vector, command);
        print_invalid_command(command);
        return 1;
    }
    pid_t child_fork = fork();
    if (child_fork == -1) {
        print_fork_failed();
        exit(1);
    } else if (child_fork > 0 && background_process == 1) {
        pid_t curr_id = getpid();
        print_command_executed(curr_id);
        vector_push_back(log_vector, command);
    } else if (child_fork > 0) {
        int status;
        waitpid(child_fork, &status, 0);
        if (logic_flag != 0) {
            vector_push_back(log_vector, command);
            return 0;
        } else {
            return 1;
        }
    } else if (child_fork == 0) {
        fflush(stdout);
        vector* args = sstring_split(cstr_to_sstring(command), ' ');
        size_t size = vector_size(args);
        char** copy_comm = malloc((size+1)*sizeof(char*));
        for(size_t i = 0; i < size; i++) {
            copy_comm[i] = vector_get(args, i);
        }
        copy_comm[size] = NULL;
            pid_t curr_id = getpid();
		    print_command_executed(curr_id);
            if(execvp(copy_comm[0], copy_comm)) {
                vector_clear(log_vector);
                save_to_file();
                print_exec_failed(command);
                exit(1);
            } else {
                return 0;
            }
        }
    return 0;
}

//Return -1 if you want to run external command
//Return 0 if there is an error  
//Return 1 if it is a build in command
int run_build_in(char* command) {
    save_to_file();
    if (is_build_in(command) == 1) {
        int ret_val = run_cd(command);
        if (ret_val == 1) {
            //save_to_file();
            return 1;
        } else {
            return 0;
        }
    } else if (is_build_in(command) == 2) {
        history_log = fopen(history_log_name, "a+");
        
        for (size_t i = 0; i < vector_size(log_vector); i++) {
            fprintf(history_log, "%s\n", vector_get(log_vector, i));
            log_index++;
        }
        fclose(history_log);
        file_to_vector();
        for (size_t i = 0; i < log_index; i++) {
            //
            print_history_line(i, vector_get(log_vector, i));
            
        }
        vector_clear(log_vector);
        return 1;
    } else if (is_build_in(command) == 3) {
        if (strlen(command) < 2) {
            print_invalid_index();
            return 0;
        }
        size_t index = atoi(&command[0]+1);
        if (index >= log_index) {
            print_invalid_index();
            return 0;
        } else {
            file_to_vector();
            char* new_comm = strdup(vector_get(log_vector, index));
            if (contains_logic_op(new_comm) != 0) {
                print_command(new_comm);
                run_logic_op(vector_get(log_vector, index));
                vector_clear(log_vector);
                vector_push_back(log_vector, new_comm);
                save_to_file();
                return 1;
            } else if (is_build_in(new_comm) == 1) {
                print_command(new_comm);
                vector_clear(log_vector);
                run_cd(new_comm);
                save_to_file();
                return 1;
            } else {
                vector_clear(log_vector);
                print_command(new_comm);
                run_extern_comm(new_comm);
                save_to_file();
                return 1;
            }
        }
    } else if (is_build_in(command) == 4) {
        file_to_vector();
        char* new_comm = NULL;
        if (strlen(command) == 1) {
            new_comm = strdup(vector_get(log_vector, vector_size(log_vector) - 1));
            if (contains_logic_op(new_comm) != 0) {
                print_command(new_comm);
                run_logic_op(new_comm);
                vector_clear(log_vector);
                vector_push_back(log_vector, new_comm);
                save_to_file();
            } else if (is_build_in(new_comm) == 1) {
                print_command(new_comm);
                vector_clear(log_vector);
                run_cd(new_comm);
                save_to_file();
                return 1;
            } else {
                print_command(new_comm);
                vector_clear(log_vector);
                run_extern_comm(new_comm);
                save_to_file();
                return 1;
            }
        } else {
            for(size_t i = vector_size(log_vector) - 1; i > 0; i--) {
                if ((strlen(command) - 1) <= strlen(vector_get(log_vector, i))) {
                    if(strncmp(vector_get(log_vector, i), command+1, strlen(command) - 1) == 0) {
                        new_comm = strdup(vector_get(log_vector, i));
                        break;
                    } 
                    new_comm = NULL;
                }
            }
            if (new_comm == NULL) {
                print_no_history_match();
                vector_clear(log_vector);
                return 0;
            } else if (is_build_in(new_comm) == 1) {
                vector_clear(log_vector);
                run_cd(new_comm);
                save_to_file();
                return 1;
            } else {
                vector_clear(log_vector);
                run_extern_comm(new_comm);
                save_to_file();
                return 1;
            }
        }
        return 1;
    } else {
        return -1;
    }
}

int contains_logic_op(char * command) {
    char* arg_1 = strstr(command, "&&");
    char* arg_2 = strstr(command, "||");
    char* arg_3 = strstr(command, ";");
    int count = 0;
    char* sub_arg;
    if (arg_1 != NULL) {
        //printf("%s\n", arg_1);
        if ((sub_arg = strstr(arg_1+2, "&&")) != NULL) {
            return 0;
        }
        count++;
    }
    if (arg_2 != NULL) {
        if ((sub_arg = strstr(arg_2+2, "||")) != NULL) {
            return 0;
        }
        count++;
    }
    if (arg_3 != NULL) {
        //printf("%s", arg_3);
        if ((sub_arg = strstr(arg_3+1, ";")) != NULL) {
            return 0;
        }
        count++;
    }
    if (count > 1) {
        return 0;
    }
    if (arg_1 != NULL && command != NULL) {
        if(arg_1[2] != ' ' || command[strlen(command) - strlen(arg_1) - 1] != ' ') {
            return 0;
        }
        return 1;
    } 
    //printf("%s\n", arg_2);
    if (arg_2 != NULL && command != NULL) {
        if(arg_2[2] != ' ' || command[strlen(command) - strlen(arg_2) - 1] != ' ') {
            return 0;
        }
        return 2;
    } 
    //printf("%s\t%s\n", arg_3, command);
    if (arg_3 != NULL && command != NULL) {
        if (arg_3[1] != ' ' || command[strlen(command) - strlen(arg_3)] != ';') {
            return 0;
        }
        return 3;
    }
    return 0;
}

int run_logic_op(char* command) {
    char *arg_1;
    int ret_val;
    if (contains_logic_op(command) == 1) {
        arg_1 = strsep(&command, "&&");
        //printf("%s\n", arg_1);
        arg_1[strlen(arg_1) - 1] = '\0';
        if (is_build_in(arg_1) != 0) {
            ret_val = run_build_in(arg_1);
            if (ret_val == 0) {
                return 0;
            } else {
                if (is_build_in(command+2) != 0) {
                    run_build_in(command+2);
                    return 1;
                } else {
                    run_extern_comm(command+2);
                    return 1;
                }
            }
        } else {
            ret_val = run_extern_comm(arg_1);
            if (ret_val == 1) {
                return 0;
            } else {
                if (is_build_in(command+2) != 0) {
                    run_build_in(command+2);
                    return 1;
                } else {
                    run_extern_comm(command+2);
                    return 1;
                }
            }
        }
    } else if (contains_logic_op(command) == 2) {
        arg_1 = strsep(&command, "||");

        //printf("%s\n", arg_1);
        arg_1[strlen(arg_1) - 1] = '\0';
        if (is_build_in(arg_1) != 0) {
            ret_val = run_build_in(arg_1);
            if (ret_val != 0) {
                return 0;
            } else {
                if (is_build_in(command+2) != 0) {
                    run_build_in(command+2);
                    return 1;
                } else {
                    run_extern_comm(command+2);
                    return 1;
                }
            }
        } else {
            ret_val = run_extern_comm(arg_1);
            if (ret_val != 1) {
                //printf("LOOOOOL");
                return 0;
            } else {
                if (is_build_in(command+2) != 0) {
                    run_build_in(command+2);
                    return 1;
                } else {
                    run_extern_comm(command+2);
                    return 1;
                }
            }
        }
    } else if (contains_logic_op(command) == 3) {
        arg_1 = strsep(&command, ";");

        //printf("%s\n%s\n", arg_1, command);
        //arg_1[strlen(arg_1) - 1] = '\0';
        if (is_build_in(arg_1) != 0) {
            ret_val = run_build_in(arg_1);
            if (is_build_in(command+1) != 0) {
                run_build_in(command+1);
                return 1;
            } else {
                run_extern_comm(command+1);
                return 1;
            }
            
        } else {
            ret_val = run_extern_comm(arg_1);
            if (is_build_in(command+1) != 0) {
                run_build_in(command+1);
                return 1;
            } else {
                run_extern_comm(command+1);
                return 1;
            }
        }
    } else {
        return 0;
    }
}

void handle_sigint(int given_signal) {
    signal(SIGINT, handle_sigint);
}

void handle_cleanup(int signal) {
  //int status;
  while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {

  }
}

void handle_sigterm(int signal) {
    exit(0);
}

void exit_program() {
    //save_to_file();
    signal(SIGCHLD, handle_cleanup);
    exit(0);
}

int shell(int argc, char **argv) {
    // TODO: This is the entry point for your shell.
    signal(SIGCHLD, handle_cleanup);
    int opt;
    history_log = fopen("unknown.txt", "w");
    history_log_name = get_full_path("unknown.txt");
    fclose(history_log);
    log_vector = vector_create(string_copy_constructor, string_destructor, string_default_constructor);
    char* user_line = NULL;
    size_t len_of_str = 0;
    signal(SIGINT, handle_sigint);
    if(argc < 2 || (argc % 2 == 0 && argc > 3)) {
        print_usage();
    } else {
        while ((opt = getopt(argc, argv, "h:f:")) != -1) {
            if (opt == 'h') {
                history_log = fopen(argv[2], "r");
                if (history_log != NULL) {
                    char* line = fgets(buff, 1024, history_log);
                    while (line != NULL) {
                        log_index++;
                        line = fgets(buff, 1024, history_log);
                    }
                    history_log_name = get_full_path(argv[2]);
                    fclose(history_log);
                } else {
                    log_index = 0;
                    history_log = fopen(argv[2], "w");
                    if (history_log == NULL) {
                        print_script_file_error();
                    } else {
                    fclose(history_log);
                    history_log_name = get_full_path(argv[2]);
                    }
                }
            } else if (opt == 'f') {
                file_log = fopen(argv[2], "r");
                if (file_log == NULL) {
                    print_script_file_error();
                } else {
                    char test = 0;
                    char line[2048];
                    int index = 0;
                    if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        curr_dir = cwd;
                    }
                    while (test != EOF) {
                        test = getc(file_log);
                        if(test == '\n' || test == EOF) {
                            print_prompt(curr_dir, getpid());
                            line[index] = '\n';
                            char* pass = sstring_slice(cstr_to_sstring(line), 0, index);
                            printf("%s\n", pass);
                            if (contains_logic_op(pass) != 0) {
                                run_logic_op(pass);
                            } else if (run_build_in(pass) == -1) {
                                run_extern_comm(pass);
                            } else {
                                vector_push_back(log_vector, pass);
                            }
                            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                                curr_dir = cwd;
                            }
                            index = 0;
                            memset(line, 0, 2048);
                            fflush(stdout);
                        } else {
                            line[index] = test;
                            index++;
                        }
                    }
                    fclose(file_log);
                    exit(0);
                }
            }
        }
        signal(SIGTERM, handle_sigterm);
        int num_char = 0;
        int ret_val = 1;
        int ret_val_2 = 0;
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                curr_dir = cwd;
        }
        print_prompt(curr_dir, getpid());
        while (getline(&user_line, &len_of_str, stdin) != -1) {
            num_char = strlen(user_line);
            user_line[num_char - 1] = '\0';
            if (strcmp(user_line, "exit") == 0) {
                exit_program();
            }            
            if (contains_logic_op(user_line) != 0) {
                logic_flag = 1;
                vector_push_back(log_vector, user_line);
                save_to_file();
                run_logic_op(user_line);
                //continue;
            } else if ((ret_val_2 = run_build_in(user_line)) == -1) {
                ret_val = run_extern_comm(user_line);
            } 
            if ((ret_val != 0 && ret_val_2 == -1) || ret_val_2 == 0) {
                vector_push_back(log_vector, user_line);
            }
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                curr_dir = cwd;
            }
            print_prompt(curr_dir, getpid());
            logic_flag = 0;
        }

    }
    return 0;
}

