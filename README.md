# CLI File Manager

A developer-oriented Unix CLI file manager written in C++17.

The project provides filesystem navigation, file operations, metadata inspection, recursive operations, searching, sorting, Unix permissions, hard links, symbolic links, robust error handling, and safety protections.

The goal is to build a serious systems-oriented portfolio project while learning how C++ interacts with the operating system and filesystem.

---

## Features

### Navigation

* `pwd`
* `cd`
* `back`
* `ls`
* `ls -a`
* `ls --files`
* `ls --dirs`
* Sorting by name
* Sorting by size
* Ascending and descending sorting

### File Operations

* Create directories
* Create files
* Rename files and directories
* Copy files
* Copy directories recursively
* Move files and directories
* Delete files
* Delete directories recursively with confirmation

### File Information

* File size
* File type
* Modification time
* Detailed information
* Unix permissions
* Symbolic-link targets

### Recursive Operations

* Directory tree
* Directory size calculation

### Search

* Search by filename
* Search by extension
* Search by minimum file size

### Unix Features

* Read Unix permissions
* Change permissions with `chmod`
* Create hard links
* Create symbolic links
* Read symbolic-link targets

### Phase 9 Safety

* Centralized exception handling
* Invalid-command detection
* Argument-count validation
* Unmatched quote detection
* Invalid permission validation
* Invalid size validation
* Protection against accidental overwrites
* Protection against copying a directory into itself
* Protection against moving a directory into itself
* Protection against deleting the current directory
* Protection against deleting `.` and `..`
* Protection against deleting the filesystem root
* Confirmation before recursive deletion
* Graceful handling of filesystem errors
* Graceful handling of EOF / `Ctrl+D`

---

# Commands

## Navigation

```text
pwd
cd <path>
back
```

## Listing

```text
ls
ls -a
ls --files
ls --dirs
ls --name
ls --size
ls --name-desc
ls --size-desc
```

Examples:

```text
ls
ls -a
ls --files
ls --dirs
ls --size-desc
```

---

## File Operations

```text
mkdir <name>
touch <name>
rename <old> <new>
cp <source> <destination>
mv <source> <destination>
rm <path>
```

Example:

```text
mkdir project
cd project
touch main.cpp
rename main.cpp app.cpp
cp app.cpp backup.cpp
mv backup.cpp backup/
```

Deleting a directory requires confirmation.

---

## Information

```text
size <file>
type <path>
modified <path>
info <path>
```

Example:

```text
info main.cpp
```

Possible output:

```text
Name: main.cpp
Path: /project/main.cpp
Type: regular file
Size: 1024 bytes
Permissions: -rw-r--r--
Modified: 2026-09-03 12:00:00
```

---

## Recursive Operations

```text
tree
tree <path>
du <path>
```

Example:

```text
tree .
```

---

## Search

```text
find <name> [path]
findext <extension> [path]
findsize <minimum_bytes> [path]
```

Examples:

```text
find main.cpp
find main.cpp src
findext cpp
findext .cpp src
findsize 100000 .
```

---

## Permissions

```text
perm <path>
chmod <mode> <path>
```

Examples:

```text
perm script.sh
chmod 755 script.sh
chmod 644 file.txt
chmod 600 secret.txt
```

Permission modes use Unix octal notation.

Examples:

```text
755
644
600
700
```

---

## Hard Links

```text
link <target> <link>
```

or:

```text
ln <target> <link>
```

Example:

```text
touch original.txt
link original.txt hardcopy.txt
```

---

## Symbolic Links

```text
ln -s <target> <link>
```

Example:

```text
ln -s original.txt shortcut.txt
readlink shortcut.txt
```

---

# Safety

The file manager deliberately avoids dangerous operations.

For example:

```text
rm project
```

will ask for confirmation if `project` is a directory.

The program also prevents:

```text
rm .
rm ..
```

and refuses to delete the current working directory or filesystem root.

The program also prevents operations such as:

```text
cp project project/src/copy
```

because copying a directory into itself would create an invalid recursive operation.

Existing destinations are not silently overwritten.

---

# Architecture

The project follows a layered design:

```text
User
  ↓
main.cpp
  ↓
CommandParser
  ↓
FileManager
  ↓
std::filesystem + POSIX APIs
  ↓
Operating System
```

### `main.cpp`

Responsible for:

* Running the command loop
* Dispatching commands
* Validating command arguments
* Displaying errors
* Displaying help
* Handling EOF / `Ctrl+D`

### `CommandParser`

Responsible for:

* Splitting commands into tokens
* Handling quoted paths
* Detecting unmatched quotes

### `FileManager`

Responsible for:

* Filesystem operations
* Navigation
* Search
* Recursive operations
* Permissions
* Links
* Safety checks
* Path validation

### `std::filesystem`

Provides portable C++17 filesystem functionality.

### POSIX

Used for Unix-specific operations such as:

* `lstat`
* `chmod`
* Unix permission inspection

---

# Error Handling

The application uses exception handling to prevent user errors from terminating the program.

The general flow is:

```text
User Input
    ↓
Command Parsing
    ↓
Argument Validation
    ↓
Path Validation
    ↓
Safety Checks
    ↓
Filesystem Operation
    ↓
Error Handling
    ↓
User-Friendly Message
```

For example:

```text
cp project project/src/copy
```

is detected as an unsafe recursive operation before the filesystem operation is performed.

---

# Project Structure

```text
cli-file-manager/
│
├── include/
│   ├── CommandParser.h
│   └── FileManager.h
│
├── src/
│   ├── CommandParser.cpp
│   ├── FileManager.cpp
│   └── main.cpp
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

# Technologies

* C++17
* C++ Standard Library
* `std::filesystem`
* POSIX APIs
* CMake
* Git
* GitHub
* Unix filesystem concepts

---

# Build

Clone the repository:

```bash
git clone https://github.com/SarinaTari/cli-file-manager.git
```

Enter the project:

```bash
cd cli-file-manager
```

Configure:

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

---

# Phase Progress

## Phase 0

Project setup

## Phase 1

Basic navigation and filesystem access

## Phase 2

File operations

## Phase 3

File metadata

## Phase 4

Project architecture

## Phase 5

Recursive filesystem operations

## Phase 6

Command parsing and path handling

## Phase 7

Search, sorting, and filtering

## Phase 8

Unix permissions and links

## Phase 9

Robust error handling and filesystem safety

## Phase 10

Testing

## Phase 11

Interactive terminal UI

## Phase 12

Advanced file-manager functionality

## Phase 13

Developer/project intelligence

## Phase 14

History, undo, and snapshots

## Phase 15

Performance, reliability, and portfolio release

---

# Phase 9 Concepts

This phase focuses on software reliability and defensive programming.

Important concepts include:

* Exceptions
* `try`
* `catch`
* `std::exception`
* Input validation
* Defensive programming
* Error propagation
* Filesystem errors
* `std::error_code`
* Path validation
* Canonical paths
* Recursive-operation safety
* Destructive-operation confirmation
* Edge cases
* Graceful failure

---

# Design Philosophy

The project is not intended to blindly reproduce existing Unix commands.

Instead, it aims to provide a safe and developer-oriented filesystem environment.

Future versions will introduce:

* Duplicate-file detection
* Storage analysis
* Git awareness
* Project detection
* Operation history
* Undo
* Snapshots
* Interactive terminal UI
* Developer-oriented filesystem analysis

---

# Long-Term Goal

The final project will demonstrate practical knowledge of:

```text
C++
 ↓
Object-Oriented Programming
 ↓
Standard Library
 ↓
Filesystem APIs
 ↓
Unix/POSIX
 ↓
Operating Systems
 ↓
System Programming
 ↓
Testing
 ↓
Performance
 ↓
Software Architecture
```

The project is developed incrementally so that every phase introduces new engineering concepts.

---

# Project Vision

The final goal is to turn this from a simple CLI file manager into a:

**Developer-oriented, safe, intelligent Unix file manager.**

Planned advanced capabilities include:

```text
Filesystem
    ├── Navigation
    ├── File Operations
    ├── Permissions
    ├── Links
    └── Search

Developer Intelligence
    ├── Project Detection
    ├── Git Awareness
    ├── Storage Analysis
    ├── Duplicate Detection
    └── Dependency Analysis

Safety
    ├── Safe Deletion
    ├── Operation History
    ├── Undo
    └── Snapshots

Interface
    └── Interactive Terminal UI
```

---

## Current Status

**Phase 9 / 15 — Robust Error Handling & Safety**

The project currently supports filesystem navigation, file operations, recursive operations, metadata inspection, searching, sorting, filtering, Unix permissions, hard links, symbolic links, robust validation, exception handling, and filesystem safety protections.
