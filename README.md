# CLI File Manager

A developer-oriented Unix CLI file manager written in C++17.

The project is being developed progressively from a simple command-line filesystem navigator into a robust, safe, intelligent developer-focused file management tool.

## Features

### Navigation

* Print current directory
* Change directories
* Move to parent directory
* Resolve relative and absolute paths

### File Operations

* Create files
* Create directories
* Rename files and directories
* Copy files and directories
* Move files and directories
* Delete files and directories

### File Information

* File size
* File type
* Modification time
* Complete file information
* Unix permissions
* Symbolic-link targets

### Recursive Operations

* Recursive directory tree
* Recursive directory size calculation
* Recursive file search

### Search

* Search by name
* Search by extension
* Search by minimum file size

### Unix Filesystem Features

* Read Unix permissions
* Change permissions with `chmod`
* Create hard links
* Create symbolic links
* Read symbolic-link targets

### Safety

* Prevent accidental overwrites
* Confirm recursive directory deletion
* Prevent deletion of the current directory
* Prevent deletion of dangerous paths
* Prevent copying directories into themselves
* Prevent moving directories into themselves
* Validate command arguments
* Detect unmatched quotes
* Gracefully handle filesystem errors

### Testing

* Automated integration tests
* CTest integration
* Regression testing
* Filesystem behavior testing
* Safety testing
* Error-handling testing
* Temporary test environments

---

# Command Reference

## Navigation

```text
pwd
cd <directory>
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

## File Operations

```text
mkdir <name>
touch <name>
rename <old> <new>
cp <source> <destination>
mv <source> <destination>
rm <name>
```

## File Information

```text
size <file>
type <file>
modified <file>
info <file>
```

## Recursive Operations

```text
tree
tree <path>
du <path>
```

## Search

```text
find <name>
find <name> <path>

findext <extension>
findext <extension> <path>

findsize <minimum_bytes>
findsize <minimum_bytes> <path>
```

## Permissions

```text
perm <path>
chmod <mode> <path>
```

Example:

```text
chmod 755 script.sh
```

## Links

Create a hard link:

```text
ln <target> <link>
```

Create a symbolic link:

```text
ln -s <target> <link>
```

Read a symbolic link:

```text
readlink <path>
```

## Help and Exit

```text
help
q
quit
exit
```

---

# Architecture

The application follows a layered structure:

```text
User
 │
 ▼
main.cpp
 │
 ▼
CommandParser
 │
 ▼
FileManager
 │
 ▼
C++ std::filesystem / POSIX APIs
 │
 ▼
Operating System
```

### `main.cpp`

Responsible for:

* Reading user input
* Calling the command parser
* Validating command arguments
* Dispatching commands
* Handling exceptions
* Running the main application loop

### `CommandParser`

Responsible for:

* Tokenizing command input
* Handling quoted arguments
* Separating commands from arguments
* Detecting unmatched quotes

### `FileManager`

Responsible for:

* Directory navigation
* File operations
* Metadata
* Recursive operations
* Searching
* Permissions
* Links
* Safety validation

---

# Error Handling

The application uses centralized exception handling.

The general flow is:

```text
User Input
    ↓
Command Parser
    ↓
Argument Validation
    ↓
FileManager
    ↓
Filesystem Operation
    ↓
Exception?
   / \
 Yes  No
  ↓    ↓
Error  Success
Message
```

Errors should not terminate the entire application unnecessarily.

For example:

```text
> info missing.txt

Error: Path does not exist: missing.txt

>
```

The program remains running.

---

# Testing

Phase 10 introduces automated integration testing.

The tests execute the real application against a temporary filesystem.

The testing flow is:

```text
Test Script
     ↓
CLI Application
     ↓
CommandParser
     ↓
FileManager
     ↓
Temporary Filesystem
```

Tests cover:

* Navigation
* File creation
* Directory creation
* Quoted paths
* Rename
* Copy
* Move
* File information
* Search
* Permissions
* `chmod`
* Hard links
* Symbolic links
* `tree`
* `du`
* Invalid arguments
* Missing files
* Overwrite protection
* Invalid permissions
* Invalid search sizes
* Unmatched quotes
* Dangerous deletion
* Recursive deletion
* Copying directories into themselves
* Moving directories into themselves

---

# Running Tests

Build the project:

```bash
cmake -S . -B build
cmake --build build
```

Run all tests:

```bash
ctest --test-dir build --output-on-failure
```

Or run the integration test directly:

```bash
./tests/test_cli.sh ./build/filemanager
```

A successful test run should end with:

```text
Passed: ...
Failed: 0

ALL TESTS PASSED
```

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
│   └── FileManager.h
│
├── src/
│   ├── main.cpp
│   ├── FileManager.cpp
│   └── CommandParser.cpp
│
├── tests/
│   └── test_cli.sh
│
└── build/
```

---

# Technologies

* C++17
* C++ Standard Library
* `std::filesystem`
* POSIX filesystem APIs
* CMake
* Bash
* CTest
* Git
* GitHub
* Unix/macOS/Linux concepts

---

# Building

Clone the repository:

```bash
git clone https://github.com/SarinaTari/cli-file-manager.git
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

Run tests:

```bash
ctest --test-dir build --output-on-failure
```

---

# Development Phases

```text
Phase 0  → Project Setup
Phase 1  → Basic Navigation
Phase 2  → File Operations
Phase 3  → File Metadata
Phase 4  → Architecture & Refactoring
Phase 5  → Recursive Filesystem Operations
Phase 6  → Advanced Parsing & Path Handling
Phase 7  → Search, Sorting & Filtering
Phase 8  → Unix Permissions & Links
Phase 9  → Robust Error Handling & Safety
Phase 10 → Testing & Quality
Phase 11 → Interactive Terminal UI
Phase 12 → Advanced File Manager Features
Phase 13 → Developer & Project Intelligence
Phase 14 → History, Undo & Snapshots
Phase 15 → Performance, Reliability & Portfolio Release
```

---

# Phase 10 Concepts

This phase focuses on:

* Automated testing
* Integration testing
* Regression testing
* Test isolation
* Temporary test environments
* CTest
* Bash scripting
* Assertions
* Exit codes
* Continuous validation
* Software reliability

---

# Design Philosophy

The project is intentionally developed incrementally.

Each phase introduces a new engineering concept while preserving the functionality of previous phases.

The goal is not simply to make a program that works once.

The goal is to make a program that:

* Works correctly
* Handles errors
* Protects the user
* Can be tested automatically
* Can be extended safely
* Has clear architecture
* Demonstrates real systems programming skills

---

# Long-Term Goal

The final project is intended to become a developer-oriented Unix file manager rather than a generic file browser.

Planned advanced capabilities include:

* Intelligent project detection
* Git awareness
* Storage analysis
* Duplicate-file detection
* Developer project analysis
* C++ include/dependency analysis
* Safe deletion workflows
* Operation history
* Undo
* Filesystem snapshots
* Snapshot comparison
* Interactive terminal UI
* Performance optimization

---

# Project Vision

The final goal is to be able to describe the project as:

> I built a Unix-oriented developer file manager in C++17 with recursive filesystem operations, permissions and links, intelligent project detection, Git awareness, duplicate detection, safe deletion, operation history, undo, snapshots, and an interactive terminal UI.

---

# Current Status

**Phase 10 / 15 — Testing & Quality**

The project now includes automated integration testing and regression testing through CTest.

The next major stage is **Phase 11 — Interactive Terminal UI**, where the project will evolve from a command-driven file manager into a more powerful interactive terminal application.
