# CLI File Manager

A developer-oriented Unix CLI file manager built in **C++17**.

This project started as a simple command-line file manager and was progressively developed into a more capable filesystem tool with navigation, file operations, metadata inspection, recursive analysis, duplicate detection, project detection, Git awareness, dependency analysis, operation history, undo, snapshots, and an interactive terminal interface.

---

## 🚀 Features

### 📁 File & Directory Management

* Navigate between directories
* Create files
* Create directories
* Rename files and directories
* Move files and directories
* Copy files and directories
* Delete files and directories
* Create symbolic links
* Create hard links

### 🧭 Navigation

* Show current working directory
* List directory contents
* Change directories
* Navigate to parent directories
* Handle absolute and relative paths
* Support paths containing spaces

### 📊 File Metadata

Inspect filesystem information including:

* File size
* File type
* Permissions
* Last modification time
* Directory status
* Symbolic-link information

### 🌳 Recursive Filesystem Operations

The file manager can work recursively with directory trees.

Examples include:

* Recursive listing
* Recursive deletion
* Recursive copying
* Directory statistics
* Storage analysis

### 🔎 Search, Sorting & Filtering

Search and organize filesystem entries using:

* Name-based searching
* Recursive searching
* File-type filtering
* Size filtering
* Sorting
* Directory/file filtering

### 💾 Storage Analyzer

Analyze a directory and determine:

* Number of files
* Number of directories
* Total storage usage
* Largest files
* Storage distribution

### 🧬 Duplicate Detection

Detect duplicate files by:

1. Grouping files by size
2. Hashing candidate files
3. Comparing file contents

This reduces unnecessary comparisons and makes duplicate detection more efficient.

### 🧠 Developer Project Detection

Automatically identify common project types.

Supported project indicators include:

* C/C++ + CMake
* C/C++ + Make
* Python
* Rust
* Java/Maven
* Java/Gradle
* Node.js

The tool also provides basic project statistics and detected project indicators.

### 🔧 Git Awareness

For Git repositories, the application can inspect:

* Whether the directory is inside a Git repository
* Repository root
* Current branch
* Latest commit
* Staged files
* Modified files
* Untracked files

### 🔗 C/C++ Dependency Analysis

Analyze C/C++ source files and inspect:

* `#include` relationships
* Included headers
* Number of source files
* Include frequencies

This provides a basic view of project dependencies directly from the terminal.

### 🕘 Operation History

The application records supported filesystem operations during a session.

History entries contain:

* Operation type
* Source path
* Destination path when applicable

Example:

```text
Operation history:

1. create_file /path/to/file.txt
2. create_directory /path/to/project
3. rename /path/to/old.txt -> /path/to/new.txt
4. move /path/to/source.txt -> /path/to/destination.txt
```

### ↩️ Undo

Supported operations can be undone safely.

Currently supported:

* File creation
* Directory creation
* Rename
* Move

The undo system uses the paths stored when the operation occurred rather than relying on the user's current directory.

For safety:

* Existing files are not blindly overwritten
* Non-empty directories are not recursively deleted during undo
* Unsupported operations are not incorrectly marked as undoable
* Failed undo operations remain in history

### 📸 Snapshots

Create an in-memory snapshot of a directory tree.

Snapshots can be used to detect:

* Added files
* Removed files
* Changed file sizes

Example:

```text
Snapshot:

/project
/project/main.cpp
/project/include
/project/include/FileManager.h
```

After changing the directory:

```text
Added:
    /project/new_file.cpp

Removed:
    /project/old_file.cpp
```

> Snapshot comparison currently uses file metadata and size rather than full content hashing.

### 🖥️ Interactive Terminal UI

The project includes an interactive terminal interface that allows commands to be entered continuously without restarting the application.

Example:

```text
Current directory: "/Users/user/project"

> ls
> cd src
> info main.cpp
> touch test.cpp
> history
> undo
> q
```

---

# 🏗️ Architecture

The project follows a layered structure:

```text
                User
                  │
                  ▼
             ┌─────────┐
             │  main   │
             └────┬────┘
                  │
                  ▼
          ┌───────────────┐
          │ CommandParser │
          └───────┬───────┘
                  │
                  ▼
          ┌───────────────┐
          │  FileManager  │
          └───────┬───────┘
                  │
        ┌─────────┼──────────┐
        ▼         ▼          ▼
   filesystem    POSIX    Specialized
                          analyzers
                              │
        ┌─────────────────────┼────────────────────┐
        ▼          ▼          ▼          ▼         ▼
   Duplicate   Storage    Project      Git    Dependency
   Detector    Analyzer   Detector   Analyzer   Analyzer
```

The main idea is to separate:

* Command parsing
* Filesystem operations
* Analysis functionality
* History/undo
* User interface

This makes the project easier to extend and maintain.

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
│   │
│   ├── FileHasher.h
│   ├── DuplicateDetector.h
│   ├── StorageAnalyzer.h
│   │
│   ├── ProjectDetector.h
│   ├── GitAnalyzer.h
│   ├── DependencyAnalyzer.h
│   │
│   ├── HistoryManager.h
│   ├── UndoManager.h
│   └── SnapshotManager.h
│
├── src/
│   ├── main.cpp
│   ├── FileManager.cpp
│   ├── CommandParser.cpp
│   ├── TerminalUI.cpp
│   │
│   ├── FileHasher.cpp
│   ├── DuplicateDetector.cpp
│   ├── StorageAnalyzer.cpp
│   │
│   ├── ProjectDetector.cpp
│   ├── GitAnalyzer.cpp
│   ├── DependencyAnalyzer.cpp
│   │
│   ├── HistoryManager.cpp
│   ├── UndoManager.cpp
│   └── SnapshotManager.cpp
│
└── tests/
    └── test_cli.sh
```

---

# 🛠️ Technologies

* **C++17**
* **CMake**
* **C++ `<filesystem>`**
* **POSIX APIs**
* **Git**
* **Bash**
* **CTest**
* **Unix/Linux/macOS concepts**

---

# 📋 Requirements

You need:

* A C++17-compatible compiler
* CMake 3.16+
* Git
* Bash

Recommended:

```text
C++17
CMake
Clang or GCC
Unix-like operating system
```

The project is primarily designed around Unix-like environments such as:

* macOS
* Linux

---

# ⚙️ Building

Clone the repository:

```bash
git clone <YOUR_REPOSITORY_URL>
cd cli-file-manager
```

Create the build directory:

```bash
mkdir build
cd build
```

Configure the project:

```bash
cmake ..
```

Build:

```bash
cmake --build .
```

The executable will be created at:

```text
build/filemanager
```

Run it:

```bash
./filemanager
```

---

# 🧪 Running Tests

The project includes automated integration tests using **CTest**.

From the build directory:

```bash
ctest --output-on-failure
```

The tests cover functionality from the different development phases, including:

* Navigation
* File creation
* Directory creation
* Rename
* Move
* Copy
* Delete
* Links
* Metadata
* Recursive operations
* Search
* Sorting
* Permissions
* Error handling
* Duplicate detection
* Storage analysis
* Project detection
* Git analysis
* Dependency analysis
* History
* Undo
* Snapshots
* Interactive UI

---

# 💻 Example Usage

Start the application:

```bash
./filemanager
```

Example session:

```text
Current directory: "/Users/user/projects"

> ls

> mkdir my-project

> cd my-project

> touch main.cpp

> info main.cpp

> history

> snapshot

> touch test.cpp

> diff

> undo

> q
```

---

# 🧩 Development Phases

The project was developed incrementally.

### Phase 0 — Setup

* Project initialization
* CMake
* Compiler configuration
* Git repository

### Phase 1 — Navigation

* Current directory
* Directory listing
* `cd`
* Parent directory

### Phase 2 — File Operations

* Create
* Rename
* Move
* Copy
* Delete

### Phase 3 — Metadata

* File information
* Size
* Type
* Permissions
* Modification time

### Phase 4 — Architecture

* Command parser
* File manager abstraction
* Cleaner separation of responsibilities

### Phase 5 — Recursive Filesystem

* Recursive traversal
* Recursive operations
* Directory trees

### Phase 6 — Path & Command Parsing

* Absolute paths
* Relative paths
* Quoted arguments
* Paths containing spaces

### Phase 7 — Search & Organization

* Search
* Filtering
* Sorting

### Phase 8 — Unix Filesystem Features

* Permissions
* Symbolic links
* Hard links

### Phase 9 — Reliability & Safety

* Error handling
* Validation
* Safer destructive operations

### Phase 10 — Testing

* Bash integration tests
* CTest integration
* Automated regression testing

### Phase 11 — Terminal UI

* Interactive command loop
* Continuous sessions
* Improved terminal experience

### Phase 12 — Advanced File Intelligence

* File hashing
* Duplicate detection
* Storage analysis

### Phase 13 — Developer Intelligence

* Project detection
* Git awareness
* C/C++ dependency analysis

### Phase 14 — History & Recovery

* Operation history
* Undo
* Snapshots
* Snapshot diff

### Phase 15 — Final Release

Planned improvements:

* Performance optimization
* Reliability improvements
* Code cleanup
* Documentation
* Final testing
* Portfolio preparation

---

# 🔐 Safety Philosophy

Filesystem applications can easily cause destructive operations.

This project therefore emphasizes safe behavior.

Examples:

* Validate paths before operations
* Avoid accidental overwrites
* Ask for confirmation for destructive operations
* Do not use recursive deletion when simple deletion is sufficient
* Verify filesystem state before undoing operations
* Preserve failed operations in history
* Never assume the user's current directory is the directory where an operation occurred

The goal is to make the tool useful without making filesystem mistakes unnecessarily dangerous.

---

# 🎯 Design Goals

The project is designed around five main goals:

### 1. Learn Systems Programming

Use C++ to interact directly with:

* Filesystems
* Paths
* Permissions
* Processes
* Unix concepts

### 2. Build Real Software

Instead of isolated exercises, the project is developed as a complete application.

### 3. Practice Software Architecture

The code is divided into components with specific responsibilities.

### 4. Practice Testing

Features are accompanied by automated integration tests.

### 5. Build a Portfolio Project

The final application demonstrates practical knowledge of:

```text
C++
├── STL
├── filesystem
├── classes
├── modular architecture
└── error handling

Operating Systems
├── Unix filesystem
├── permissions
├── symbolic links
├── processes
└── POSIX APIs

Software Engineering
├── CMake
├── Git
├── testing
├── architecture
└── documentation
```

---

# 📈 Future Improvements

Potential future features include:

* Persistent operation history
* Persistent snapshots
* Content-based snapshot comparison
* Trash/recycle-bin support
* Safer deletion recovery
* Parallel duplicate scanning
* Improved terminal UI
* File previews
* Configuration files
* Custom aliases
* Command scripting
* File watching
* Advanced Git integration
* Dependency graphs
* Remote filesystem support

---

# 🧠 What I Learned

This project provides practical experience with:

* Modern C++
* Object-oriented design
* `std::filesystem`
* Recursive filesystem traversal
* Unix permissions
* Symbolic and hard links
* File hashing
* Directory analysis
* Process execution
* Git integration
* C/C++ source analysis
* Command parsing
* Error handling
* Automated testing
* CMake
* Git/GitHub
* Software architecture

---

# 👩‍💻 Author

**Sarina**

Computer Engineering Student

This project was developed as part of a larger effort to build practical systems-programming and software-engineering experience.

---

# 📄 License

This project is available under the MIT License.

See `LICENSE` for details.
