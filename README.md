# CLI File Manager

A command-line file manager written in C++ using `std::filesystem`.

The project is being developed progressively to demonstrate practical C++ programming, filesystem operations, command parsing, software architecture, error handling, and Git/GitHub workflow.

## Features

### Navigation

* List directory contents
* Show current working directory
* Change directories
* Navigate to parent directories

### File Operations

* Create directories
* Create files
* Rename files and directories
* Copy files and directories
* Move files and directories
* Remove files and directories

### File Information

* Show file size
* Identify file types
* Show modification time
* Display detailed file information

### Recursive Operations

* Display directory trees
* Calculate directory sizes
* Recursively copy directories
* Recursively remove directories

### Command Parsing

* Command tokenization
* Quoted arguments
* File and directory names containing spaces
* Relative paths
* Absolute paths
* Path normalization
* Improved argument validation

## Example

```text
CLI File Manager

Type 'help' for available commands.

> pwd
"/Users/mym1/Documents/Projects/cli-file-manager"

> mkdir "My Projects"
Directory created: My Projects

> cd "My Projects"

> touch "hello world.txt"
File created: hello world.txt

> ls
hello world.txt  [0 bytes]

> info "hello world.txt"
Name: hello world.txt
Path: ".../My Projects/hello world.txt"
Type: regular file
Size: 0 bytes

> cd ..
```

## Commands

### Navigation

```text
ls
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

### Other

```text
help
q
quit
```

## Quoted Paths

Paths containing spaces can be written using quotes:

```text
cd "My Folder"
```

```text
touch "hello world.txt"
```

```text
rename "old file.txt" "new file.txt"
```

Both single and double quotes are supported:

```text
touch 'hello world.txt'
```

## Path Handling

The file manager supports:

### Relative paths

```text
cd photos
cd photos/vacation
cd ..
```

### Absolute paths

```text
cd /Users/mym1/Documents
```

### Current directory

```text
tree .
```

Paths are normalized using `std::filesystem::path::lexically_normal()`.

## Architecture

The project follows a simple layered architecture:

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

* Running the command loop
* Receiving user input
* Dispatching commands
* Validating argument counts

### `CommandParser`

Responsible for:

* Tokenizing user input
* Handling quoted arguments
* Separating commands from arguments

### `FileManager`

Responsible for:

* Filesystem operations
* Directory navigation
* Metadata
* Recursive operations
* Path resolution
* Error handling

### `std::filesystem`

Provides the C++ interface to the underlying filesystem.

## Project Structure

```text
cli-file-manager/
├── build/
├── docs/
├── include/
│   ├── CommandParser.h
│   └── FileManager.h
├── src/
│   ├── CommandParser.cpp
│   ├── FileManager.cpp
│   └── main.cpp
├── tests/
├── CMakeLists.txt
├── README.md
└── .gitignore
```

## Technologies

* C++
* C++17
* CMake
* `std::filesystem`
* C++ Standard Library
* Git
* GitHub

## Building

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

Run the program:

```bash
./build/filemanager
```

## Development Phases

### Phase 0 — Project Setup

* CMake project
* Basic C++ executable
* Git repository
* GitHub repository
* Initial project structure

### Phase 1 — Navigation

* `ls`
* `pwd`
* `cd`
* `back`
* Basic command loop
* Basic error handling

### Phase 2 — File Operations

* `mkdir`
* `touch`
* `rename`
* `cp`
* `mv`
* `rm`
* Command validation

### Phase 3 — File Information

* File sizes
* File types
* Modification times
* Detailed file information

### Phase 4 — Architecture

* Separated headers and implementations
* `FileManager` class
* `CommandParser` class
* Cleaner project structure
* Separation of responsibilities

### Phase 5 — Recursive Filesystem Operations

* Recursive directory trees
* Recursive directory size calculation
* Recursive directory copying
* Recursive directory deletion
* `tree`
* `du`

### Phase 6 — Command Parsing & Path Handling

* Tokenization
* Quoted arguments
* Paths containing spaces
* Relative paths
* Absolute paths
* Path normalization
* Improved filesystem validation
* Safer filesystem operations

## Design Principles

The project focuses on:

* Separation of concerns
* Reusable classes
* Clear interfaces
* Error handling
* Standard C++ facilities
* Incremental development
* Testable functionality
* Clean project organization

## Future Development

Planned future improvements include:

* More advanced command parsing
* Search functionality
* Sorting and filtering
* File permissions
* Unix-specific filesystem features
* Better interactive interface
* Automated tests
* Confirmation before destructive operations
* Configuration support
* Performance improvements
* Portfolio-level polish

## Learning Goals

This project is designed to provide practical experience with:

* C++17
* Object-oriented programming
* Filesystem programming
* Command-line applications
* Recursion
* Path manipulation
* Parsing
* Error handling
* CMake
* Git and GitHub
* Software architecture

## Project Status

**Current phase: Phase 6 — Command Parsing & Path Handling**

The project is actively being developed phase by phase.
