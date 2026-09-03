# CLI File Manager

A C++17 command-line file manager built from scratch using `std::filesystem` and Unix/POSIX filesystem features.

The project is being developed progressively to demonstrate practical skills in C++, filesystem programming, command parsing, Unix concepts, software architecture, error handling, testing, and professional software development.

---

## Features

### Navigation

* `ls`
* `ls -a`
* `pwd`
* `cd`
* `back`
* `tree`

### File Operations

* `mkdir`
* `touch`
* `rename`
* `cp`
* `mv`
* `rm`

### File Information

* `size`
* `type`
* `modified`
* `info`

### Search

* `find`
* `findext`
* `findsize`

### Sorting and Filtering

* `ls --files`
* `ls --dirs`
* `ls --name`
* `ls --size`
* `ls --name-desc`
* `ls --size-desc`

### Unix Features

* Hidden file support
* File permission inspection
* Permission modification with `chmod`
* Octal permission modes
* Hard links
* Symbolic links
* Symbolic link inspection with `readlink`
* Recursive deletion confirmation

---

## Command Reference

### Navigation

```text
ls
ls -a
pwd
cd <directory>
back
tree [path]
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

### File Information

```text
size <file>
type <path>
modified <path>
info <path>
```

### Search

```text
find <name> [path]
findext <extension> [path]
findsize <minimum_bytes> [path]
```

### Sorting and Filtering

```text
ls --files
ls --dirs
ls --name
ls --size
ls --name-desc
ls --size-desc
```

### Unix Features

```text
perm <path>
chmod <mode> <path>

link <target> <link>

ln <target> <link>
ln -s <target> <link>

readlink <path>
```

### General

```text
help
q
quit
exit
```

---

## Examples

### Create and navigate directories

```text
fm> mkdir projects
fm> cd projects
fm> pwd
fm> back
```

### Create and manipulate files

```text
fm> touch example.txt
fm> rename example.txt renamed.txt
fm> cp renamed.txt backup.txt
fm> mv backup.txt ..
```

### Work with paths containing spaces

```text
fm> mkdir "My Folder"
fm> cd "My Folder"
fm> touch "hello world.txt"
fm> info "hello world.txt"
```

### Search for files

```text
fm> find example.txt
fm> findext cpp
fm> findsize 1000
```

### View directory structure

```text
fm> tree .
```

Example:

```text
project
├── include
│   ├── FileManager.h
│   └── CommandParser.h
├── src
│   ├── FileManager.cpp
│   ├── CommandParser.cpp
│   └── main.cpp
└── README.md
```

### Work with permissions

```text
fm> perm script.sh
Permissions: rw-r--r--

fm> chmod 755 script.sh
Permissions changed successfully.

fm> perm script.sh
Permissions: rwxr-xr-x
```

Common permission modes:

```text
644 → rw-r--r--
755 → rwxr-xr-x
700 → rwx------
600 → rw-------
```

### Work with symbolic links

```text
fm> ln -s original.txt shortcut.txt
fm> readlink shortcut.txt
Symlink target: original.txt

fm> info shortcut.txt
```

### Work with hard links

```text
fm> link original.txt hardcopy.txt
```

---

## Architecture

The project follows a simple layered architecture:

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
             │        │
             ▼        ▼
      std::filesystem  POSIX
             │        │
             └────┬───┘
                  ▼
            Operating System
```

### `main.cpp`

Responsible for:

* running the main command loop
* receiving user input
* parsing commands
* validating arguments
* dispatching commands

### `CommandParser`

Responsible for:

* splitting commands into tokens
* handling quoted arguments
* supporting paths containing spaces
* separating the command from its arguments

### `FileManager`

Responsible for:

* navigation
* file operations
* directory operations
* metadata
* recursive operations
* searching
* sorting
* filtering
* permissions
* hard links
* symbolic links

---

## Project Structure

```text
cli-file-manager/
│
├── build/
│
├── docs/
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
├── tests/
│
├── CMakeLists.txt
├── README.md
└── .gitignore
```

---

## Technologies

* **C++17**
* **CMake**
* **Standard Library**
* **`std::filesystem`**
* **POSIX filesystem APIs**
* **Git**
* **GitHub**

---

## Building

Clone the repository and enter the project directory:

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

---

## Running

Run the executable with:

```bash
./build/filemanager
```

You should see:

```text
CLI File Manager
Type 'help' for commands.

fm>
```

Type:

```text
help
```

to see the available commands.

---

## C++ Concepts Demonstrated

This project demonstrates practical use of:

* Classes and objects
* Encapsulation
* Header/source separation
* Functions
* References
* `const`
* `std::string`
* `std::vector`
* `std::filesystem`
* Iterators
* Lambdas
* Algorithms
* Exception handling
* Recursion
* File streams
* Chrono/time handling
* Command parsing
* Path manipulation
* POSIX APIs
* File permissions
* Symbolic links
* Hard links

---

## Unix Concepts Demonstrated

Phase 8 introduces important Unix filesystem concepts:

### File Permissions

```text
rwxr-xr--
```

Permissions are divided into:

```text
Owner   Group   Others
rwx     r-x     r--
```

Numeric permissions use:

```text
r = 4
w = 2
x = 1
```

For example:

```text
755

7 = rwx
5 = r-x
5 = r-x
```

Therefore:

```text
755 = rwxr-xr-x
```

### Symbolic Links

A symbolic link points to another filesystem path:

```text
shortcut.txt
      │
      ▼
original.txt
```

### Hard Links

A hard link provides another directory entry referring to the same underlying file:

```text
original.txt ───┐
                │
                ▼
          underlying file
                ▲
                │
hardcopy.txt ───┘
```

---

## Safety

The `rm` command protects against accidental recursive deletion.

When deleting a directory:

```text
Warning: this will recursively delete ...
Continue? (y/n):
```

The operation only proceeds when the user enters:

```text
y
```

or:

```text
Y
```

---

## Development Phases

The project is being developed incrementally.

```text
Phase 0  → Project setup
Phase 1  → Navigation
Phase 2  → File operations
Phase 3  → File metadata
Phase 4  → Project architecture
Phase 5  → Recursive filesystem operations
Phase 6  → Advanced parsing and path handling
Phase 7  → Search, sorting and filtering
Phase 8  → Unix permissions and links
Phase 9  → Error handling and safety
Phase 10 → Testing
Phase 11 → Terminal UI
Phase 12 → Advanced file-manager features
Phase 13 → Professional features
Phase 14 → Performance and reliability
Phase 15 → Final portfolio release
```

---

## Current Status

**Phase 8 — Unix Filesystem Features**

Completed:

* Navigation
* File creation
* File deletion
* File copying
* File moving
* File renaming
* Recursive operations
* File metadata
* Directory trees
* Directory size calculation
* Search
* Sorting
* Filtering
* Quoted paths
* Hidden files
* Permission inspection
* `chmod`
* Hard links
* Symbolic links
* Symlink inspection
* Recursive deletion confirmation

---

## Future Improvements

Planned future development includes:

* More robust error handling
* Unit and integration testing
* Automated test environments
* Improved terminal interface
* Keyboard navigation
* Colors and visual feedback
* Multi-file selection
* Bulk operations
* File previews
* Trash/recycle functionality
* Undo functionality
* Bookmarks
* Command history
* Configuration files
* Performance optimization
* Continuous integration
* Professional documentation
* Final portfolio release

---

## Goal

The goal of this project is to build a progressively more capable command-line file manager while developing practical knowledge of:

**C++ → Operating Systems → Unix/Linux → Filesystems → Software Architecture → Testing → Performance → Professional Software Development**

The final version is intended to demonstrate these skills through a complete, maintainable, and well-documented C++ project.
