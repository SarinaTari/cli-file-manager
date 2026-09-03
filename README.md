# CLI File Manager

A developer-oriented Unix CLI file manager built in **C++17**, featuring filesystem navigation, file operations, metadata inspection, recursive search, permissions and links, storage analysis, duplicate detection, automated testing, and an interactive terminal UI.

---

## Overview

This project is a Unix-oriented command-line file manager designed to go beyond basic file manipulation.

The goal is to build a practical systems-programming application while learning how operating systems interact with files, directories, permissions, links, and the filesystem.

The project is being developed incrementally, with each phase introducing a new layer of functionality, architecture, safety, testing, or usability.

---

## Features

### Navigation

* Show current working directory
* Change directories
* Navigate to parent directories
* List directory contents
* Show hidden files
* Sort directory entries
* Filter files and directories

### File Operations

* Create files
* Create directories
* Rename files and directories
* Copy files
* Copy directories recursively
* Move files and directories
* Delete files
* Recursively delete directories
* Confirmation before recursive deletion
* Protection against dangerous deletion paths
* Protection against overwriting existing destinations

### File Information

* File size
* File type
* Modification time
* Detailed file information
* Directory tree visualization
* Directory size calculation

### Recursive Search

* Search by filename
* Search by file extension
* Search by minimum file size
* Recursive directory traversal

### Permissions and Links

* Display Unix permissions
* Change file permissions
* Create hard links
* Create symbolic links
* Inspect symbolic link targets

### Storage Analysis

Phase 12 introduces storage analysis functionality.

The analyzer can:

* Count directories
* Count files
* Calculate total storage usage
* Identify the largest files
* Recursively analyze directory contents

Example:

```text
Storage analysis for: "project"

Directories: 4
Files: 18
Total size: 52480 bytes

Largest files:
  1. project/build/app (18240 bytes)
  2. project/data.db (12400 bytes)
  3. project/src/main.cpp (5200 bytes)
```

### Duplicate Detection

The file manager can detect duplicate files by:

1. Collecting regular files recursively
2. Grouping files by size
3. Hashing files within matching size groups
4. Comparing candidate files byte-by-byte
5. Reporting confirmed duplicate groups

Example:

```text
Duplicate group (size: 12 bytes)
Hash: 8a5f...

  duplicate-test/a.txt
  duplicate-test/b.txt
```

This combines filesystem traversal, hashing, binary file I/O, data structures, and algorithmic optimization.

### Interactive Terminal UI

The project also includes an interactive terminal interface that can be launched with:

```text
ui
```

The UI provides keyboard-driven navigation through the filesystem while still using the same underlying `FileManager` functionality.

---

## Commands

### Navigation

```text
ls [options] [filter]
pwd
cd <directory>
back
```

Examples:

```text
ls
ls -a
ls size
ls files
pwd
cd Documents
back
```

### File Operations

```text
mkdir <name>
touch <name>
rename <old> <new>
cp <source> <destination>
mv <source> <destination>
rm <name>
```

Example:

```text
mkdir projects
cd projects
touch main.cpp
rename main.cpp app.cpp
```

### Information

```text
size <name>
type <name>
modified <name>
info <name>
tree [name]
du [name]
```

### Search

```text
find <name> [path]
findext <extension> [path]
findsize <minimum-size> [path]
```

Examples:

```text
find main.cpp .
findext .cpp .
findsize 1000 .
```

### Permissions and Links

```text
perm <name>
chmod <mode> <name>
ln <target> <link>
symlink <target> <link>
linktarget <name>
```

### Advanced Features

```text
analyze [path]
duplicates [path]
```

### Interface

```text
ui
help
q
quit
exit
```

---

## Architecture

The project follows a layered architecture:

```text
                User
                  │
                  ▼
          ┌─────────────────┐
          │   CommandParser │
          └────────┬────────┘
                   │
                   ▼
          ┌─────────────────┐
          │   Command Mode  │
          │    / TerminalUI │
          └────────┬────────┘
                   │
                   ▼
          ┌─────────────────┐
          │   FileManager   │
          └────────┬────────┘
                   │
          ┌────────┴─────────┐
          ▼                  ▼
   std::filesystem       POSIX APIs
          │                  │
          └────────┬─────────┘
                   ▼
              Unix/macOS
               Filesystem
```

Advanced functionality is separated into dedicated components:

```text
FileManager
    │
    ├── FileHasher
    │
    ├── DuplicateDetector
    │
    └── StorageAnalyzer
```

This separation keeps the project modular and makes future features easier to add.

---

## Project Structure

```text
cli-file-manager/
│
├── CMakeLists.txt
├── README.md
├── .gitignore
│
├── include/
│   ├── CommandParser.h
│   ├── FileManager.h
│   ├── TerminalUI.h
│   ├── FileHasher.h
│   ├── DuplicateDetector.h
│   └── StorageAnalyzer.h
│
├── src/
│   ├── main.cpp
│   ├── CommandParser.cpp
│   ├── FileManager.cpp
│   ├── TerminalUI.cpp
│   ├── FileHasher.cpp
│   ├── DuplicateDetector.cpp
│   └── StorageAnalyzer.cpp
│
└── tests/
    └── test_cli.sh
```

---

## Building

### Requirements

* C++17 compatible compiler
* CMake 3.16+
* Unix-like operating system
* Bash
* CTest

The project has been developed and tested on macOS using Clang.

### Build

Clone the repository:

```bash
git clone https://github.com/SarinaTari/cli-file-manager.git
cd cli-file-manager
```

Create the build directory:

```bash
cmake -S . -B build
```

Build the project:

```bash
cmake --build build
```

The executable will be created at:

```text
build/filemanager
```

---

## Running

Run the application with:

```bash
./build/filemanager
```

You will see:

```text
CLI File Manager
Type 'help' for available commands.

>
```

For example:

```text
> pwd
> ls
> mkdir test
> cd test
> touch example.txt
> info example.txt
> tree
> back
```

---

## Testing

The project contains automated integration tests covering the command-line interface and filesystem behavior.

Run the tests with:

```bash
ctest --test-dir build --output-on-failure
```

The test suite currently covers:

* Navigation
* File creation
* Directory creation
* Quoted paths
* Rename
* Copy
* Move
* File information
* Directory trees
* Directory size
* Recursive search
* Permissions
* Symbolic links
* Hard links
* Invalid arguments
* Error handling
* Overwrite protection
* Dangerous path protection
* Safe recursive deletion
* Copy/move safety
* Storage analysis
* Duplicate detection
* Empty-directory handling
* Invalid paths

Current target:

```text
Passed: 51
Failed: 0
```

---

## Safety

File managers can perform destructive operations, so safety is an important part of the project.

The application includes protections against:

* Removing the current directory
* Removing the parent directory
* Removing the filesystem root
* Accidentally recursively deleting directories
* Copying a directory into itself
* Moving a directory into itself
* Overwriting existing destinations

Recursive directory deletion requires explicit confirmation:

```text
Warning: this will recursively delete directory "example".

Are you sure? [y/N]:
```

Only `y` or `Y` confirms the operation.

---

## Technologies

### Programming

* C++17
* Standard Library
* `std::filesystem`
* Binary file I/O
* Hashing
* Recursion
* STL containers
* Exception handling

### Operating Systems

* Unix filesystem concepts
* POSIX permissions
* Hard links
* Symbolic links
* Unix paths
* macOS / Unix terminal behavior

### Build System

* CMake

### Testing

* Bash
* CTest
* Automated integration testing

### Development Tools

* Clang
* Git
* GitHub
* VS Code

---

## Development Phases

The project is being developed progressively.

### Phase 0 — Setup

* Project structure
* CMake
* C++17
* Initial executable

### Phase 1 — Navigation

* `ls`
* `pwd`
* `cd`
* `back`

### Phase 2 — File Operations

* `mkdir`
* `touch`
* `rename`
* `cp`
* `mv`
* `rm`

### Phase 3 — Metadata

* File size
* File type
* Modification time
* Detailed information

### Phase 4 — Architecture

* Command parser
* Separation of responsibilities
* Modular source/header structure

### Phase 5 — Recursive Filesystem

* Directory trees
* Recursive directory traversal
* Directory size calculation

### Phase 6 — Path Handling

* Improved path resolution
* Quoted arguments
* Path safety

### Phase 7 — Search

* Filename search
* Extension search
* Size-based search

### Phase 8 — Unix Features

* Permissions
* `chmod`
* Hard links
* Symbolic links

### Phase 9 — Reliability and Safety

* Error handling
* Input validation
* Dangerous-path protection
* Recursive deletion confirmation
* Copy/move safety

### Phase 10 — Testing

* Automated integration tests
* CTest integration
* Filesystem behavior testing
* Error-condition testing

### Phase 11 — Terminal UI

* Interactive terminal interface
* Keyboard navigation
* Command mode
* Terminal rendering

### Phase 12 — Advanced Filesystem Intelligence

* Storage analysis
* File hashing
* Duplicate detection
* Content comparison
* Large-file identification

---

## Design Philosophy

The project is intentionally being built as more than a collection of filesystem commands.

The main design goals are:

### Safety

Destructive filesystem operations should fail safely rather than silently causing damage.

### Modularity

Functionality is divided into separate components instead of placing the entire application inside `main.cpp`.

### Testability

Important behavior is covered by automated integration tests.

### Unix Awareness

The project uses Unix filesystem concepts such as:

* Permissions
* Hard links
* Symbolic links
* Recursive traversal
* Unix paths
* POSIX terminal behavior

### Practicality

The final application should resemble a useful developer tool rather than simply demonstrating isolated C++ features.

---

## Future Roadmap

Future phases will expand the project toward a more intelligent developer-oriented file manager.

Planned features include:

* Git repository awareness
* Automatic project detection
* C++ include/dependency analysis
* Project statistics
* Advanced filtering and sorting
* File operation history
* Undo functionality
* Snapshots
* Directory comparison
* Improved terminal UI
* Performance optimization
* More comprehensive testing
* Configuration files
* User-defined aliases
* Scripting support
* Remote filesystem support

The long-term goal is to turn the project into a lightweight developer-oriented alternative to traditional terminal file managers.

---

## What This Project Demonstrates

This project demonstrates practical experience with:

* C++17
* Object-oriented design
* Filesystem programming
* Unix/Linux concepts
* POSIX permissions
* Links and filesystem metadata
* Recursive algorithms
* Binary file processing
* Hashing
* Error handling
* Input parsing
* CMake
* Automated testing
* Bash scripting
* Terminal interfaces
* Git/GitHub
* Software architecture

It also demonstrates the ability to build a larger project incrementally rather than writing a single small program.

---

## Portfolio Description

> **CLI File Manager** — A developer-oriented Unix file manager built in C++17 with recursive filesystem operations, permissions and links, intelligent storage analysis, duplicate detection, safe deletion, automated integration testing, and an interactive terminal UI.

---

## Status

**Current phase: Phase 12 — Advanced Filesystem Intelligence**

The project is actively being developed toward a complete developer-oriented Unix file manager.

---

## License

This project is currently intended as a personal educational and portfolio project.

````

### After replacing the README

Run:

```bash
cd ~/Documents/Projects/cli-file-manager
````

Then:

```bash
git diff -- README.md
```

Then test one final time:

```bash
ctest --test-dir build --output-on-failure
```

**Don't commit yet until we see `51 passed / 0 failed`.** After that, we'll do the GitHub cleanup, commit, push, and make sure your repository looks portfolio-ready.
