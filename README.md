# File System Simulation

This project implements a basic file system simulation in C. The file system supports both files and directories, allowing you to create, read, and list files and directories in a hierarchical structure. The file system operates in memory, with data organized into blocks, and supports basic operations such as file creation, directory navigation, and file reading.

## Features

- **File and Directory Management:** Create and navigate directories, and create and read files within directories.
- **In-Memory Simulation:** The file system is entirely in memory, with blocks representing the storage medium.
- **Hierarchical Structure:** Supports hierarchical organization of files and directories, similar to Unix-like file systems.

## Installation

### Prerequisites

To compile and run this project, you need a C compiler (e.g., `gcc`) installed on your system.

### Cloning the Repository

```bash
git clone git@github.com:nikolaospanagopoulos/customFileSystem.git
cd customFileSystem
```

### Compiling

```bash
gcc -o filesystem main.c file_system.c
```

### Running

```bash
./filesystem
```
