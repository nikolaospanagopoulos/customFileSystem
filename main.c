#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512         // Size of each block in bytes
#define NUM_BLOCKS 1024        // Total number of blocks on the "disk"
#define MAX_FILES 100          // Maximum number of files
#define MAX_BLOCKS_PER_FILE 10 // Maximum number of blocks a file can use

struct inode {
  int size;
  int *blocks;
  char *name;
};

char **disk;

struct inode *inode_table;

void init_file_system() {
  disk = (char **)calloc(NUM_BLOCKS, sizeof(char *));
  for (int i = 0; i < NUM_BLOCKS; i++) {
    disk[i] = calloc(BLOCK_SIZE, sizeof(char));
  }
  inode_table = (struct inode *)calloc(MAX_FILES, sizeof(struct inode));
  for (int i = 0; i < MAX_FILES; i++) {
    inode_table[i].blocks = calloc(MAX_BLOCKS_PER_FILE, sizeof(int));
    inode_table[i].name = calloc(20, sizeof(char));
    for (int j = 0; j < MAX_BLOCKS_PER_FILE; j++) {
      inode_table[i].blocks[j] = -1;
    }
  }
}

void free_file_system() {
  for (int i = 0; i < NUM_BLOCKS; i++) {
    free(disk[i]);
  }
  free(disk);

  for (int i = 0; i < MAX_FILES; i++) {
    free(inode_table[i].blocks);
    free(inode_table[i].name);
  }
  free(inode_table);
}

int findFreeBlock() {
  for (int i = 0; i < NUM_BLOCKS; i++) {
    if (disk[i][0] == '\0') {
      return i;
    }
  }
  return -1;
}

int createFile(const char *filename, char *contents) {
  // find a free inode
  int inode_idx = -1;
  for (int i = 0; i < MAX_FILES; i++) {
    if (inode_table[i].name[0] == '\0') {
      inode_idx = i;
      break;
    }
  }
  if (inode_idx == -1) {
    printf("Couldn't find a free inode \n");
    return -1;
  }
  int content_size = strlen(contents);
  int num_blocks = content_size % 512 == 0
                       ? (content_size / BLOCK_SIZE)
                       : ((content_size + BLOCK_SIZE - 1) / BLOCK_SIZE);
  if (num_blocks > MAX_BLOCKS_PER_FILE) {
    printf("File too large \n");
    return -1;
  }
  for (int i = 0; i < num_blocks; i++) {
    int block_idx = findFreeBlock();
    if (block_idx == -1) {
      printf("No free blocks available\n");
      return -1;
    }
    inode_table[inode_idx].blocks[i] = block_idx;
    // maybe clear block before writing
    strncpy(disk[block_idx], contents + i * BLOCK_SIZE, BLOCK_SIZE);
  }
  inode_table[inode_idx].size = content_size;
  strncpy(inode_table[inode_idx].name, filename, 20 - 1);
  inode_table[inode_idx].name[20 - 1] = '\0';
  return 0;
}

void read_file(const char *filename) {
  for (int i = 0; i < MAX_FILES; i++) {
    if (strcmp(inode_table[i].name, filename) == 0) {
      printf("Reading file '%s':\n", filename);
      int bytes_left = inode_table[i].size;
      for (int j = 0; j < MAX_BLOCKS_PER_FILE &&
                      inode_table[i].blocks[j] != -1 && bytes_left > 0;
           j++) {
        int block_idx = inode_table[i].blocks[j];
        int bytes_to_read = bytes_left > BLOCK_SIZE ? BLOCK_SIZE : bytes_left;
        printf("%.*s", bytes_to_read, disk[block_idx]);
        bytes_left -= bytes_to_read;
      }
      printf("\n");
      return;
    }
  }
  printf("File not found\n");
}
int main(int argc, char *argv[]) {
  init_file_system();
  createFile("neo_arxeio", "test test test");
  createFile("neo_arxeio2", "test test test test test");
  read_file("neo_arxeio");
  read_file("neo_arxeio2");
  free_file_system();
  return 0;
}
