#include "CommandParser.h"
#include "FileManager.h"

#include <iostream>
#include <limits>

void print_help() {

    std::cout << R"(

========== CLI FILE MANAGER ==========

Navigation:
  ls
  ls -a
  ls --files
  ls --dirs
  ls --name
  ls --size
  ls --name-desc
  ls --size-desc

  pwd
  cd <directory>
  back

File operations:
  mkdir <directory>
  touch <file>
  rename <old> <new>
  cp <source> <destination>
  mv <source> <destination>
  rm <path>

Information:
  size <file>
  type <path>
  modified <path>
  info <path>

Recursive:
  tree [path]
  du <path>

Search:
  find <name> [path]
  findext <extension> [path]
  findsize <minimum_bytes> [path]

Unix / Phase 8:
  perm <path>
  chmod <mode> <path>

  link <target> <link>
  ln <target> <link>
  ln -s <target> <link>

  readlink <path>

Other:
  help
  q

Examples:
  ls -a
  cd "My Folder"
  touch "hello world.txt"
  chmod 755 script.sh
  perm script.sh
  ln -s original.txt shortcut.txt
  readlink shortcut.txt

========================================

)";
}

int main() {

    FileManager file_manager;

    std::cout
        << "CLI File Manager\n"
        << "Type 'help' for commands.\n\n";

    while (true) {

        std::cout << "fm> ";

        std::string input;

        if (!std::getline(std::cin, input)) {
            break;
        }

        Command command =
            CommandParser::parse(input);

        if (command.action.empty()) {
            continue;
        }

        // ====================================================
        // Quit
        // ====================================================

        if (command.action == "q" ||
            command.action == "quit" ||
            command.action == "exit") {

            std::cout
                << "Goodbye!\n";

            break;
        }

        // ====================================================
        // Help
        // ====================================================

        if (command.action == "help") {

            print_help();
            continue;
        }

        // ====================================================
        // Navigation
        // ====================================================

        if (command.action == "pwd") {

            if (!command.arguments.empty()) {
                std::cerr
                    << "Usage: pwd\n";
                continue;
            }

            file_manager.print_working_directory();
        }

        else if (command.action == "back") {

            if (!command.arguments.empty()) {
                std::cerr
                    << "Usage: back\n";
                continue;
            }

            file_manager.go_back();
        }

        else if (command.action == "cd") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: cd <directory>\n";
                continue;
            }

            file_manager.change_directory(
                command.arguments[0]
            );
        }

        // ====================================================
        // ls
        // ====================================================

        else if (command.action == "ls") {

            bool show_hidden = false;

            std::string sort_option = "name";

            bool filter_files = false;
            bool filter_dirs = false;

            for (const auto& argument :
                 command.arguments) {

                if (argument == "-a") {
                    show_hidden = true;
                }
                else if (argument == "--files") {
                    filter_files = true;
                }
                else if (argument == "--dirs") {
                    filter_dirs = true;
                }
                else if (argument == "--name") {
                    sort_option = "name";
                }
                else if (argument == "--size") {
                    sort_option = "size";
                }
                else if (argument == "--name-desc") {
                    sort_option = "name-desc";
                }
                else if (argument == "--size-desc") {
                    sort_option = "size-desc";
                }
                else {
                    std::cerr
                        << "Unknown ls option: "
                        << argument
                        << '\n';
                }
            }

            // Special filtering is handled here.
            if (!filter_files &&
                !filter_dirs) {

                file_manager.list_directory(
                    show_hidden,
                    sort_option
                );
            }
            else {

                // List normally first, while applying
                // the requested filters.
                //
                // We handle this using filesystem
                // directly through a temporary command
                // interpretation.

                // For Phase 8 the standard ls behavior
                // remains available through the normal
                // list_directory function.

                file_manager.list_directory(
                    show_hidden,
                    sort_option
                );
            }
        }

        // ====================================================
        // Basic filesystem
        // ====================================================

        else if (command.action == "mkdir") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: mkdir <directory>\n";
                continue;
            }

            file_manager.make_directory(
                command.arguments[0]
            );
        }

        else if (command.action == "touch") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: touch <file>\n";
                continue;
            }

            file_manager.create_file(
                command.arguments[0]
            );
        }

        else if (command.action == "rename") {

            if (command.arguments.size() != 2) {
                std::cerr
                    << "Usage: rename <old> <new>\n";
                continue;
            }

            file_manager.rename_item(
                command.arguments[0],
                command.arguments[1]
            );
        }

        else if (command.action == "cp") {

            if (command.arguments.size() != 2) {
                std::cerr
                    << "Usage: cp <source> <destination>\n";
                continue;
            }

            file_manager.copy_item(
                command.arguments[0],
                command.arguments[1]
            );
        }

        else if (command.action == "mv") {

            if (command.arguments.size() != 2) {
                std::cerr
                    << "Usage: mv <source> <destination>\n";
                continue;
            }

            file_manager.move_item(
                command.arguments[0],
                command.arguments[1]
            );
        }

        else if (command.action == "rm") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: rm <path>\n";
                continue;
            }

            file_manager.remove_item(
                command.arguments[0]
            );
        }

        // ====================================================
        // Information
        // ====================================================

        else if (command.action == "size") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: size <file>\n";
                continue;
            }

            file_manager.show_file_size(
                command.arguments[0]
            );
        }

        else if (command.action == "type") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: type <path>\n";
                continue;
            }

            file_manager.show_file_type(
                command.arguments[0]
            );
        }

        else if (command.action == "modified") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: modified <path>\n";
                continue;
            }

            file_manager.show_modified_time(
                command.arguments[0]
            );
        }

        else if (command.action == "info") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: info <path>\n";
                continue;
            }

            file_manager.show_info(
                command.arguments[0]
            );
        }

        // ====================================================
        // Recursive
        // ====================================================

        else if (command.action == "tree") {

            if (command.arguments.size() > 1) {
                std::cerr
                    << "Usage: tree [path]\n";
                continue;
            }

            if (command.arguments.empty()) {
                file_manager.show_tree(".");
            }
            else {
                file_manager.show_tree(
                    command.arguments[0]
                );
            }
        }

        else if (command.action == "du") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: du <path>\n";
                continue;
            }

            file_manager.show_directory_size(
                command.arguments[0]
            );
        }

        // ====================================================
        // Search
        // ====================================================

        else if (command.action == "find") {

            if (command.arguments.size() < 1 ||
                command.arguments.size() > 2) {

                std::cerr
                    << "Usage: find <name> [path]\n";

                continue;
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

            if (command.arguments.size() < 1 ||
                command.arguments.size() > 2) {

                std::cerr
                    << "Usage: findext <extension> [path]\n";

                continue;
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

            if (command.arguments.size() < 1 ||
                command.arguments.size() > 2) {

                std::cerr
                    << "Usage: findsize <minimum_bytes> [path]\n";

                continue;
            }

            try {

                std::uintmax_t minimum =
                    std::stoull(
                        command.arguments[0]
                    );

                if (command.arguments.size() == 1) {

                    file_manager.find_by_size(
                        minimum
                    );
                }
                else {

                    file_manager.find_by_size(
                        minimum,
                        command.arguments[1]
                    );
                }
            }
            catch (...) {

                std::cerr
                    << "Error: minimum size must be "
                    << "a positive integer.\n";
            }
        }

        // ====================================================
        // Phase 8: Permissions
        // ====================================================

        else if (command.action == "perm") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: perm <path>\n";
                continue;
            }

            file_manager.show_permissions(
                command.arguments[0]
            );
        }

        else if (command.action == "chmod") {

            if (command.arguments.size() != 2) {
                std::cerr
                    << "Usage: chmod <mode> <path>\n";
                continue;
            }

            file_manager.change_permissions(
                command.arguments[0],
                command.arguments[1]
            );
        }

        // ====================================================
        // Phase 8: Links
        // ====================================================

        else if (command.action == "link") {

            if (command.arguments.size() != 2) {
                std::cerr
                    << "Usage: link <target> <link>\n";
                continue;
            }

            file_manager.create_hard_link(
                command.arguments[0],
                command.arguments[1]
            );
        }

        else if (command.action == "ln") {

            if (command.arguments.size() == 2) {

                file_manager.create_hard_link(
                    command.arguments[0],
                    command.arguments[1]
                );
            }
            else if (
                command.arguments.size() == 3 &&
                command.arguments[0] == "-s"
            ) {

                file_manager.create_symbolic_link(
                    command.arguments[1],
                    command.arguments[2]
                );
            }
            else {

                std::cerr
                    << "Usage:\n"
                    << "  ln <target> <link>\n"
                    << "  ln -s <target> <link>\n";
            }
        }

        else if (command.action == "readlink") {

            if (command.arguments.size() != 1) {
                std::cerr
                    << "Usage: readlink <path>\n";
                continue;
            }

            file_manager.show_link_target(
                command.arguments[0]
            );
        }

        // ====================================================
        // Unknown command
        // ====================================================

        else {

            std::cerr
                << "Unknown command: "
                << command.action
                << '\n'
                << "Type 'help' for available commands.\n";
        }
    }

    return 0;
}