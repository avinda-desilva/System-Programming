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
/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/
uint32_t search_tree(FILE* file, int offset, char* search_word) {
  fseek(file, offset+16, SEEK_SET);
	int node_size = 16;
	while(1){
		int c = fgetc(file);
		node_size++;
		if(c == '\0'){
			break;
		}
	}
	BinaryTreeNode node[node_size];
  int file_seek = ftell(file);
  if (file_seek > offset) {
	  fseek(file, offset, SEEK_SET);
  }
	fread(node, node_size, 1, file);
  int comparison = strcmp(search_word, node->word);

  if(comparison == 0) {
    return offset;
  } else if (comparison < 0) {
    if(node->left_child == 0) {
      return 0;
    } else {
      return search_tree(file, node->left_child, search_word);
    }
  } else if(comparison > 0) {
    if(node->right_child == 0) {
      return 0;
    } else {
      return search_tree(file, node->right_child, search_word);
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
  // fseek(search_file, 0, SEEK_SET); // Move to the start of the file 
  char valid_root[BINTREE_ROOT_NODE_OFFSET];
  fread(valid_root, BINTREE_ROOT_NODE_OFFSET, 1, search_file);
  if (strcmp(valid_root, BINTREE_HEADER_STRING) != 0) {
    formatFail(file_name);
    exit(2);
  }
  for (int i = 2; i < argc; i++) {
    char* search_word = argv[i];
    uint32_t node_offset = search_tree(search_file, BINTREE_ROOT_NODE_OFFSET, search_word);
    if (node_offset) {
      fseek(search_file, node_offset+16, SEEK_SET);
	    int node_size = 16;
	    while(1){
        int c = fgetc(search_file);
        node_size++;
        if(c == '\0'){
          break;
        }
	    }
      BinaryTreeNode node[node_size];
      uint32_t file_seek = ftell(search_file);
      if (file_seek > node_offset) {
        fseek(search_file, node_offset, SEEK_SET);
      }
      fread(node, node_size, 1, search_file);
      printFound(search_word, node->count, node->price);
    } else {
      printNotFound(search_word);
    }
  }
  fclose(search_file);
  return 0;
}
