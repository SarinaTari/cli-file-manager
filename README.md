# CLI File Manager

A developer-oriented Unix CLI file manager written in **C++17**.

The project started as a simple command-line filesystem program and is being developed progressively into a safer, more powerful, and more intelligent terminal-based file manager.

The goal is not simply to recreate `ls`, `cp`, `mv`, and `rm`, but to build a portfolio-quality systems project that demonstrates practical knowledge of:

* C++
* Object-oriented programming
* C++17 `std::filesystem`
* Unix/Linux filesystem concepts
* File permissions
* Hard and symbolic links
* Recursive filesystem operations
* Command parsing
* Error handling
* Defensive programming
* Automated testing
* CMake
* Terminal UI development
* Git and GitHub

---

## Current Status

**Phase 11 / 15 — Interactive Terminal UI**

The project currently supports:

* Directory navigation
* File and directory creation
* Rename
* Copy
* Move
* Safe deletion
* File metadata
* Recursive directory trees
* Directory size calculation
* Recursive searching
* Extension searching
* Size-based searching
* Unix permissions
* Permission modification
* Hard links
* Symbolic links
* Link target inspection
* Robust error handling
* Filesystem safety checks
* Automated integration testing
* CTest integration
* Interactive terminal UI

---

# Features

## 1. Directory Navigation

Navigate through the filesystem without leaving the application.

```text
pwd
cd <directory>
back
```

The program maintains its own current working directory.

---

## 2. Directory Listing

List files and directories with different options.

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

Supported functionality includes:

* Hidden files
* Files-only filtering
* Directories-only filtering
* Name sorting
* Size sorting
* Ascending and descending order

---

## 3. File Operations

Create and manipulate filesystem objects.

```text
mkdir <name>
touch <name>
rename <old> <new>
cp <source> <destination>
mv <source> <destination>
rm <name>
```

---

## 4. File Information

Inspect filesystem metadata.

```text
info <path>
```

The information system can display information such as:

* File name
* File type
* File size
* Modification time
* Permissions
* Other filesystem metadata

Individual metadata commands are also available:

```text
size <path>
type <path>
modified <path>
```

---

## 5. Recursive Filesystem Operations

The file manager can recursively inspect directories.

### Tree

```text
tree
tree <path>
```

Example:

```text
project/
├── include/
│   ├── FileManager.h
│   └── CommandParser.h
├── src/
│   ├── FileManager.cpp
│   └── main.cpp
└── CMakeLists.txt
```

### Directory Size

```text
du <path>
```

The program recursively calculates directory storage usage.

---

# 6. Search

The file manager supports recursive searching.

### Search by name

```text
find <name>
find <name> <path>
```

### Search by extension

```text
findext <extension>
findext <extension> <path>
```

### Search by minimum size

```text
findsize <minimum_bytes>
findsize <minimum_bytes> <path>
```

---

# 7. Unix Permissions

Inspect and modify Unix permissions.

### View permissions

```text
perm <path>
```

Example:

```text
-rwxr-xr--
```

### Change permissions

```text
chmod <mode> <path>
```

Example:

```text
chmod 755 script.sh
```

The application validates permission modes before attempting to modify them.

---

# 8. Hard Links and Symbolic Links

The project also demonstrates Unix filesystem links.

### Create a hard link

```text
link <target> <link>
```

or:

```text
ln <target> <link>
```

### Create a symbolic link

```text
ln -s <target> <link>
```

### Inspect a symbolic link

```text
readlink <path>
```

---

# 9. Command Parsing

Commands are parsed separately from filesystem operations.

For example:

```text
rename "old file.txt" "new file.txt"
```

The command parser handles:

* Commands
* Arguments
* Whitespace
* Single quotes
* Double quotes
* Quoted paths containing spaces
* Invalid/unmatched quotes

This separates input parsing from filesystem logic.

---

# 10. Error Handling

Filesystem operations can fail for many reasons.

Examples include:

* File does not exist
* Directory does not exist
* Permission denied
* Destination already exists
* Invalid path
* Invalid command
* Invalid permission mode
* Invalid numeric argument

The application catches exceptions and reports meaningful errors instead of terminating unexpectedly.

Example:

```text
Error: Item does not exist.
```

---

# 11. Filesystem Safety

Destructive filesystem operations are treated carefully.

The application includes safety checks for operations such as:

```text
rm
mv
cp
rename
```

Protection includes checks against:

* Dangerous filesystem paths
* Invalid destinations
* Copying/moving a directory into itself
* Conflicting destinations
* Invalid filesystem operations

Recursive deletion requires an explicit confirmation.

This is an intentional design decision because filesystem applications can cause irreversible damage if destructive operations are implemented carelessly.

---

# 12. Automated Testing

The project includes automated integration tests.

Tests are executed through **CTest**.

Run:

```bash
ctest --test-dir build --output-on-failure
```

The tests verify important application behavior including:

* Navigation
* File creation
* Directory creation
* Rename
* Copy
* Move
* Delete
* Search
* Permissions
* Links
* Error handling
* Quoted paths

Testing is an important part of the project because filesystem programs need to be reliable before adding more advanced functionality.

---

# 13. Interactive Terminal UI

Phase 11 introduces an interactive terminal interface.

The UI displays:

* Current directory
* Directory contents
* Selected item
* File type
* File size
* Keyboard controls
* Command input
* Status messages

Basic navigation:

```text
↑ / ↓     Move selection
Enter     Open directory
h         Go back
:         Enter command mode
q         Quit
```

The goal of the UI is to eventually provide a terminal experience similar in spirit to applications such as `ranger` or `nnn`, while keeping the implementation educational and custom-built.

---

# Architecture

The current architecture is conceptually:

```text
                 User
                  │
                  ▼
            Terminal UI
                  │
                  ▼
           Command Parser
                  │
                  ▼
            File Manager
                  │
                  ▼
       C++17 std::filesystem
                  │
                  ▼
              Unix OS
```

### Terminal UI

Responsible for:

* Display
* Selection
* Keyboard input
* Interactive navigation
* User feedback

### Command Parser

Responsible for converting:

```text
rename "old file.txt" "new file.txt"
```

into structured command data.

### File Manager

Responsible for:

* Filesystem operations
* Navigation
* Searching
* Metadata
* Permissions
* Links
* Safety checks
* Error handling

### `std::filesystem`

Provides the main C++17 filesystem abstraction.

The project also uses POSIX functionality where appropriate for Unix-specific features such as permissions and links.

---

# Project Structure

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
│   └── TerminalUI.h
│
├── src/
│   ├── main.cpp
│   ├── CommandParser.cpp
│   ├── FileManager.cpp
│   └── TerminalUI.cpp
│
└── tests/
    └── test_cli.sh
```

---

# Building

## Requirements

* C++17-compatible compiler
* CMake
* Unix-like operating system
* Bash

The project is primarily designed for:

* macOS
* Linux

---

## Build

From the project directory:

```bash
cmake -S . -B build
```

Then:

```bash
cmake --build build
```

---

# Running

Run the executable with:

```bash
./build/filemanager
```

The interactive terminal UI will start.

---

# Running Tests

Build the project first:

```bash
cmake --build build
```

Then run:

```bash
ctest --test-dir build --output-on-failure
```

---

# Example Session

```text
CLI FILE MANAGER

Current directory:
/Users/user/projects

> [DIR]  cli-file-manager/
  [DIR]  documents/
  [FILE] README.md
  [FILE] notes.txt

↑ ↓ Navigate
Enter Open
h Back
: Command
q Quit
```

Command mode can be used with:

```text
: mkdir test
```

or:

```text
: find main.cpp
```

---

# Technologies

## Language

**C++17**

Important concepts used:

* Classes
* References
* Exceptions
* STL containers
* Strings
* Streams
* Algorithms
* `std::filesystem`
* Lambda expressions
* RAII concepts

## Filesystem

* C++17 `std::filesystem`
* Unix filesystem concepts
* POSIX permissions
* Hard links
* Symbolic links

## Build System

**CMake**

## Testing

**CTest**

## Version Control

**Git**

## Platform

Unix-oriented development with primary testing on macOS/Linux.

---

# Development Phases

The project is being developed progressively.

### Phase 0 — Setup

* Project structure
* Compiler
* CMake
* Git

### Phase 1 — Basic Navigation

* Current directory
* Directory listing
* `pwd`
* `cd`
* `back`

### Phase 2 — File Operations

* Create files
* Create directories
* Rename
* Copy
* Move
* Delete

### Phase 3 — File Metadata

* Size
* Type
* Modification time
* File information

### Phase 4 — Architecture

* Header/source separation
* Command parser
* Cleaner class design
* Separation of responsibilities

### Phase 5 — Recursive Filesystem

* Tree
* Recursive directory size

### Phase 6 — Advanced Parsing and Paths

* Quoted arguments
* Better path handling
* Path resolution

### Phase 7 — Search and Filtering

* Name search
* Extension search
* Size search
* Sorting
* Filtering

### Phase 8 — Unix Filesystem Features

* Permissions
* `chmod`
* Hard links
* Symbolic links
* Link inspection

### Phase 9 — Safety and Error Handling

* Exception handling
* Validation
* Dangerous path protection
* Recursive deletion confirmation
* Safe filesystem operations

### Phase 10 — Testing

* Integration tests
* CTest
* Automated regression testing

### Phase 11 — Interactive Terminal UI

* Interactive interface
* File selection
* Keyboard navigation
* Directory browsing
* Command mode
* Status messages

### Phase 12 — Advanced File Manager

Planned:

* Storage analyzer
* Duplicate detection
* File hashing
* Better filtering
* File statistics

### Phase 13 — Developer Intelligence

Planned:

* Project detection
* Git awareness
* CMake detection
* Python project detection
* Node.js project detection
* Dependency/project information

### Phase 14 — History and Recovery

Planned:

* Operation history
* Undo
* Snapshots
* Filesystem change tracking

### Phase 15 — Performance and Release

Planned:

* Performance optimization
* Large-directory handling
* Better terminal rendering
* Reliability improvements
* Final documentation
* Portfolio release

---

# Design Philosophy

The project follows several principles.

## Safety First

Filesystem applications can cause irreversible data loss.

Destructive operations should therefore:

1. Validate the path.
2. Validate the operation.
3. Check dangerous cases.
4. Ask for confirmation when necessary.
5. Only then perform the operation.

---

## Separation of Responsibilities

The project avoids putting everything inside `main.cpp`.

Instead:

```text
main.cpp
    ↓
TerminalUI / command interface
    ↓
CommandParser
    ↓
FileManager
    ↓
Filesystem / POSIX
```

This makes the project easier to:

* Understand
* Test
* Extend
* Debug
* Maintain

---

# Long-Term Vision

The final goal is to build more than a basic file manager.

The project is intended to become a **developer-oriented Unix file management environment**.

Future capabilities may include:

```text
Filesystem
    │
    ├── Navigation
    ├── Search
    ├── File operations
    ├── Permissions
    ├── Links
    ├── Storage analysis
    └── Duplicate detection
          │
          ▼
Developer Intelligence
    │
    ├── Git detection
    ├── Project detection
    ├── Build system detection
    ├── Dependency analysis
    └── Project statistics
          │
          ▼
Safety & Recovery
    │
    ├── History
    ├── Undo
    └── Snapshots
```

This gives the project a stronger identity than a basic CRUD-style file manager.

---

# Portfolio Description

A concise description for GitHub/LinkedIn:

> **Developer-oriented Unix CLI file manager written in C++17, featuring filesystem navigation and operations, recursive search, Unix permissions and links, defensive filesystem safety, automated integration testing, and an interactive terminal UI.**

As the later phases are completed, the project will additionally include:

* Intelligent project detection
* Git awareness
* Storage analysis
* Duplicate detection
* Operation history
* Undo
* Snapshots
* Advanced terminal UI

---

# Current Goal

**Phase 11 — Interactive Terminal UI**

The current focus is transforming the underlying command-line filesystem engine into a more usable interactive terminal application while maintaining the reliability and safety developed in previous phases.
