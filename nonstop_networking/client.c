/**
 * Nonstop Networking
 * CS 241 - Spring 2020
 */
#include "common.h"
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
char **parse_args(int argc, char **argv);
verb check_args(char **args);
void close_server_connection();
int connect_to_server(const char *host, const char *port);
int main(int argc, char **argv) {
    // Good luck!
    char** args = parse_args(argc, argv);
    verb request_name = check_args(args);
    int serverSocket = connect_to_server(args[0], args[1]);
    if (request_name == PUT) {
        char write_str[strlen(args[3]) + 6];
        sprintf(write_str, "PUT %s\n", args[3]);
        ssize_t write_num = write_all_to_socket(serverSocket, write_str, strlen(write_str));
        if (write_num < 0) {
            print_connection_closed();
            free(args);
            exit(1);
        }
        FILE* fd = fopen(args[4], "r");
        if (fd == NULL) {
            char* err_msg = strdup(err_bad_request);
            print_error_message(err_msg);
            free(err_msg);
            free(args);
            exit(1);
        }
        fseek(fd, 0L, SEEK_END);
        size_t sz = ftell(fd);
        write_all_to_socket(serverSocket, (char*)&sz, sizeof(size_t));
        fseek(fd, 0L, SEEK_SET);
        size_t write_ct = 0;
        while (write_ct < sz) {
            ssize_t buff_size = sz - write_ct;
            if (buff_size > 1024) {
                buff_size = 1024;
            }
            char buffer[buff_size+1];
            fread(buffer, 1, buff_size, fd);
            ssize_t write_bytes = write_all_to_socket(serverSocket, buffer, buff_size);
            if (write_bytes < buff_size) {
                // print_too_little_data();
                print_connection_closed();
                free(args);
                fclose(fd);
                exit(1);
            }
            write_ct += write_bytes;
            }
        fclose(fd);
        int shut_code = shutdown(serverSocket, SHUT_WR);
        if (shut_code != 0) {
            perror("shutdown()");
            free(args);
            exit(1);
        }
        char ret[3];
        read_all_from_socket(serverSocket, ret, 3);
        ret[2] = '\0';
        if (strncmp(ret, "OK", 2) != 0) {
            print_invalid_response();
            free(args);
            exit(1);
        } else {
            print_success();
        }
        free(args);
        close(serverSocket);        
        return 0;
    } else if (request_name == DELETE) {
        char write_str[strlen(args[3]) + 8];
        sprintf(write_str, "DELETE %s\n", args[3]);
        ssize_t write_num = write_all_to_socket(serverSocket, write_str, strlen(write_str));
        if (write_num < 0) {
            print_connection_closed();
            free(args);      
            exit(1);
        }
        char ret[3];
        read_all_from_socket(serverSocket, ret, 3);
        ret[2] = '\0';
        if (strncmp(ret, "OK", 2) != 0) {
            char* err_msg = strdup(err_no_such_file);
            print_error_message(err_msg);
            free(err_msg);
            free(args);      
            exit(1);
        } else {
            print_success();
        }
        close_server_connection(serverSocket);
        free(args);      
        return 0;
    } else if (request_name == GET) {
        char write_str[strlen(args[3]) + 6];
        sprintf(write_str, "GET %s\n", args[3]);
        write_str[strlen(args[3]) + 5] = '\0';
        ssize_t write_num = write_all_to_socket(serverSocket, write_str, strlen(write_str));
        if (write_num < 0) {
            print_connection_closed();
            free(args);
            exit(1);
        }
        FILE* fd = fopen(args[4], "w");
        if (fd == NULL) {
            char* err_msg = strdup(err_bad_request);
            print_error_message(err_msg);
            free(err_msg);
            free(args);
            exit(1);
        }
        char ret[3];
        read_all_from_socket(serverSocket, ret, 3);
        ret[2] = '\0';
        // printf("%s", ret);
        if (strncmp(ret, "OK", 2) != 0) {
            char* err_msg = strdup(err_no_such_file);
            print_error_message(err_msg);
            free(err_msg);
            free(args);
            fclose(fd);
            // close_server_connection(serverSocket);
            exit(1);
        } 
        // read_all_from_socket(serverSocket, ret, 1);
        ssize_t readsz; //= get_message_size(serverSocket);
        read_all_from_socket(serverSocket, (char *)&readsz, 8);
        // printf("%lu\n", readsz);
        ssize_t read_count = 0;

        while (read_count < readsz) {
            ssize_t buff_size = 1024;
            if (readsz - read_count < buff_size) {
                buff_size = readsz - read_count;
            }
            char buffer[buff_size];
            ssize_t read_bytes = read_all_from_socket(serverSocket, buffer, buff_size);
            if (read_bytes < 0) {
                print_too_little_data();
                free(args);
                fclose(fd);
                exit(1);
            }
            if (read_bytes == 0) {
                break;
            }
            fwrite(buffer, 1, read_bytes, fd);
            read_count += read_bytes;
            // printf("%lu\n", read_count);

        }
        if (read_count < readsz) {
            print_too_little_data();
            free(args);
            fclose(fd);
            exit(1);
        }
         if (read_count > readsz) {
            // printf("%zu\n", read_count);
            print_received_too_much_data();
            free(args);
            fclose(fd);
            exit(1);
        }
        char buffer[1024];
        ssize_t read_bytes = read_all_from_socket(serverSocket, buffer, 1024);
        if (read_bytes > 0) {
            print_received_too_much_data();
            free(args);
            fclose(fd);
            exit(1);
        }
        fclose(fd);
        close_server_connection(serverSocket);
        free(args);
        return 0;
    } else if (request_name == LIST) {
        char write_str[5];
        sprintf(write_str, "LIST\n");
        ssize_t write_num = write_all_to_socket(serverSocket, write_str, strlen(write_str));
        if (write_num < 0) {
            print_connection_closed();
            free(args);
            exit(1);
        }
        char buffer[1024];
        ssize_t read_bytes = read_all_from_socket(serverSocket, buffer, 3);
        buffer[2] = '\n';
        read_all_from_socket(serverSocket, buffer, 0);
        get_message_size(serverSocket);
        if (strncmp(buffer, "OK", 2) != 0) {
            print_invalid_response();
            free(args);
            exit(1);
        } 
        while(read_bytes > 0) {
            fwrite(buffer, 1, read_bytes, stdout);
            read_bytes = read_all_from_socket(serverSocket, buffer, 1024);
        }
        close_server_connection(serverSocket);
        free(args);
        return 0;
    }

    int shut_code = shutdown(serverSocket, SHUT_WR);
    if (shut_code != 0) {
        perror("shutdown()");
        exit(1);
    }
    char ret[3];
    read_all_from_socket(serverSocket, ret, 3);
    ret[1] = '\0';
    if (strcmp(ret, "OK\n") != 0) {
        print_error_message(ret);
        free(args);
        exit(1);
    } 
    if (strcmp(ret, "OK\n") == 0) {
        print_success();
    }
    free(args);
    close(serverSocket);
    return 0;
}

/**
 * Shuts down connection with 'serverSocket'.
 * Called by close_program upon SIGINT.
 */
void close_server_connection(int serverSocket) {
    // Your code here
    int shut_code = shutdown(serverSocket, SHUT_RDWR);
    if (shut_code != 0) {
        perror("shutdown()");
        exit(1);
    }
    close(serverSocket);
    // Referenced from server.c cleanup
}

/**
 * Sets up a connection to a chatroom server and returns
 * the file descriptor associated with the connection.
 *
 * host - Server to connect to.
 * port - Port to connect to server on.
 *
 * Returns integer of valid file descriptor, or exit(1) on failure.
 */
int connect_to_server(const char *host, const char *port) {
    /*QUESTION 1*/
    /*QUESTION 2*/
    /*QUESTION 3*/
    int sock_code = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_code < 0) {
        perror("socket()");
        exit(1);
    }
    struct addrinfo host_addr;
    struct addrinfo* ret_info;
    
    /*QUESTION 4*/
    /*QUESTION 5*/
    memset(&host_addr, 0, sizeof(host_addr));
    
    host_addr.ai_family = AF_INET; // AF_INET for IPv4
    host_addr.ai_socktype = SOCK_STREAM; // Only want stream-based connection

    /*QUESTION 6*/
    int addr_code = getaddrinfo(host, port, &host_addr, &ret_info);
    if (addr_code != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addr_code));
        exit(1);
    }

    /*QUESTION 7*/
    int connect_code = connect(sock_code, ret_info->ai_addr, ret_info->ai_addrlen);
    if (connect_code != 0) {
        perror("connect()");
        exit(1);
    }

    freeaddrinfo(ret_info); //Optional but for cleanup of memory leaks.
    return sock_code;
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}




