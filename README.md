# CLI File Manager

A modular command-line file manager written in C++17.

The project is being developed progressively to demonstrate practical C++ programming, filesystem manipulation, command parsing, recursion, sorting, searching, error handling, CMake, and software architecture.

## Features

### Navigation

* List directory contents
* Show current working directory
* Change directories
* Navigate to the parent directory
* Support relative and absolute paths
* Support `.` and `..`

### File Operations

* Create directories
* Create files
* Rename files and directories
* Copy files and directories
* Move files and directories
* Remove files and directories recursively

### File Information

* Show file size
* Show file type
* Show modification time
* Show detailed information about an item

### Recursive Operations

* Display directory trees
* Calculate directory sizes
* Recursively copy directories
* Recursively remove directories

### Search and Filtering

* Search for files and directories by name
* Search recursively
* Search by file extension
* Search by minimum file size
* Filter directory listings to files or directories

### Sorting

* Sort directory contents by name
* Sort directory contents by size
* Sort in ascending or descending order

### Command Parsing

* Parse commands and arguments
* Support quoted paths containing spaces
* Support both single and double quotes

## Example

```text
CLI File Manager

Type 'help' for available commands.

> pwd
/Users/example/projects/cli-file-manager

> ls
CMakeLists.txt
README.md
include/
src/
tests/

> mkdir "My Folder"
Directory created: My Folder

> cd "My Folder"

> touch "hello world.txt"
File created: hello world.txt

> ls
hello world.txt  [0 bytes]

> info "hello world.txt"
Name: hello world.txt
Path: /Users/example/projects/cli-file-manager/My Folder/hello world.txt
Type: regular file
Size: 0 bytes
Modified: 2026-09-03 10:30:00

> cd ..

> find "hello world.txt"
"/Users/example/projects/cli-file-manager/My Folder/hello world.txt"
```

## Commands

### Navigation

```text
ls
ls --files
ls --dirs
ls --name
ls --size
ls --name-desc
ls --size-desc

pwd
cd <directory>
back
```

### File Operations

```text
mkdir <directory>
touch <file>
rename <old> <new>
cp <source> <destination>
mv <source> <destination>
rm <path>
```

### Information

```text
size <file>
type <path>
modified <path>
info <path>
```

### Recursive Operations

```text
tree [path]
du <path>
```

### Search

```text
find <name> [path]
findext <extension> [path]
findsize <minimum_bytes> [path]
```

### Other

```text
help
q
quit
```

## Quoted Paths

The command parser supports paths containing spaces.

For example:

```text
mkdir "My Folder"
cd "My Folder"
touch "hello world.txt"
find "hello world.txt"
```

Both single and double quotes are supported:

```text
touch 'hello world.txt'
touch "hello world.txt"
```

## Path Handling

The file manager supports:

* Relative paths

```text
cd src
```

* Absolute paths

```text
cd /Users/example/Documents
```

* Current directory

```text
tree .
```

* Parent directory

```text
cd ..
```

Paths are normalized using `std::filesystem::path::lexically_normal()`.

## Architecture

The project separates command parsing from filesystem operations.

```text
User
  ↓
main.cpp
  ↓
CommandParser
  ↓
FileManager
  ↓
std::filesystem
  ↓
Operating System
```

### `main.cpp`

Responsible for:

* Running the main command loop
* Receiving user input
* Calling `CommandParser`
* Validating command arguments
* Calling the appropriate `FileManager` function

### `CommandParser`

Responsible for:

* Splitting input into tokens
* Identifying the command
* Identifying arguments
* Handling quoted arguments

### `FileManager`

Responsible for:

* Directory navigation
* File operations
* Metadata
* Recursive operations
* Searching
* Sorting
* Path resolution

### `std::filesystem`

Provides the interface used to communicate with the operating system's filesystem.

## Project Structure

```text
cli-file-manager/
├── build/
├── docs/
├── include/
│   ├── FileManager.h
│   └── CommandParser.h
├── src/
│   ├── main.cpp
│   ├── FileManager.cpp
│   └── CommandParser.cpp
├── tests/
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Technologies

* C++17
* C++ Standard Library
* `std::filesystem`
* CMake
* Git
* GitHub

## Building

Clone the repository and enter the project directory:

```bash
cd cli-file-manager
```

Configure the project:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build
```

Run:

```bash
./build/filemanager
```

## Development Phases

### Phase 0 — Project Setup

* CMake project
* C++ executable
* Git/GitHub
* Project structure

### Phase 1 — Navigation

* `ls`
* `pwd`
* `cd`
* `back`
* Command loop
* Basic error handling

### Phase 2 — File Operations

* `mkdir`
* `touch`
* `rename`
* `cp`
* `mv`
* `rm`

### Phase 3 — File Metadata

* File size
* File type
* Modification time
* File information

### Phase 4 — Architecture

* Header/source separation
* `FileManager` class
* `CommandParser`
* Improved project organization

### Phase 5 — Recursive Filesystem Operations

* Recursive directory tree
* Recursive directory size
* Recursive copying
* Recursive removal

### Phase 6 — Command Parsing and Paths

* Quoted arguments
* Paths containing spaces
* Relative paths
* Absolute paths
* `.` and `..`
* Path normalization

### Phase 7 — Search, Sorting and Filtering

* Recursive name search
* Extension search
* Size-based search
* File/directory filtering
* Name sorting
* Size sorting
* Ascending/descending sorting

### Future Phases

Potential future development includes:

* Improved command parsing
* Permission information
* Unix file permissions
* Symbolic links
* Interactive deletion confirmation
* More advanced search expressions
* File previews
* Configuration files
* Automated unit tests
* Improved user interface
* Performance improvements
* Cross-platform improvements

## Design Principles

The project follows several software engineering principles:

### Separation of Responsibilities

Command parsing and filesystem manipulation are kept separate.

### Reusable Components

Filesystem functionality is implemented inside `FileManager` rather than directly inside the command loop.

### Error Handling

Filesystem operations are protected against common errors and report useful messages to the user.

### Incremental Development

The application is developed through multiple phases, with each phase introducing additional functionality.

### Standard Library

The project uses the C++ standard library, particularly `std::filesystem`, instead of implementing low-level filesystem functionality from scratch.

## Learning Goals

This project is designed to provide practical experience with:

* Modern C++
* Object-oriented programming
* C++17
* Classes and encapsulation
* Header/source separation
* Command-line applications
* Filesystem programming
* Recursive algorithms
* Searching
* Sorting
* Lambda functions
* Exception handling
* String parsing
* CMake
* Git
* GitHub
* Software architecture

## Project Status

**Completed through Phase 7.**

The current version supports filesystem navigation, file operations, metadata inspection, recursive operations, path handling, command parsing, searching, filtering, and sorting.
