#include "file_system.h"

int main() {
  init_file_system();

  // start in root dir
  int current_directory = 0;

  int dir1 = create_directory("dir1", current_directory);

  // change to new directory
  current_directory = change_directory("dir1", current_directory);

  // Create a file in the new directory
  createFile("file1.txt", "This is some test content in file1.",
             current_directory);

  read_file("file1.txt", current_directory);

  list_directory(current_directory);

  // go back to root
  current_directory = change_directory("..", current_directory);

  list_directory(current_directory);

  int dir2 = create_directory("dir2", current_directory);

  int dir3 = create_directory("dir3", current_directory);

  createFile("file3.txt", "This is test content in file3", current_directory);
  read_file("file3.txt", current_directory);

  current_directory = change_directory("dir2", current_directory);
  createFile("file4.txt", "This is test content in file4", current_directory);

  list_directory(current_directory);
  read_file("file4.txt", current_directory);
  free_file_system();
  return 0;
}
