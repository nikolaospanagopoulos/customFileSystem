#include "file_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512         // Size of each block in bytes
#define NUM_BLOCKS 1024        // Total number of blocks on the "disk"
#define MAX_FILES 100          // Maximum number of files or directories
#define MAX_BLOCKS_PER_FILE 10 // Maximum number of blocks a file can use
#define MAX_NAME_LEN 20        // Maximum name length for files or directories
#define MAX_FILES_IN_DIR                                                       \
  100 // Maximum number of files or directories in a directory

char **disk;
struct index_node *index_node_table;

void init_file_system() {
  // initialize disk
  disk = (char **)calloc(NUM_BLOCKS, sizeof(char *));
  for (int i = 0; i < NUM_BLOCKS; i++) {
    disk[i] = (char *)calloc(BLOCK_SIZE, sizeof(char));
  }
  index_node_table =
      (struct index_node *)calloc(MAX_FILES, sizeof(struct index_node));
  for (int i = 0; i < MAX_FILES; i++) {
    index_node_table[i].blocks = calloc(MAX_BLOCKS_PER_FILE, sizeof(int));
    index_node_table[i].name = calloc(MAX_NAME_LEN, sizeof(char));
    index_node_table[i].children_index_nodes =
        calloc(MAX_FILES_IN_DIR, sizeof(char));
    index_node_table[i].parent_index_node = -1;
    index_node_table[i].num_of_children = 0;
    for (int j = 0; j < MAX_BLOCKS_PER_FILE; j++) {
      index_node_table[i].blocks[j] = -1;
    }
  }
  // root directory
  index_node_table[0].type = DIRECTORY_TYPE;
  strcpy(index_node_table[0].name, "/");
  index_node_table[0].size = 0;
  // root has no parent
  index_node_table[0].parent_index_node = -1;
}
void free_file_system() {
  for (int i = 0; i < MAX_FILES; i++) {
    free(index_node_table[i].blocks);
    free(index_node_table[i].name);
    free(index_node_table[i].children_index_nodes);
  }
  free(index_node_table);
  for (int i = 0; i < NUM_BLOCKS; i++) {
    free(disk[i]);
  }
  free(disk);
}
int create_directory(const char *dirname, int parent_inode_idx) {
  int idx = -1;
  for (int i = 0; i < MAX_FILES; i++) {
    if (index_node_table[i].name[0] == '\0') {
      idx = i;
      break;
    }
  }
  if (idx == -1) {
    printf("no free nodes available\n");
    return -1;
  }
  index_node_table[idx].type = DIRECTORY_TYPE;
  strncpy(index_node_table[idx].name, dirname, MAX_NAME_LEN - 1);
  index_node_table[idx].name[MAX_NAME_LEN - 1] = '\0';
  index_node_table[idx].parent_index_node = parent_inode_idx;

  // make sure parent now contains this directory
  index_node_table[parent_inode_idx].children_index_nodes
      [index_node_table[parent_inode_idx].num_of_children] = idx;
  index_node_table[parent_inode_idx].num_of_children++;

  return idx;
}
int find_free_block() {
  for (int i = 0; i < NUM_BLOCKS; i++) {
    if (disk[i][0] == '\0') {
      return i;
    }
  }
  return -1;
}
int createFile(const char *filename, char *contents, int parent_inode_idx) {
  int idx = -1;
  for (int i = 0; i < MAX_FILES; i++) {
    if (index_node_table[i].name[0] == '\0') {
      idx = i;
    }
  }

  if (idx == -1) {
    printf("No free nodes available\n");
    return -1;
  }
  int content_size = strlen(contents);
  int num_blocks = (content_size + BLOCK_SIZE - 1) / BLOCK_SIZE;
  if (num_blocks > MAX_BLOCKS_PER_FILE) {
    printf("file is too large");
    return -1;
  }
  for (int i = 0; i < num_blocks; i++) {
    int block_idx = find_free_block();
    if (block_idx == -1) {
      printf("no free disk blocks available \n");
      return -1;
    }
    index_node_table[idx].blocks[i] = block_idx;
    strncpy(disk[block_idx], contents + i * BLOCK_SIZE, BLOCK_SIZE);
  }
  index_node_table[idx].type = FILE_TYPE;
  index_node_table[idx].size = content_size;
  strncpy(index_node_table[idx].name, filename, MAX_NAME_LEN - 1);
  index_node_table[idx].name[MAX_NAME_LEN - 1] = '\0';
  // goes to parent directory
  index_node_table[parent_inode_idx].children_index_nodes
      [index_node_table[parent_inode_idx].num_of_children] = idx;
  index_node_table[parent_inode_idx].num_of_children++;
  return 0;
}
int change_directory(const char *dirname, int current_inode_idx) {
  if (strcmp(dirname, "..") == 0) {
    // move to parent
    if (current_inode_idx == 0) {
      // cannot go further
      return current_inode_idx;
    } else {
      return index_node_table[current_inode_idx].parent_index_node;
    }
  }
  // search for directory in the current directory's children
  for (int i = 0; i < index_node_table[current_inode_idx].num_of_children;
       i++) {
    int child_idx = index_node_table[current_inode_idx].children_index_nodes[i];
    if (strcmp(index_node_table[child_idx].name, dirname) == 0 &&
        index_node_table[child_idx].type == DIRECTORY_TYPE) {
      return child_idx;
    }
  }
  printf("Directory with name: {%s} not found\n", dirname);
  return -1;
}
void list_directory(int inode_idx) {
  printf("{%s} Contents: ", index_node_table[inode_idx].name);
  for (int i = 0; i < index_node_table[inode_idx].num_of_children; i++) {
    int child_index = index_node_table[inode_idx].children_index_nodes[i];
    if (index_node_table[child_index].type == DIRECTORY_TYPE) {
      printf("[DIR] %s\n", index_node_table[child_index].name);
    } else {
      printf("[FILE] %s (%d bytes)\n", index_node_table[child_index].name,
             index_node_table[child_index].size);
    }
  }
}

void read_file(const char *filename, int current_inode_idx) {
  for (int i = 0; i < index_node_table[current_inode_idx].num_of_children;
       i++) {
    int file_idx = index_node_table[current_inode_idx].children_index_nodes[i];
    if (strcmp(index_node_table[file_idx].name, filename) == 0 &&
        index_node_table[file_idx].type == FILE_TYPE) {
      printf("reading file %s: \n", filename);
      int bytes_left = index_node_table[file_idx].size;
      for (int j = 0;
           j < MAX_BLOCKS_PER_FILE &&
           index_node_table[file_idx].blocks[j] != -1 && bytes_left > 0;
           j++) {
        int block_idx = index_node_table[file_idx].blocks[j];
        int bytes_to_read = bytes_left > BLOCK_SIZE ? BLOCK_SIZE : bytes_left;
        printf("%.*s", bytes_to_read, disk[block_idx]);
        bytes_left -= bytes_to_read;
      }
      printf("\n");
      return;
    }
  }
  printf("file {%s} not found \n", filename);
}
