/**
 * Charming Chatroom
 * CS 241 - Spring 2020
 */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define MAX_CLIENTS 8

void *process_client(void *p);

static volatile int serverSocket;
static volatile int endSession;

static volatile int clientsCount;
static volatile int clients[MAX_CLIENTS];

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * Signal handler for SIGINT.
 * Used to set flag to end server.
 */
void close_server() {
    endSession = 1;
    // add any additional flags here you want.
}

/**
 * Cleanup function called in main after `run_server` exits.
 * Server ending clean up (such as shutting down clients) should be handled
 * here.
 */
void cleanup() {
    if (shutdown(serverSocket, SHUT_RDWR) != 0) {
        perror("shutdown():");
    }
    close(serverSocket);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1) {
            if (shutdown(clients[i], SHUT_RDWR) != 0) {
                perror("shutdown(): ");
            }
            close(clients[i]);
        }
    }
}

// Helper function for run_server
void get_free_client(size_t* access_no, int accept_fd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == -1) {
            *access_no = i;
            clients[*access_no] = accept_fd;
            break;
        }
    }
}

/**
 * Sets up a server connection.
 * Does not accept more than MAX_CLIENTS connections.  If more than MAX_CLIENTS
 * clients attempts to connects, simply shuts down
 * the new client and continues accepting.
 * Per client, a thread should be created and 'process_client' should handle
 * that client.
 * Makes use of 'endSession', 'clientsCount', 'client', and 'mutex'.
 *
 * port - port server will run on.
 *
 * If any networking call fails, the appropriate error is printed and the
 * function calls exit(1):
 *    - fprtinf to stderr for getaddrinfo
 *    - perror() for any other call
 */
void run_server(char *port) {
    pthread_t client_td[MAX_CLIENTS];
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
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = -1; //Initialize all clients to empty state.
    }
    while(endSession == 0) {
        int accept_code = accept(sock_code, NULL, NULL);
        if (accept_code < 0) {
            perror(NULL);
            exit(1);
        }
        pthread_mutex_lock(&mutex);
        clientsCount++;
        if (clientsCount <= MAX_CLIENTS) {
            size_t* access_no = malloc(sizeof(ssize_t));
            *access_no = -1;
            get_free_client(access_no, accept_code);
            int create_code = pthread_create(client_td + *access_no, NULL, process_client, (void *)*access_no);
            if (create_code != 0) {
                perror("pthread_create()");
                pthread_mutex_unlock(&mutex);
                exit(1);
            }
            free(access_no);
        } else {
            int shut_code = shutdown(accept_code, SHUT_RDWR);
            if (shut_code != 0) {
                perror("shutdown()");
                pthread_mutex_unlock(&mutex);
                exit(1);
            }
            clientsCount--;
            pthread_mutex_unlock(&mutex);
            continue;
        }
        pthread_mutex_unlock(&mutex);
    }
}

/**
 * Broadcasts the message to all connected clients.
 *
 * message  - the message to send to all clients.
 * size     - length in bytes of message to send.
 */
void write_to_clients(const char *message, size_t size) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1) {
            ssize_t retval = write_message_size(size, clients[i]);
            if (retval > 0) {
                retval = write_all_to_socket(clients[i], message, size);
            }
            if (retval == -1) {
                perror("write(): ");
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

/**
 * Handles the reading to and writing from clients.
 *
 * p  - (void*)intptr_t index where clients[(intptr_t)p] is the file descriptor
 * for this client.
 *
 * Return value not used.
 */
void *process_client(void *p) {
    pthread_detach(pthread_self());
    intptr_t clientId = (intptr_t)p;
    ssize_t retval = 1;
    char *buffer = NULL;

    while (retval > 0 && endSession == 0) {
        retval = get_message_size(clients[clientId]);
        if (retval > 0) {
            buffer = calloc(1, retval);
            retval = read_all_from_socket(clients[clientId], buffer, retval);
        }
        if (retval > 0)
            write_to_clients(buffer, retval);

        free(buffer);
        buffer = NULL;
    }

    printf("User %d left\n", (int)clientId);
    close(clients[clientId]);

    pthread_mutex_lock(&mutex);
    clients[clientId] = -1;
    clientsCount--;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "%s <port>\n", argv[0]);
        return -1;
    }

    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = close_server;
    if (sigaction(SIGINT, &act, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    run_server(argv[1]);
    cleanup();
    pthread_exit(NULL);
}
