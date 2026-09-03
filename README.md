## Developer Intelligence

Phase 13 adds developer-oriented analysis capabilities that allow the file manager to understand more about the directory being explored.

### Project Detection

The `project` command examines a directory for common project indicators such as:

* `CMakeLists.txt`
* `Makefile`
* `pyproject.toml`
* `requirements.txt`
* `Cargo.toml`
* `pom.xml`
* `build.gradle`
* `package.json`

It also reports source-file statistics for common programming languages.

Example:

```text
> project .

Project analysis for: "."

Project type(s):
  - C++ / CMake

Source statistics:
  C/C++ source files: 7
  C/C++ header files: 6
  Python files: 0
  Rust files: 0
  Java files: 0

Detected project indicators:
  CMakeLists.txt
```

### Git Awareness

The `git` command analyzes the Git repository associated with a directory.

It can display:

* Whether the directory is inside a Git repository
* Repository root
* Current branch
* Latest commit
* Staged changes
* Modified files
* Untracked files
* Current Git status

Example:

```text
> git .

Git repository: Yes
Repository root: /Users/user/projects/cli-file-manager
Branch: main
Latest commit: abc1234 Add developer intelligence

Working tree:
  Staged changes: 0
  Modified files: 0
  Untracked files: 0

Working tree is clean.
```

### C/C++ Dependency Analysis

The `deps` command analyzes C/C++ source and header files.

It detects:

* `.cpp`
* `.cc`
* `.cxx`
* `.h`
* `.hh`
* `.hpp`
* `.hxx`

It extracts `#include` directives and displays relationships between files.

Example:

```text
> deps .

C/C++ dependency analysis for: "."

C/C++ files: 12
Total #include directives: 34

Include relationships:

"src/main.cpp"
  -> CommandParser.h
  -> FileManager.h
  -> TerminalUI.h

"src/FileManager.cpp"
  -> FileManager.h
  -> iostream
  -> filesystem
```

This provides a basic static view of the project's C/C++ dependency structure.

---

## Developer Commands

```text
project [path]       Detect project type and statistics
git [path]           Analyze Git repository
deps [path]          Analyze C/C++ dependencies
```

Examples:

```text
project .
git .
deps .
```

The path is optional. When omitted, the current file-manager directory is analyzed.

---

## Development Phases

### Phase 13 — Developer Intelligence

* Automatic project-type detection
* CMake project detection
* Make project detection
* Python project detection
* Rust project detection
* Java project detection
* Node.js project detection
* Source-file statistics
* Git repository detection
* Git branch detection
* Git status analysis
* Latest commit information
* C/C++ source discovery
* C/C++ header discovery
* `#include` extraction
* Include-frequency analysis
* Automated tests for developer features

---

## Future Roadmap

The next phases will continue turning the file manager into a developer-oriented filesystem tool.

### Phase 14 — History and Recovery

Planned features:

* Operation history
* Undo
* Snapshots
* Filesystem state comparison
* Safer recovery workflows

### Phase 15 — Performance and Portfolio Release

Planned features:

* Performance optimization
* Large-directory optimization
* Improved terminal UI
* Better command architecture
* More comprehensive tests
* Configuration system
* Developer documentation
* Final portfolio polish
* Release-quality error handling

Longer-term ideas include:

* Git-aware file operations
* Project statistics
* Advanced dependency graphs
* User-defined aliases
* Scripting
* Remote filesystem support
