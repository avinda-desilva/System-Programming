/**
 * Mad Mad Access Patterns
 * CS 241 - Spring 2020
 * avindad2
 * janaks2
 * ajariw2
 */
#include "tree.h"
#include "utils.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/
uint32_t search_tree(char* addr, int offset, char* search_word) {
  BinaryTreeNode* root = ((BinaryTreeNode *) ((addr + offset)));
  char* node_word = addr + offset + sizeof(BinaryTreeNode);
  int comparison = strcmp(search_word, node_word);

  if(comparison == 0) {
    return offset;
  } else if (comparison < 0) {
    if(root->left_child == 0) {
      return 0;
    } else {
      return search_tree(addr, root->left_child, search_word);
    }
  } else if(comparison > 0) {
    if(root->right_child == 0) {
      return 0;
    } else {
      return search_tree(addr, root->right_child, search_word);
    }
  }

  return 0;
}

int main(int argc, char **argv) {
  if (argc < 3) {
    printArgumentUsage();
    exit(1);
  }
  char* file_name = argv[1];
  FILE* search_file = fopen(file_name, "r");
  if (search_file == NULL) {
    openFail(file_name);
    exit(1);
  }
  fseek(search_file, 0L, SEEK_END);
  int file_sz = ftell(search_file);
  fseek(search_file, 0, SEEK_SET); // Move to the start of the file 
  fclose(search_file);
  int mmap_fd = open(file_name, O_RDONLY);
  if (mmap_fd == -1) {
    openFail(file_name);
    exit(1);
  }
  char* file_to_addr = mmap(NULL, file_sz, PROT_READ, MAP_PRIVATE, mmap_fd, 0);
  if (file_to_addr == (void *) -1) {
    mmapFail(file_name);
    exit(2);
  }
  if (strncmp(file_to_addr, BINTREE_HEADER_STRING, BINTREE_ROOT_NODE_OFFSET) != 0) {
    formatFail(file_name);
    exit(2);
  }
  for (int i = 2; i < argc; i++) {
    char* search_word = argv[i];
    uint32_t node_offset =  search_tree(file_to_addr, BINTREE_ROOT_NODE_OFFSET, search_word);
    if (node_offset) {
      BinaryTreeNode* node = ((BinaryTreeNode *) (file_to_addr + node_offset));
      printFound(search_word, node->count, node->price);
    } else {
      printNotFound(search_word);
    }
  }
  close(mmap_fd);
  return 0;
}

