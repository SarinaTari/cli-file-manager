# CLI File Manager

A command-line file manager written in C++ using the C++17 standard library and `std::filesystem`.

The project is being developed progressively to practice C++, filesystem operations, software architecture, CMake, Linux/Unix concepts, Git, and GitHub.

---

## Features

### Navigation

* `ls` — List directory contents
* `pwd` — Show the current working directory
* `cd <directory>` — Change directory
* `back` — Move to the parent directory

### File and Directory Operations

* `mkdir <name>` — Create a directory
* `touch <name>` — Create an empty file
* `rename <old> <new>` — Rename a file or directory
* `cp <source> <destination>` — Copy a file
* `mv <source> <destination>` — Move a file or directory
* `rm <name>` — Remove a file or empty directory

### File Information

* `size <file>` — Display the size of a file
* `type <name>` — Display the type of an item
* `modified <name>` — Display the last modification time
* `info <name>` — Display detailed information about an item

### General Commands

* `help` — Display the available commands
* `q` — Exit the file manager

---

## Example

After launching the program:

```text
CLI File Manager
Type 'help' for available commands.

> pwd
/private/tmp

> mkdir projects
Directory created: projects

> cd projects

> touch hello.txt
File created: hello.txt

> ls
hello.txt  [0 bytes]

> info hello.txt
Name: hello.txt
Path: "/private/tmp/projects/hello.txt"
Type: regular file
Size: 0 bytes
Last modified: 2026-09-03 11:30:25

> rename hello.txt hello2.txt
Renamed hello.txt -> hello2.txt

> q
Goodbye!
```

---

## Architecture

The project uses a layered structure to separate responsibilities:

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
             std::filesystem
                     │
                     ▼
              Operating System
```

### `main.cpp`

Responsible for:

* Running the main command loop
* Reading user input
* Dispatching commands
* Validating command arguments
* Displaying help information

### `CommandParser`

Responsible for:

* Parsing raw user input
* Separating the command from its arguments
* Creating a structured `Command` object

For example:

```text
rename old.txt new.txt
```

becomes:

```text
action:
rename

arguments:
old.txt
new.txt
```

### `FileManager`

Responsible for filesystem operations, including:

* Directory navigation
* Creating files
* Creating directories
* Renaming
* Copying
* Moving
* Removing
* File size information
* File type information
* Modification timestamps
* Detailed file information

---

## Project Structure

```text
cli-file-manager/
│
├── include/
│   ├── FileManager.h
│   └── CommandParser.h
│
├── src/
│   ├── main.cpp
│   ├── FileManager.cpp
│   └── CommandParser.cpp
│
├── tests/
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```

### `include/`

Contains header files containing class and function declarations.

### `src/`

Contains the C++ source files containing the implementations.

### `tests/`

Reserved for automated tests that will be added in a later phase.

### `build/`

The CMake build directory is generated locally and is intentionally excluded from Git.

---

## Technologies

* **C++**
* **C++17**
* **CMake**
* **C++ Standard Library**
* **`std::filesystem`**
* **Git**
* **GitHub**
* **Unix/macOS filesystem concepts**

---

## Building

### Requirements

You need:

* A C++17-compatible compiler
* CMake 3.16 or newer
* Git

### Configure the project

From the project root:

```bash
cmake -S . -B build
```

### Build

```bash
cmake --build build
```

### Run

```bash
./build/filemanager
```

---

## Available Commands

| Command                     | Description                    |
| --------------------------- | ------------------------------ |
| `ls`                        | List directory contents        |
| `pwd`                       | Show current directory         |
| `cd <dir>`                  | Change directory               |
| `back`                      | Go to parent directory         |
| `mkdir <name>`              | Create directory               |
| `touch <name>`              | Create empty file              |
| `rename <old> <new>`        | Rename item                    |
| `cp <source> <destination>` | Copy file                      |
| `mv <source> <destination>` | Move item                      |
| `rm <name>`                 | Remove file or empty directory |
| `size <file>`               | Show file size                 |
| `type <name>`               | Show item type                 |
| `modified <name>`           | Show modification time         |
| `info <name>`               | Show detailed information      |
| `help`                      | Show available commands        |
| `q`                         | Exit the program               |

---

## Error Handling

The file manager performs basic validation and reports filesystem errors to the user.

Examples include:

* Attempting to enter a directory that does not exist
* Attempting to enter a regular file
* Attempting to create an item that already exists
* Attempting to copy a nonexistent file
* Attempting to remove a nonexistent item
* Attempting to remove a non-empty directory
* Providing an incorrect number of command arguments
* Entering an unknown command

Example:

```text
> cd doesnotexist
Error: directory does not exist.
```

---

## Development Phases

### Phase 0 — Project Setup

* Created the CMake project
* Configured the C++17 build environment
* Created the initial project structure
* Set up Git and GitHub

### Phase 1 — Basic Navigation

Implemented:

* `ls`
* `pwd`
* `cd`
* `back`
* `help`
* `q`

Introduced:

* `std::filesystem`
* Filesystem paths
* Directory iteration
* Basic command parsing

### Phase 2 — File Operations

Implemented:

* `mkdir`
* `touch`
* `rename`
* `cp`
* `mv`
* `rm`

Improved:

* Error handling
* Command validation
* Filesystem manipulation

### Phase 3 — File Information and Metadata

Implemented:

* File sizes
* File types
* Modification timestamps
* Detailed file information

Introduced additional filesystem functionality such as:

* `std::filesystem::file_size`
* `std::filesystem::last_write_time`
* `std::filesystem::is_regular_file`
* `std::filesystem::is_directory`
* `std::filesystem::is_symlink`

### Phase 4 — Refactoring and Architecture

Refactored the original single-file implementation into separate components.

Introduced:

* `FileManager` class
* `CommandParser` class
* Header/source separation
* `include/` directory
* Multi-file CMake configuration
* Separation of concerns
* Improved project organization

---

## Design Principles

The project is being developed with several software engineering principles in mind.

### Separation of Concerns

Different parts of the program have different responsibilities.

```text
main.cpp
    ↓
CommandParser
    ↓
FileManager
    ↓
std::filesystem
```

### Encapsulation

Filesystem functionality is encapsulated inside the `FileManager` class rather than being scattered throughout `main.cpp`.

### Interface and Implementation Separation

Declarations are stored in:

```text
include/
```

while implementations are stored in:

```text
src/
```

For example:

```text
FileManager.h
      │
      │ declaration
      ▼
FileManager.cpp
      │
      │ implementation
      ▼
std::filesystem
```

---

## Future Development

Planned features include:

* Recursive directory operations
* Improved command parsing
* Support for paths containing spaces
* Command aliases
* File searching
* Sorting directory contents
* Filtering files
* File previews
* Permissions information
* Symbolic link handling
* Interactive file selection
* Configuration files
* Automated unit tests
* Cross-platform improvements
* Improved terminal interface
* More advanced error handling

---

## Learning Goals

This project is being developed as a practical way to strengthen knowledge of:

* C++
* Object-oriented programming
* C++ standard library
* Filesystems
* Operating system concepts
* Unix/macOS command-line environments
* CMake
* Software architecture
* Error handling
* Git
* GitHub
* Testing
* Project organization

---

## Project Status

**Current Phase: Phase 4 — Refactoring and Architecture**

The project is actively being developed and additional functionality will be added in future phases.
