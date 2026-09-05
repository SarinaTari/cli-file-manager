#include "CommandParser.h"
#include "FileManager.h"
#include "TerminalUI.h"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>


namespace {

void print_help() {
    std::cout << R"(
Available commands:

Navigation:
  ls [options] [filter]       List directory contents
  pwd                         Show current directory
  cd <directory>              Change directory
  back                        Go to parent directory

File operations:
  mkdir <name>                Create directory
  touch <name>                Create file
  rename <old> <new>          Rename item
  cp <source> <destination>   Copy item
  mv <source> <destination>   Move item
  rm <name>                   Remove item
  safe rm <path>              Safely preview and remove item

Information:
  size <name>                 Show file size
  type <name>                 Show file type
  modified <name>             Show modification time
  info <name>                 Show detailed information
  tree [name]                 Show directory tree
  tree --smart [path]         Show developer-aware project tree
  du [name]                   Show directory size

Search:
  find <name> [path]          Find by filename
  findext <extension> [path]  Find by extension
  findsize <bytes> [path]     Find files by minimum size

Permissions and links:
  perm <name>                 Show permissions
  chmod <mode> <name>         Change permissions
  ln <target> <link>          Create hard link
  symlink <target> <link>     Create symbolic link
  linktarget <name>           Show symbolic-link target

Advanced filesystem:
  analyze [path]              Analyze storage usage
  duplicates [path]           Find duplicate files

Developer intelligence:
  project [path]              Show project dashboard
  doctor [path]               Check project health
  why <path>                  Explain a file
  git [path]                  Analyze Git repository
  deps [path]                 Analyze C/C++ dependencies

Interface:
  ui                          Launch interactive terminal UI
  help                        Show this help
  q                           Quit
  quit                        Quit
  exit                        Quit

History:
  history                     Show operation history
  undo                        Undo last operation

Snapshots:
  snapshot                    Create filesystem snapshot
  showsnapshot                Show current snapshot
  diff                        Compare current state with snapshot
  clearsnapshot               Delete stored snapshot

Examples:

  ls
  ls -a
  ls size
  cd Documents
  mkdir projects
  touch main.cpp
  cp main.cpp backup.cpp
  findext .cpp .
  findsize 1000 .
  chmod 755 script.sh
  analyze .
  duplicates .
  project .
  doctor .
  why src/main.cpp
  git .
  deps .
  tree --smart .
  ui

)";
}

std::uintmax_t parse_size(
    const std::string& value
) {
    if (value.empty()) {
        throw std::invalid_argument(
            "Minimum size must be a non-negative integer."
        );
    }

    for (char character : value) {
        if (
            character < '0'
            || character > '9'
        ) {
            throw std::invalid_argument(
                "Minimum size must be a non-negative integer."
            );
        }
    }

    try {
        return std::stoull(value);
    }
    catch (...) {
        throw std::invalid_argument(
            "Minimum size must be a non-negative integer."
        );
    }
}

}

int main() {
    FileManager file_manager;

    std::cout
        << "CLI File Manager\n"
        << "Type 'help' for available commands.\n\n";

    std::string input;

    while (true) {
        try {
            std::cout << "> ";

            if (!std::getline(
                    std::cin,
                    input
                )) {
                break;
            }

            Command command =
                CommandParser::parse(input);

            if (command.action.empty()) {
                continue;
            }

            // --------------------------------------------------
            // Quit
            // --------------------------------------------------

            if (
                command.action == "q"
                || command.action == "quit"
                || command.action == "exit"
            ) {
                break;
            }

            // --------------------------------------------------
            // Help
            // --------------------------------------------------

            if (command.action == "help") {
                print_help();
            }

            // --------------------------------------------------
            // Navigation
            // --------------------------------------------------

            else if (command.action == "pwd") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: pwd"
                    );
                }

                file_manager.print_working_directory();
            }

            else if (command.action == "ls") {
                bool show_hidden = false;

                std::string sort_option =
                    "name";

                std::string filter =
                    "all";

                for (
                    const std::string& argument :
                    command.arguments
                ) {
                    if (
                        argument == "-a"
                        || argument == "--all"
                    ) {
                        show_hidden = true;
                    }
                    else if (
                        argument == "name"
                        || argument == "size"
                        || argument == "modified"
                    ) {
                        sort_option = argument;
                    }
                    else if (
                        argument == "all"
                        || argument == "files"
                        || argument == "dirs"
                    ) {
                        filter = argument;
                    }
                    else {
                        throw std::invalid_argument(
                            "Unknown ls option: "
                            + argument
                        );
                    }
                }

                file_manager.list_directory(
                    show_hidden,
                    sort_option,
                    filter
                );
            }

            else if (command.action == "cd") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: cd <directory>"
                    );
                }

                file_manager.change_directory(
                    command.arguments[0]
                );
            }

            else if (command.action == "back") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: back"
                    );
                }

                file_manager.go_back();
            }

            // --------------------------------------------------
            // File operations
            // --------------------------------------------------

            else if (command.action == "mkdir") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: mkdir <name>"
                    );
                }

                file_manager.make_directory(
                    command.arguments[0]
                );
            }

            else if (command.action == "touch") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: touch <name>"
                    );
                }

                file_manager.create_file(
                    command.arguments[0]
                );
            }

            else if (command.action == "rename") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: rename <old> <new>"
                    );
                }

                file_manager.rename_item(
                    command.arguments[0],
                    command.arguments[1]
                );
            }

            else if (command.action == "cp") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: cp <source> <destination>"
                    );
                }

                file_manager.copy_item(
                    command.arguments[0],
                    command.arguments[1]
                );
            }

            else if (command.action == "mv") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: mv <source> <destination>"
                    );
                }

                file_manager.move_item(
                    command.arguments[0],
                    command.arguments[1]
                );
            }

            else if (command.action == "rm") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: rm <name>"
                    );
                }

                file_manager.remove_item(
                    command.arguments[0]
                );
            }

            else if (command.action == "safe") {
                if (
                    command.arguments.size() != 2
                    || command.arguments[0] != "rm"
                ) {
                    throw std::invalid_argument(
                        "Usage: safe rm <path>"
                    );
                }

                file_manager.safe_remove(
                    command.arguments[1]
                );
            }

            // --------------------------------------------------
            // Information
            // --------------------------------------------------

            else if (command.action == "size") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: size <name>"
                    );
                }

                file_manager.show_file_size(
                    command.arguments[0]
                );
            }

            else if (command.action == "type") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: type <name>"
                    );
                }

                file_manager.show_file_type(
                    command.arguments[0]
                );
            }

            else if (command.action == "modified") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: modified <name>"
                    );
                }

                file_manager.show_modified_time(
                    command.arguments[0]
                );
            }

            else if (command.action == "info") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: info <name>"
                    );
                }

                file_manager.show_info(
                    command.arguments[0]
                );
            }

            // --------------------------------------------------
            // Directory tree
            // --------------------------------------------------

            else if (command.action == "tree") {

                if (
                    !command.arguments.empty()
                    && command.arguments[0] == "--smart"
                ) {
                    if (command.arguments.size() > 2) {
                        throw std::invalid_argument(
                            "Usage: tree --smart [path]"
                        );
                    }

                    const std::string path =
                        command.arguments.size() == 2
                            ? command.arguments[1]
                            : ".";

                    file_manager.show_smart_tree(
                        path
                    );
                }
                else {
                    if (command.arguments.size() > 1) {
                        throw std::invalid_argument(
                            "Usage: tree [name]"
                        );
                    }

                    if (command.arguments.empty()) {
                        file_manager.show_tree();
                    }
                    else {
                        file_manager.show_tree(
                            command.arguments[0]
                        );
                    }
                }
            }

            else if (command.action == "du") {
                if (command.arguments.size() > 1) {
                    throw std::invalid_argument(
                        "Usage: du [name]"
                    );
                }

                if (command.arguments.empty()) {
                    file_manager.show_directory_size(
                        "."
                    );
                }
                else {
                    file_manager.show_directory_size(
                        command.arguments[0]
                    );
                }
            }

            // --------------------------------------------------
            // Search
            // --------------------------------------------------

            else if (command.action == "find") {
                if (
                    command.arguments.size() < 1
                    || command.arguments.size() > 2
                ) {
                    throw std::invalid_argument(
                        "Usage: find <name> [path]"
                    );
                }

                if (command.arguments.size() == 1) {
                    file_manager.find_by_name(
                        command.arguments[0]
                    );
                }
                else {
                    file_manager.find_by_name(
                        command.arguments[0],
                        command.arguments[1]
                    );
                }
            }

            else if (command.action == "findext") {
                if (
                    command.arguments.size() < 1
                    || command.arguments.size() > 2
                ) {
                    throw std::invalid_argument(
                        "Usage: findext <extension> [path]"
                    );
                }

                if (command.arguments.size() == 1) {
                    file_manager.find_by_extension(
                        command.arguments[0]
                    );
                }
                else {
                    file_manager.find_by_extension(
                        command.arguments[0],
                        command.arguments[1]
                    );
                }
            }

            else if (command.action == "findsize") {
                if (
                    command.arguments.size() < 1
                    || command.arguments.size() > 2
                ) {
                    throw std::invalid_argument(
                        "Usage: findsize <bytes> [path]"
                    );
                }

                std::uintmax_t minimum_size =
                    parse_size(
                        command.arguments[0]
                    );

                if (command.arguments.size() == 1) {
                    file_manager.find_by_size(
                        minimum_size
                    );
                }
                else {
                    file_manager.find_by_size(
                        minimum_size,
                        command.arguments[1]
                    );
                }
            }

            // --------------------------------------------------
            // Permissions and links
            // --------------------------------------------------

            else if (command.action == "perm") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: perm <name>"
                    );
                }

                file_manager.show_permissions(
                    command.arguments[0]
                );
            }

            else if (command.action == "chmod") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: chmod <mode> <name>"
                    );
                }

                file_manager.change_permissions(
                    command.arguments[0],
                    command.arguments[1]
                );
            }

            else if (command.action == "ln") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: ln <target> <link>"
                    );
                }

                file_manager.create_hard_link(
                    command.arguments[0],
                    command.arguments[1]
                );
            }

            else if (command.action == "symlink") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: symlink <target> <link>"
                    );
                }

                file_manager.create_symbolic_link(
                    command.arguments[0],
                    command.arguments[1]
                );
            }

            else if (command.action == "linktarget") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: linktarget <name>"
                    );
                }

                file_manager.show_link_target(
                    command.arguments[0]
                );
            }

            // --------------------------------------------------
            // Advanced filesystem
            // --------------------------------------------------

            else if (command.action == "analyze") {
                if (command.arguments.size() > 1) {
                    throw std::invalid_argument(
                        "Usage: analyze [path]"
                    );
                }

                const std::string path =
                    command.arguments.empty()
                        ? "."
                        : command.arguments[0];

                file_manager.analyze_storage(
                    path
                );
            }

            else if (command.action == "duplicates") {
                if (command.arguments.size() > 1) {
                    throw std::invalid_argument(
                        "Usage: duplicates [path]"
                    );
                }

                const std::string path =
                    command.arguments.empty()
                        ? "."
                        : command.arguments[0];

                file_manager.find_duplicates(
                    path
                );
            }

            // --------------------------------------------------
            // Phase 15.1: Project Dashboard
            // --------------------------------------------------

            else if (command.action == "project") {
                if (command.arguments.size() > 1) {
                    throw std::invalid_argument(
                        "Usage: project [path]"
                    );
                }

                const std::string path =
                    command.arguments.empty()
                        ? "."
                        : command.arguments[0];

                file_manager.show_project_dashboard(
                    path
                );
            }

            else if (command.action == "doctor") {
                if (command.arguments.size() > 1) {
                    throw std::invalid_argument(
                        "Usage: doctor [path]"
                    );
                }

                const std::string path =
                    command.arguments.empty()
                        ? "."
                        : command.arguments[0];

                file_manager.project_doctor(
                    path
                );
            }

            // --------------------------------------------------
            // Git
            // --------------------------------------------------

            else if (command.action == "git") {
                if (command.arguments.size() > 1) {
                    throw std::invalid_argument(
                        "Usage: git [path]"
                    );
                }

                const std::string path =
                    command.arguments.empty()
                        ? "."
                        : command.arguments[0];

                file_manager.analyze_git(
                    path
                );
            }

            else if (command.action == "why") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: why <path>"
                    );
                }

                file_manager.explain_file(
                    command.arguments[0]
                );
            }

            // --------------------------------------------------
            // Dependencies
            // --------------------------------------------------

            else if (command.action == "deps") {
                if (command.arguments.size() > 1) {
                    throw std::invalid_argument(
                        "Usage: deps [path]"
                    );
                }

                const std::string path =
                    command.arguments.empty()
                        ? "."
                        : command.arguments[0];

                file_manager.analyze_dependencies(
                    path
                );
            }

            // --------------------------------------------------
            // Terminal UI
            // --------------------------------------------------

            else if (command.action == "ui") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: ui"
                    );
                }

                TerminalUI ui(
                    file_manager
                );

                ui.run();
            }

            // --------------------------------------------------
            // History
            // --------------------------------------------------

            else if (command.action == "history") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: history"
                    );
                }

                file_manager.show_history();
            }

            else if (command.action == "undo") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: undo"
                    );
                }

                file_manager.undo();
            }

            // --------------------------------------------------
            // Snapshots
            // --------------------------------------------------

            else if (command.action == "snapshot") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: snapshot"
                    );
                }

                file_manager.create_snapshot();
            }

            else if (command.action == "showsnapshot") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: showsnapshot"
                    );
                }

                file_manager.show_snapshot();
            }

            else if (command.action == "diff") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: diff"
                    );
                }

                file_manager.show_snapshot_diff();
            }

            else if (command.action == "clearsnapshot") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: clearsnapshot"
                    );
                }

                file_manager.clear_snapshot();
            }

            // --------------------------------------------------
            // Unknown command
            // --------------------------------------------------

            else {
                std::cout
                    << "Unknown command: "
                    << command.action
                    << "\n";

                std::cout
                    << "Type 'help' for available commands.\n";
            }
        }
        catch (const std::exception& error) {
            std::cout
                << "Error: "
                << error.what()
                << "\n";
        }
    }

    return 0;
}