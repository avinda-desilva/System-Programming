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
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <signal.h>
#include "includes/dictionary.h"

#define MAX_CLIENTS 20
#define MAX_EVENTS 20

typedef struct client_info {
    int state;
    char command_info[1024];
    char* client_file;
    size_t file_sz;
} client_info;

static dictionary* client_dict;
static volatile int endSession;
static char* server_dir;
static int ep_num;
void close_server() {
    endSession = 1;
}

void cleanup() {
    remove(server_dir);
}


int main(int argc, char **argv) {
    if (argc != 2) {
        print_server_usage();
        return -1;
    }

    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = close_server;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    char new_dir_temp[] = "XXXXXX";
    server_dir = mkdtemp(new_dir_temp);
    print_temp_directory(server_dir);

    char* port = argv[1];
    client_dict = int_to_shallow_dictionary_create();
    /*QUESTION 1*/
    /*QUESTION 2*/
    /*QUESTION 3*/
    int sock_code = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_code < 0) {
        perror("socket()");
        exit(1);
    }
    struct addrinfo port_addr;
    struct addrinfo* ret_info;
    /*QUESTION 8*/
    int optval = 1;
    int opt_code = setsockopt(sock_code, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(int)); //sizeof(optval)
    if (opt_code != 0) {
        perror("setsockopt()");
        exit(1);
    }
    /*QUESTION 4*/
    /*QUESTION 5*/
    /*QUESTION 6*/
    memset(&port_addr, 0, sizeof(port_addr));
    
    port_addr.ai_family = AF_INET; // AF_INET for IPv4
    port_addr.ai_socktype = SOCK_STREAM; // Only want stream-based connection
    port_addr.ai_flags = AI_PASSIVE;
    int addr_code = getaddrinfo(NULL, port, &port_addr, &ret_info);
    if (addr_code != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addr_code));
        exit(1);
    }

    /*QUESTION 9*/
    int bind_code = bind(sock_code, ret_info->ai_addr, ret_info->ai_addrlen);
    if (bind_code != 0) {
        perror("bind()");
        exit(1);
    }
    freeaddrinfo(ret_info); //Optional but for cleanup of memory leaks.
    /*QUESTION 10*/
    int listen_code = listen(sock_code, MAX_CLIENTS);
    if (listen_code != 0) {
        perror("listen()");
        exit(1);
    }


    // for (int i = 0; i < MAX_CLIENTS; i++) {
    //     clients[i] = -1; //Initialize all clients to empty state.
    // }

    ep_num = epoll_create(1);
    if (ep_num == -1) {
        perror("epoll_create()");
        exit(1);
    }

    struct epoll_event ep_v;
    ep_v.events = EPOLLIN;
    ep_v.data.fd = sock_code;
    epoll_ctl(ep_num, EPOLL_CTL_ADD, sock_code, &ep_v);
    struct epoll_event all_events[MAX_EVENTS];


    while(endSession == 0) {
        if (endSession != 0) {
            break;
        }
        int event_num = epoll_wait(ep_num, all_events, MAX_EVENTS, -1);
        // if (event_num == -1) {
        //     perror("epoll_wait()");
        //     exit(1);
        // } 
        for (int i = 0; i < event_num; i++) {
            if (all_events[i].data.fd == sock_code) {
                int accept_code = accept(sock_code, NULL, NULL);
                if (accept_code < 0) {
                    perror(NULL);
                    exit(1);
                }

                fcntl(sock_code, F_SETFL, fcntl(sock_code, F_GETFL, 0) | O_NONBLOCK);
                ep_v.events = EPOLLIN;
                ep_v.data.fd = accept_code;
                epoll_ctl(ep_num, EPOLL_CTL_ADD, accept_code, &ep_v);
                client_info curr_info;
                curr_info.state = 0;
                dictionary_set(client_dict, &accept_code, &curr_info);
            } else {
                //Run Commands Here
            } 
        }

    }
    cleanup();
    return 0;
}
// Server Setup is referenced from lab charming_chatroom.
