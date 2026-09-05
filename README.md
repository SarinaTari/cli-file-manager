# CLI File Manager

A **developer-focused, safety-oriented filesystem intelligence tool** built with **C++17** for Unix-like systems.

CLI File Manager started as a simple terminal-based filesystem utility and evolved into a modular developer tool for **navigating, manipulating, analyzing, and understanding files and software projects** directly from the command line.

It combines traditional file-manager functionality with developer-oriented features such as **project detection, Git analysis, dependency analysis, storage analysis, duplicate detection, snapshots, undo, project health checks, and intelligent project visualization**.

---

## ✨ Highlights

* 📁 Filesystem navigation and file management
* 🔎 Recursive search, filtering, and sorting
* 📊 File and directory metadata inspection
* 🔐 Unix permissions and link support
* 💾 Storage usage analysis
* 🧬 Duplicate file detection using content hashing
* 🌿 Git repository and status analysis
* 🔗 C/C++ dependency analysis
* 🧠 Project structure and technology detection
* 🕐 Operation history
* ↩️ Undo for supported operations
* 📸 Filesystem snapshots and comparisons
* 🛡️ Safe deletion and destructive-operation protection
* 🩺 Project health checks
* 💡 File explanation and project-role detection
* 🌳 Smart project trees
* 🖥️ Interactive terminal UI
* 🧪 Automated integration testing
* ⚡ Performance-conscious filesystem traversal
* 🧱 Modular C++ architecture

---

# 🎯 What Is This Project?

Most file managers answer questions such as:

```text
What files are here?
Where am I?
How do I move this file?
```

CLI File Manager aims to answer higher-level questions as well:

```text
What kind of project is this?

How is this project organized?

What does this file do?

Which files depend on this one?

How much storage is this project using?

Where are the largest files?

Are there duplicate files?

Is this a Git repository?

Is the project healthy?

What changed since my snapshot?

Can I undo this filesystem operation?

Is it safe to delete this?
```

The goal is to bridge the gap between a traditional **filesystem utility** and a **developer-oriented project intelligence tool**.

---

# 🚀 Features

## 📁 Filesystem Management

Core filesystem operations include:

* List files and directories
* Navigate between directories
* Create files
* Create directories
* Rename files and directories
* Copy files and directories
* Move files and directories
* Remove files and directories
* Recursive directory operations
* Hidden-file handling
* Path resolution
* Quoted-path support
* Protection against dangerous paths
* Protection against accidental overwrites

---

## 🔎 Search, Filtering & Sorting

The file manager supports recursive filesystem discovery and organization.

Capabilities include:

* Search by name
* Search by extension
* Search by minimum size
* Recursive search
* Directory filtering
* File filtering
* Sorting by name
* Sorting by size
* Descending sorting
* Path-aware search

Example:

```text
find-name README.md
find-ext cpp
find-size 1000
```

---

## 📊 File Information

Inspect detailed filesystem metadata with:

```text
info <path>
```

Information can include:

* File type
* File size
* Modification time
* Permissions
* Directory information
* Symbolic-link information
* Filesystem metadata

---

## 🔐 Unix Filesystem Support

The project works with important Unix filesystem concepts including:

* File permissions
* Permission modification
* Symbolic links
* Hard links
* Link detection
* Link targets
* Unix filesystem metadata

Example:

```text
chmod 755 script.sh
```

---

# 💾 Storage Intelligence

CLI File Manager can analyze how storage is being used within a directory or project.

Storage analysis includes:

* Recursive size calculation
* File size statistics
* Directory size statistics
* Largest files
* Storage summaries
* Project storage analysis

Example:

```text
storage
```

This makes the tool useful for investigating large repositories, build directories, datasets, and other storage-heavy projects.

---

# 🧬 Duplicate Detection

The duplicate detector identifies files with identical contents.

Rather than relying only on filenames, the system uses **file hashing** to compare file contents.

This allows the tool to detect cases such as:

```text
report.pdf
backup/report.pdf
old/report-copy.pdf
```

when multiple files contain exactly the same data.

Example:

```text
duplicates
```

---

# 🧠 Developer Intelligence

The project goes beyond filesystem operations by analyzing software projects.

It can detect information such as:

* Project structure
* Source directories
* Header directories
* Test directories
* Build systems
* Programming languages
* Project-related files
* Git repositories
* Dependencies

This allows the filesystem itself to become a source of **project-level information**.

---

# 🌿 Git Intelligence

Git-aware functionality provides information about repositories from inside the file manager.

Capabilities include:

* Git repository detection
* Git status
* Git-tracked file detection
* Repository information
* Integration with project analysis

Example:

```text
git
```

---

# 🔗 Dependency Analysis

The dependency analyzer provides a basic view of relationships between source files.

For C/C++ projects, it can inspect local header relationships such as:

```cpp
#include "FileManager.h"
```

and identify relationships between source and header files.

This provides a lightweight view of the project's internal dependency structure without requiring a full compiler-level dependency graph.

---

# 🕐 Operation History

Filesystem operations can be recorded in an operation history.

Supported functionality includes:

```text
history
undo
clearhistory
```

The history system provides the foundation for reversible filesystem workflows and allows users to inspect previous operations.

---

# ↩️ Undo

Supported filesystem operations can be reversed through the undo system.

Example:

```text
undo
```

Undo is designed as a safety-oriented feature rather than as a replacement for version control.

It provides an additional recovery mechanism for supported filesystem operations.

---

# 📸 Snapshots

The snapshot system captures a representation of a project's filesystem state.

Users can then compare the current state against the snapshot.

Supported functionality includes:

* Create a snapshot
* View the current snapshot
* Compare filesystem states
* Detect added entries
* Detect removed entries
* Detect changed entries
* Clear snapshots

Example workflow:

```text
snapshot
...
make changes
...
diff
```

This provides a lightweight way to answer:

> "What changed in this project?"

---

# 📊 Project Dashboard

The project dashboard provides a high-level overview of a project.

Example:

```text
project
```

The dashboard can summarize information such as:

* Number of files
* Number of directories
* Storage usage
* Project structure
* Programming languages
* Git information
* Detected project components

Instead of manually inspecting dozens of files, the dashboard provides a quick project overview from the terminal.

---

# 🩺 Project Doctor

The project health checker examines important project components and reports potential problems.

Example:

```text
doctor
```

Checks can include:

* `CMakeLists.txt`
* `src/`
* `include/`
* `tests/`
* `README.md`
* Git repository
* Git status
* Expected project structure

Results are organized into categories such as:

```text
PASS
WARNING
ERROR
```

This provides a quick sanity check before development, testing, or release.

---

# 💡 File Explainer

The file explainer helps users understand the role of a file inside a project.

Example:

```text
why src/main.cpp
```

The analyzer can identify:

* File type
* Programming language
* Project role
* Related files
* Related headers
* Git status
* Tracking state
* A short explanation of the file's likely purpose

This is particularly useful when exploring an unfamiliar codebase.

---

# 🌳 Smart Project Tree

The smart tree provides a project-oriented alternative to a traditional filesystem tree.

Example:

```text
tree --smart
```

Instead of displaying only filenames, project components can be classified by their role:

```text
src/             [SOURCE]
include/         [HEADERS]
tests/            [TESTS]
docs/             [DOCUMENTATION]
build/            [BUILD OUTPUT]
.git/             [GIT METADATA]

CMakeLists.txt    [BUILD SYSTEM]
main.cpp          [ENTRY POINT]
```

This makes the structure of software projects easier to understand at a glance.

---

# 🛡️ Safety

Filesystem manipulation can be destructive, so safety is a core design consideration.

## Dangerous Path Protection

The application protects against destructive operations targeting dangerous filesystem locations such as:

```text
.
..
```

and other paths identified as unsafe.

---

## Overwrite Protection

Operations that could unintentionally overwrite existing files or directories are validated before execution.

---

## Recursive Operation Protection

The application checks recursive operations to prevent invalid operations such as attempting to copy or move a directory into itself.

---

## Safe Delete

The safe-delete functionality provides a preview before destructive deletion.

A preview can include:

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

Deletion proceeds only after explicit confirmation.

---

# 🏗️ Architecture

The project follows a layered, modular architecture designed around **separation of concerns**.

```text
                         ┌─────────────────┐
                         │    main.cpp     │
                         └────────┬────────┘
                                  │
                                  ▼
                         ┌─────────────────┐
                         │ CommandParser   │
                         └────────┬────────┘
                                  │
                                  ▼
                         ┌─────────────────┐
                         │   FileManager   │
                         │    Facade       │
                         └────────┬────────┘
                                  │
             ┌────────────────────┼─────────────────────┐
             │                    │                     │
             ▼                    ▼                     ▼
      ┌──────────────┐    ┌───────────────┐     ┌──────────────┐
      │ Filesystem   │    │ Developer     │     │ History /    │
      │ Operations   │    │ Intelligence  │     │ Snapshots    │
      └──────┬───────┘    └───────┬───────┘     └──────────────┘
             │                    │
             ▼                    ▼
      std::filesystem        Git / Analysis
             │
             ▼
      ┌─────────────────┐
      │ Operating System│
      └─────────────────┘
```

### Main Components

| Component            | Responsibility                                     |
| -------------------- | -------------------------------------------------- |
| `main.cpp`           | Application entry point and top-level command flow |
| `CommandParser`      | Parses and validates user commands                 |
| `FileManager`        | Main application façade                            |
| `TerminalUI`         | Interactive terminal rendering and input           |
| `StorageAnalyzer`    | Storage usage analysis                             |
| `DuplicateDetector`  | Duplicate-content detection                        |
| `FileHasher`         | File content hashing                               |
| `ProjectDetector`    | Project structure detection                        |
| `GitAnalyzer`        | Git repository analysis                            |
| `DependencyAnalyzer` | Source dependency analysis                         |
| `HistoryManager`     | Operation history                                  |
| `UndoManager`        | Reversible filesystem operations                   |
| `SnapshotManager`    | Filesystem snapshots and comparisons               |
| `ProjectDashboard`   | Project overview                                   |
| `ProjectDoctor`      | Project health checks                              |
| `FileExplainer`      | File-role analysis                                 |
| `SmartTree`          | Project-oriented tree visualization                |
| `SafeDelete`         | Destructive-operation preview and confirmation     |

The architecture was intentionally refactored so that `main.cpp` does not directly coordinate every specialized subsystem.

Instead:

```text
User
 ↓
main.cpp
 ↓
CommandParser
 ↓
FileManager
 ↓
Specialized Components
```

This keeps responsibilities separated and makes the project easier to extend and maintain.

---

# 📂 Project Structure

```text
cli-file-manager/
│
├── .gitignore
├── CMakeLists.txt
├── README.md
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
└── tests/
    └── test_cli.sh
```

Generated directories such as `build/` and CTest output are intentionally excluded from the repository.

---

# 🛠️ Technologies

### Language

* **C++17**

### Build System

* **CMake**

### Filesystem

* `std::filesystem`
* POSIX/Unix filesystem APIs

### Testing

* **CTest**
* Bash integration tests

### Development Tools

* Git
* GitHub
* Bash
* Unix terminal

---

# 💻 Requirements

A Unix-like environment with:

* C++17-compatible compiler
* CMake 3.16+
* Bash
* Git

Supported/developed environments include:

* macOS
* Linux

---

# 🔨 Build

Clone the repository:

```bash
git clone https://github.com/SarinaTari/cli-file-manager.git
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

The executable will be generated at:

```text
build/filemanager
```

Run:

```bash
./build/filemanager
```

---

# 🧪 Testing

Build the project:

```bash
cmake --build build
```

Run the automated test suite:

```bash
ctest --test-dir build --output-on-failure
```

For a clean release-style build:

```bash
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

The integration tests cover functionality including:

* Navigation
* File operations
* Recursive operations
* Path handling
* File metadata
* Permissions
* Symbolic links
* Error handling
* Filesystem safety
* Storage analysis
* Duplicate detection
* Git analysis
* Dependency analysis
* Operation history
* Undo
* Snapshots
* Project dashboard
* Safe deletion
* Project health checks
* File explanation
* Smart project tree
* End-to-end workflows

---

# 🎮 Command Reference

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

## Search

```text
find-name <name>
find-ext <extension>
find-size <minimum-size>
```

## Permissions & Links

```text
chmod <mode> <path>
```

Additional commands provide support for inspecting and creating filesystem links.

## Tree & Storage

```text
tree
tree --smart
du <path>
storage
duplicates
```

## Developer Intelligence

```text
project
doctor
why <path>
git
dependencies
```

## History & Undo

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

> Command syntax may evolve as the project continues to develop.

---

# 🧪 Development Process

The project was intentionally developed incrementally.

Each phase introduced a specific group of concepts while preserving previously implemented functionality.

```text
Phase 0   → Project setup
Phase 1   → Basic navigation
Phase 2   → File operations
Phase 3   → File information and metadata
Phase 4   → Architecture refactoring
Phase 5   → Recursive filesystem operations
Phase 6   → Advanced parsing and path handling
Phase 7   → Search, sorting and filtering
Phase 8   → Unix permissions and links
Phase 9   → Error handling and filesystem safety
Phase 10  → Automated integration testing
Phase 11  → Interactive terminal UI
Phase 12  → Storage analysis and duplicate detection
Phase 13  → Developer intelligence
Phase 14  → History, undo and snapshots
Phase 15  → Developer tools and project intelligence
Phase 16  → Final cleanup, reliability, performance and release preparation
```

This phased approach allowed the project to grow from a small filesystem program into a substantially more complete systems-oriented application.

---

# ⚙️ Engineering Practices

The project emphasizes practical software-engineering principles rather than simply accumulating features.

### Separation of Concerns

Filesystem operations, command parsing, terminal rendering, project analysis, history, and safety logic are separated into dedicated components.

### RAII

C++ resource management follows RAII principles where appropriate, avoiding unnecessary manual resource ownership.

### Error Propagation

Filesystem operations use `std::error_code` where appropriate and convert failures into contextual exceptions at the application layer.

### Defensive Programming

Destructive operations validate paths, targets, and operation preconditions before modifying the filesystem.

### Performance Awareness

Filesystem traversal avoids unnecessary repeated queries where practical and caches metadata during operations such as directory listing and sorting.

### Automated Testing

Integration tests exercise complete command workflows rather than testing only isolated functions.

### Incremental Development

Functionality was introduced in controlled phases, with builds and tests performed throughout development.

---

# 🧩 What This Project Demonstrates

## C++

* C++17
* Classes and encapsulation
* Header/source separation
* STL containers and algorithms
* `std::filesystem`
* Exception handling
* `std::error_code`
* RAII
* Object-oriented design
* Modular architecture
* Resource management

## Systems Programming

* Filesystem traversal
* Paths and directories
* File metadata
* File permissions
* Symbolic links
* Hard links
* Recursive operations
* Unix filesystem semantics
* Terminal interaction
* Shell integration

## Software Engineering

* Separation of concerns
* Facade-style application architecture
* Defensive programming
* Error handling
* Integration testing
* Build systems
* Performance optimization
* Versioning
* Documentation
* Release preparation

## Developer Tooling

* Git
* GitHub
* CMake
* CTest
* Bash
* Unix terminal applications

---

# 📈 Version History

## v0.16.0

Final engineering and release preparation:

* Code cleanup
* Architecture cleanup
* Performance improvements
* Filesystem efficiency improvements
* Memory/resource safety review
* Robust error handling
* Edge-case handling
* Automated testing
* Compiler warning checks
* Release build verification
* Repository cleanup
* Documentation finalization

## v0.15.0

Developer tools and project intelligence:

* Project Dashboard
* Safe Delete
* Project Health Check
* File Explainer
* Smart Project Tree
* Developer workflow integration

## v0.14.0

Operation history and filesystem snapshots:

* Operation history
* Undo
* Filesystem snapshots
* Snapshot comparison

## v0.13.0

Developer intelligence:

* Project detection
* Git analysis
* Dependency analysis

## v0.12.0

Storage intelligence:

* Storage analysis
* Duplicate detection
* File hashing

## v0.11.0

Interactive terminal UI.

## v0.10.0

Automated integration testing.

## v0.9.0

Filesystem safety and robust error handling.

## v0.8.0

Unix permissions and filesystem link support.

## v0.7.0

Search, sorting and filtering.

## v0.6.0

Advanced command parsing and path handling.

## v0.5.0

Recursive filesystem operations.

## v0.4.0

Project architecture refactoring.

## v0.3.0

File information and metadata.

## v0.2.0

File operations.

## v0.1.0

Basic filesystem navigation.

---

# 🗺️ Future Development

Although the current release focuses on a stable developer-oriented filesystem tool, possible future work includes:

* More advanced dependency graphs
* Improved Git integration
* Persistent configuration
* File previewing
* Advanced project detection
* Interactive project analysis
* Parallel filesystem analysis
* More sophisticated storage analysis
* Improved terminal rendering
* Keyboard-driven navigation
* Cross-platform improvements
* Persistent history
* More advanced snapshots
* Plugin architecture

---

# 🎓 Educational Purpose

This project was built as a practical systems-programming and software-engineering project.

It was designed to reinforce concepts including:

```text
C++17
   ↓
STL
   ↓
Filesystem APIs
   ↓
Unix concepts
   ↓
Software architecture
   ↓
Error handling
   ↓
Testing
   ↓
Performance
   ↓
Developer tooling
```

Rather than implementing a collection of unrelated demonstrations, the project uses one continuously evolving application to apply these concepts together.

---

# 👩‍💻 Author

**Sarina Tari**

Computer Engineering student interested in:

* Systems programming
* C/C++
* Linux and Unix systems
* Software engineering
* Databases
* Developer tools

---

## 📄 License

This project is licensed under the **MIT License**.

See the [LICENSE](LICENSE) file for the complete license text.
