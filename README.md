# CLI File Manager

A developer-focused command-line file manager built in **C++17** for Unix-like systems.

The project started as a simple filesystem utility and progressively evolved into a developer-oriented tool for navigating, managing, analyzing, and understanding projects directly from the terminal.

---

## 🚀 Features

### 📁 File & Directory Management

* List files and directories
* Navigate between directories
* Create files and directories
* Remove files and directories
* Rename files and directories
* Copy files and directories
* Move files and directories
* Recursive filesystem operations
* Protection against dangerous paths
* Protection against accidental overwrites
* Safe deletion with confirmation

### 🔎 Search & Filtering

* Search for files and directories
* Filter filesystem entries
* Sort directory contents
* Search recursively
* Find files by size
* Improved path handling
* Support for quoted paths

### 📊 File Information

Display detailed information about files and directories:

* File type
* File size
* Permissions
* Modification time
* Directory information
* Symbolic link information
* Other filesystem metadata

### 🔐 Unix Filesystem Support

The application supports Unix filesystem concepts including:

* File permissions
* Permission modification
* Symbolic links
* Hard links
* Permission inspection
* Link detection
* Unix-style filesystem metadata

### 💾 Storage Analysis

Analyze how storage is being used by a project or directory.

Features include:

* Recursive storage analysis
* File size statistics
* Directory size statistics
* Largest files
* Storage summaries
* Duplicate file detection

### 🧬 Duplicate Detection

Find files that contain identical data.

The duplicate detector uses file hashing to identify files with the same contents rather than relying only on filenames.

### 🧠 Developer Intelligence

The file manager can inspect a project and identify useful development information.

It can detect:

* Git repositories
* Git status
* Project structure
* Programming languages
* Source directories
* Header directories
* Test directories
* Build systems
* Dependencies
* Project-related files

### 🌿 Git Intelligence

Git-aware functionality includes:

* Git repository detection
* Git status
* Git-tracked file detection
* Project Git information

### 🔗 Dependency Analysis

The application can analyze source files and detect relationships such as local C/C++ header includes.

This provides a basic view of how source files depend on one another.

### 🕐 Operation History

Filesystem operations can be recorded in an operation history.

Supported functionality includes:

* View operation history
* Track filesystem operations
* Undo supported operations
* Clear history

### ↩️ Undo

The project includes an undo system for supported filesystem operations.

This provides a foundation for safer filesystem manipulation and reversible developer workflows.

### 📸 Snapshots

Create a snapshot of a project's filesystem state and compare the current state against it.

Supported functionality:

* Create snapshots
* View snapshots
* Compare current filesystem state with a snapshot
* Detect added entries
* Detect removed entries
* Detect changed entries

### 🛠️ Project Dashboard

Use:

```text
project
```

to get a high-level overview of a project.

The dashboard can display information such as:

* Total files
* Total directories
* Storage usage
* Git information
* Programming languages
* Project structure

### 🩺 Project Health Check

Use:

```text
doctor
```

to inspect the health of a project.

The health checker verifies important project components such as:

* `CMakeLists.txt`
* `src/`
* `include/`
* `tests/`
* `README.md`
* Git repository
* Git status

It reports passed checks, warnings, errors, and an overall summary.

### 💡 File Explainer

Use:

```text
why <path>
```

to understand the role of a file inside a project.

For example:

```text
why src/main.cpp
```

The file explainer can identify:

* File type
* Programming language
* Project role
* Related files
* Related headers
* Git status
* Whether the file is tracked
* A short explanation of the file's purpose

### 🌳 Smart Project Tree

Use:

```text
tree --smart
```

to display a project-oriented filesystem tree.

Instead of showing only filenames, the smart tree classifies project components such as:

```text
src/          [SOURCE]
include/      [HEADERS]
tests/        [TESTS]
docs/         [DOCUMENTATION]
build/        [BUILD OUTPUT]
.git/         [GIT METADATA]
CMakeLists.txt [BUILD SYSTEM]
main.cpp      [ENTRY POINT]
```

This makes large development projects easier to understand from the terminal.

---

# 🏗️ Architecture

The project follows a layered architecture.

```text
                    ┌──────────────────┐
                    │     main.cpp     │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │  CommandParser   │
                    └────────┬─────────┘
                             │
                             ▼
                    ┌──────────────────┐
                    │   FileManager    │
                    └────────┬─────────┘
                             │
          ┌──────────────────┼────────────────────┐
          │                  │                    │
          ▼                  ▼                    ▼
   ┌─────────────┐   ┌──────────────┐   ┌─────────────────┐
   │ Filesystem  │   │ Developer    │   │ Terminal UI     │
   │ Operations  │   │ Intelligence │   │                 │
   └─────────────┘   └──────────────┘   └─────────────────┘
          │                  │
          ▼                  ▼
   std::filesystem       Git / Analysis
          │
          ▼
     Operating System
```

The main application coordinates commands while specialized components handle individual responsibilities.

---

# 📂 Project Structure

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
│   ├── StorageAnalyzer.h
│   ├── ProjectDetector.h
│   ├── GitAnalyzer.h
│   ├── DependencyAnalyzer.h
│   ├── HistoryManager.h
│   ├── UndoManager.h
│   ├── SnapshotManager.h
│   ├── ProjectDashboard.h
│   ├── SafeDelete.h
│   ├── ProjectDoctor.h
│   ├── FileExplainer.h
│   └── SmartTree.h
│
├── src/
│   ├── main.cpp
│   ├── FileManager.cpp
│   ├── CommandParser.cpp
│   ├── TerminalUI.cpp
│   ├── FileHasher.cpp
│   ├── DuplicateDetector.cpp
│   ├── StorageAnalyzer.cpp
│   ├── ProjectDetector.cpp
│   ├── GitAnalyzer.cpp
│   ├── DependencyAnalyzer.cpp
│   ├── HistoryManager.cpp
│   ├── UndoManager.cpp
│   ├── SnapshotManager.cpp
│   ├── ProjectDashboard.cpp
│   ├── SafeDelete.cpp
│   ├── ProjectDoctor.cpp
│   ├── FileExplainer.cpp
│   └── SmartTree.cpp
│
├── tests/
│   ├── test_cli.sh
│   └── ...
│
└── build/
```

---

# 🛠️ Technologies

The project is built using:

* **C++17**
* **CMake**
* **std::filesystem**
* **POSIX / Unix filesystem concepts**
* **Git**
* **Bash**
* **CTest**
* **Shell scripting**

---

# 💻 Requirements

You need:

* A C++17-compatible compiler
* CMake 3.16+
* Bash
* Git

The project is primarily designed for Unix-like environments such as:

* macOS
* Linux

---

# 🔨 Building

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

Run it:

```bash
./build/filemanager
```

---

# 🧪 Testing

The project includes an automated integration test suite.

Build the project first:

```bash
cmake --build build
```

Then run:

```bash
ctest --test-dir build --output-on-failure
```

The test suite covers functionality from the different development phases, including:

* Navigation
* File operations
* Recursive operations
* Path handling
* File information
* Permissions
* Symbolic links
* Error handling
* Safe filesystem operations
* Storage analysis
* Duplicate detection
* Git intelligence
* Dependency analysis
* History
* Undo
* Snapshots
* Project dashboard
* Safe delete
* Project health checks
* File explanation
* Smart project tree
* Integration workflows

---

# 🎮 Commands

The project supports a growing set of commands.

## Navigation

```text
pwd
cd <path>
back
```

## File Operations

```text
mkdir <name>
touch <name>
rm <path>
rename <old> <new>
cp <source> <destination>
mv <source> <destination>
```

## Information

```text
info <path>
```

## Search & Analysis

```text
find <name>
findsize <size>
```

## Permissions

```text
chmod <mode> <path>
```

## Storage

```text
storage
duplicates
```

## Developer Intelligence

```text
project
doctor
why <path>
tree --smart
```

## History

```text
history
undo
clearhistory
```

## Snapshots

```text
snapshot
showsnapshot
diff
clearsnapshot
```

## Exit

```text
q
quit
exit
```

---

# 🛡️ Safety

Filesystem tools can be dangerous, so this project includes several safety mechanisms.

### Dangerous path protection

The application refuses dangerous operations such as attempting to delete:

```text
.
..
```

### Overwrite protection

Operations that could unintentionally overwrite existing data are checked.

### Recursive operation safety

Recursive operations are handled carefully to avoid operations such as copying or moving a directory into itself.

### Safe Delete

The `safe rm` command displays a preview before deleting:

```text
========================================
             SAFE DELETE
========================================

Target: ...
Type: Directory
Files: ...
Directories: ...
Total size: ...

WARNING: This operation cannot be undone.

Continue? [y/N]:
```

Only explicit `y` or `Y` confirmation proceeds with deletion.

---

# 🧪 Development Approach

This project was developed incrementally rather than being written all at once.

The development process was divided into phases.

```text
Phase 0   → Project setup
Phase 1   → Basic navigation
Phase 2   → File operations
Phase 3   → File information
Phase 4   → Architecture
Phase 5   → Recursive filesystem operations
Phase 6   → Advanced parsing and path handling
Phase 7   → Search, sorting and filtering
Phase 8   → Unix permissions and links
Phase 9   → Error handling and filesystem safety
Phase 10  → Automated testing
Phase 11  → Interactive terminal UI
Phase 12  → Storage analysis and duplicate detection
Phase 13  → Developer intelligence
Phase 14  → History, undo and snapshots
Phase 15  → Developer tools and project intelligence
```

Each phase introduced new functionality while keeping previous functionality working.

---

# 📈 Current Version

```text
v0.15.0
```

Phase 15 focuses on turning the application into a more developer-oriented filesystem tool.

Major additions in this phase:

* Project Dashboard
* Safe Delete
* Project Health Check
* File Explainer
* Smart Project Tree
* Integrated developer workflows
* Expanded integration testing

---

# 🎯 Project Goals

The long-term goal is to build more than a basic file manager.

The project aims to become a:

> **Developer-focused, safety-oriented filesystem intelligence tool.**

Instead of simply answering:

```text
"What files are here?"
```

the application should eventually help answer:

```text
"What is this project?"
"What does this file do?"
"How is the project organized?"
"Which files depend on each other?"
"Where is the storage being used?"
"What changed?"
"Is the project healthy?"
"What would happen if I delete this?"
"Can I safely undo this operation?"
```

---

# 📚 What This Project Demonstrates

This project demonstrates practical knowledge of:

### C++

* Modern C++17
* Classes and encapsulation
* Header/source separation
* STL
* `std::filesystem`
* Error handling
* RAII-oriented resource management
* Object-oriented design
* Modular architecture

### Operating Systems / Linux

* Filesystems
* Paths
* File permissions
* Symbolic links
* Hard links
* Unix filesystem behavior
* Processes and command execution
* Shell interaction

### Software Engineering

* Modular architecture
* Separation of concerns
* Error handling
* Defensive programming
* Testing
* Integration testing
* Build systems
* Versioning
* Documentation

### Development Tools

* Git
* GitHub
* CMake
* Bash
* CTest
* Terminal applications

---

# 🗺️ Future Development

Potential future improvements include:

* Performance optimization
* Better terminal rendering
* Keyboard-driven navigation
* Improved search
* More advanced dependency analysis
* Persistent operation history
* More powerful snapshot management
* Configuration files
* Plugin architecture
* Parallel filesystem analysis
* Improved cross-platform support
* More comprehensive filesystem metadata
* Advanced project detection
* Better Git integration
* File previewing
* Interactive project analysis

---

# 📜 Version History

### v0.15.0

Developer tools and project intelligence:

* Project Dashboard
* Safe Delete
* Project Health Check
* File Explainer
* Smart Project Tree
* Integration testing

### v0.14.0

Operation history and filesystem snapshots:

* Operation history
* Undo
* Snapshots
* Snapshot comparison

### v0.13.0

Developer intelligence:

* Project detection
* Git analysis
* Dependency analysis

### v0.12.0

Storage intelligence:

* Storage analysis
* Duplicate detection
* File hashing

### v0.11.0

Interactive terminal UI.

### v0.10.0

Automated integration testing.

### v0.9.0

Filesystem safety and robust error handling.

### v0.8.0

Unix permissions and link support.

### v0.7.0

Search, sorting and filtering.

### v0.6.0

Advanced command parsing and path handling.

### v0.5.0

Recursive filesystem operations.

### v0.4.0

Project architecture refactoring.

### v0.3.0

File information and metadata.

### v0.2.0

File operations.

### v0.1.0

Basic filesystem navigation.

---

# 👩‍💻 Author

**Sarina Tari**

Computer Engineering student interested in:

* Systems programming
* C/C++
* Linux
* Software engineering
* Databases
* Developer tools

---

# 📄 License

This project is currently intended as a personal educational and portfolio project.
