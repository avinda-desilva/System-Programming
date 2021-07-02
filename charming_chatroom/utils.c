/**
 * Charming Chatroom
 * CS 241 - Spring 2020
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes =
        read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    // Your code here
    uint32_t new_size = htonl(size);
    ssize_t write_bytes = write_all_to_socket(socket, (char *)&new_size, MESSAGE_SIZE_DIGITS);
    if (write_bytes <= 0) {
        return write_bytes;
    }
    return (ssize_t)htonl(new_size);
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
    size_t byte_ct = 0;
    while(count > 0) {
        int ret_code = read(socket, buffer + byte_ct, count);
        if (ret_code == 0) {
            return byte_ct;
        } else if (ret_code == -1 && errno == EINTR) {
            continue;
        } else if (ret_code > 0) {
            byte_ct += ret_code;
            count -= ret_code;
        } else {
            return byte_ct;
        }
    }
    return byte_ct;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    // Your Code Here
    size_t byte_ct = 0;
    while(count > 0) {
        int ret_code = write(socket, buffer + byte_ct, count);
        if (ret_code == 0) {
            return byte_ct;
        } else if (ret_code == -1 && errno == EINTR) {
            continue;
        } else if (ret_code > 0) {
            byte_ct += ret_code;
            count -= ret_code;
        } else {
            return byte_ct;
        }
    }
    return byte_ct;
}
