/**
 * Deepfried dd
 * CS 241 - Spring 2020
 * avindad2
 * janaks2
 * ajariw2
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "format.h"
#include <time.h>
#include <string.h>
#include <signal.h>
static volatile int flag = 0;
double getTime() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9 * t.tv_nsec;
}

void sig_handle(int signal) {
    (void)signal;
    flag = 1;
}

int main(int argc, char **argv) {
    int c;
    char* input = NULL;
    char* output = NULL;
    size_t block_size = 0;
    size_t count = 0;
    size_t read_off = 0;
    size_t write_off = 0;
    size_t full_blocks_in = 0;
    size_t partial_blocks_in = 0;
    size_t full_blocks_out = 0;
    size_t partial_blocks_out = 0;
    size_t total_bytes_copied = 0;
    while ((c = getopt (argc, argv, "iobcpk:")) != -1) {
        switch(c) {
            case 'i':
                // printf("%s\n", argv[optind]);
                input = argv[optind];
                break;
            case 'o':
                // printf("%s\n", argv[optind]);
                output = argv[optind];
                break;
            case 'b':
                // printf("%s\n", argv[optind]);
                block_size = atoi(argv[optind]);
                break;
            case 'c':
                // printf("%s\n", argv[optind]);
                count = atoi(argv[optind]);
                break;
            case 'p':
                // printf("%s\n", argv[optind]);
                read_off = atoi(argv[optind]);
                break;
            case 'k':
                // printf("%s\n", argv[optind]);
                write_off = atoi(optarg);
                // printf("%zu\n", write_off);
                break;
            case '?':
                exit(1);
                break;
            default:
                break;
        }
    }
    if (block_size == 0) {
        block_size = 512;
    }
    // printf("%zu\n", block_size);
    FILE* i_file;
    FILE* o_file;
    // char buffer[1024];
    double startTime = getTime();
    double elapsedTime = 0;

    if (input != NULL) {
        i_file = fopen(input, "r");
        if (!i_file) {
            print_invalid_input(input);
            exit(1);
        }
    } else {
        i_file = stdin;
    }
    if (output != NULL) {
        o_file = fopen(output, "w+");
        if (!o_file) {
            print_invalid_output(output);
            exit(1);
        }
    } else {
        o_file = stdout;
    }
    startTime = getTime();
    signal(SIGUSR1, sig_handle);
    if (count > 0) {
        size_t curr_count = 0;
        if (read_off > 0 && i_file != stdin) {
            fseek(i_file, read_off * block_size, 0);
        }
        if (write_off > 0 && o_file != stdout) {
            fseek(o_file, write_off * block_size, 0);
        }
        while (curr_count < count) {
            char* buffer = malloc(block_size);
            size_t read_ct = fread(buffer, 1, block_size, i_file);
            fwrite(buffer, 1, read_ct, o_file);
            if (feof(i_file)) {
                fclose(i_file);
                if (read_ct > 0 && read_ct < block_size) {
                    // printf("%zu\n", read_ct);
                    partial_blocks_in += 1;
                    partial_blocks_out += 1;
                    total_bytes_copied += read_ct;
                }
                break;
            } else {
                full_blocks_in += 1;
                full_blocks_out += 1;
                total_bytes_copied += block_size;          
            }
            curr_count++;
            free(buffer);
            if (flag) {
                double curr_time = getTime() - startTime; 
                print_status_report(full_blocks_in, partial_blocks_in, full_blocks_out, partial_blocks_out, total_bytes_copied, curr_time);
                flag = 0;
            }
        }
        elapsedTime = getTime() - startTime;
    } else {
        if (read_off > 0 && i_file != stdin) {
            fseek(i_file, read_off * block_size, 0);
        }
        if (write_off > 0 && o_file != stdout) {
            fseek(o_file, write_off * block_size, 0);
        }
        while(1) {

            char* buffer = malloc(block_size); // = malloc(block_size);
            size_t read_ct = fread(buffer, 1, block_size, i_file);
            fwrite(buffer, 1, read_ct, o_file);
            if (feof(i_file)) {
                fclose(i_file);
                if (read_ct > 0 && read_ct < block_size) {
                partial_blocks_in += 1;
                partial_blocks_out += 1;
                total_bytes_copied += read_ct;
                // printf("%zu\n",  read_ct);
                }
                // printf("YOOOOOOOOOO %lu\n", strlen(buffer));
                break;
            } else {
                full_blocks_in += 1;
                full_blocks_out += 1;
                total_bytes_copied += block_size;
                // printf("%d\n",  read_ct);
                // printf("%lu\n", strlen(buffer));
            }
            if (flag) {
                double curr_time = getTime() - startTime; 
                print_status_report(full_blocks_in, partial_blocks_in, full_blocks_out, partial_blocks_out, total_bytes_copied, curr_time);
                flag = 0;
            }
            // curr_count++;
            // free(buffer);
        }
        elapsedTime = getTime() - startTime;
    }
    


    print_status_report(full_blocks_in, partial_blocks_in, full_blocks_out, partial_blocks_out, total_bytes_copied, elapsedTime);
    fclose(o_file);


    return 0;
}