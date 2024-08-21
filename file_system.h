#pragma once

typedef enum { FILE_TYPE, DIRECTORY_TYPE } index_node_type;

struct index_node {
  index_node_type type;
  int size;    // size of file in bytes (0 for directories)
  int *blocks; // array of block indices for file data
  char *name;
  int parent_index_node;
  int *children_index_nodes; // array of indices of children;
  int num_of_children;
};

void init_file_system();
void free_file_system();
int find_free_block();
int create_directory(const char *dirname, int parent_inode_idx);
int createFile(const char *filename, char *contents, int parent_inode_idx);
int change_directory(const char *dirname, int current_inode_idx);
void list_directory(int inode_idx);
void read_file(const char *filename, int current_inode_idx);
