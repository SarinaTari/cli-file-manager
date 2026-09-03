#include "CommandParser.h"
#include "FileManager.h"
#include "TerminalUI.h"

#include <cstdint>
#include <filesystem>
#include <iostream>
#include <limits>
#include <string>

namespace fs = std::filesystem;

static bool is_unsigned_integer(
    const std::string& value
) {
    if (value.empty()) {
        return false;
    }

    for (char character : value) {
        if (
            character < '0'
            || character > '9'
        ) {
            return false;
        }
    }

    return true;
}

static void print_help() {
    std::cout << R"(
Available commands:

Navigation:
  pwd
  cd <directory>
  back

Listing:
  ls
  ls -a
  ls --files
  ls --dirs
  ls --name
  ls --size
  ls --name-desc
  ls --size-desc

File operations:
  mkdir <name>
  touch <name>
  rename <old> <new>
  cp <source> <destination>
  mv <source> <destination>
  rm <name>

Information:
  size <file>
  type <file>
  modified <file>
  info <file>

Recursive:
  tree [path]
  du <path>

Search:
  find <name> [path]
  findext <extension> [path]
  findsize <minimum_bytes> [path]

Permissions:
  perm <path>
  chmod <mode> <path>

Links:
  ln <target> <link>
  ln -s <target> <link>
  readlink <path>

Interface:
  ui
  help
  q
  quit
  exit
)";
}

static void run_command_mode(
    FileManager& file_manager
) {
    std::cout << "CLI File Manager\n";
    std::cout << "Type 'help' for commands.\n\n";

    std::string input;

    while (true) {
        std::cout << "fm> ";

        if (!std::getline(std::cin, input)) {
            std::cout << "\nGoodbye.\n";
            break;
        }

        try {
            Command command =
                CommandParser::parse(input);

            if (command.action.empty()) {
                continue;
            }

            if (
                command.action == "q"
                || command.action == "quit"
                || command.action == "exit"
            ) {
                std::cout << "Goodbye.\n";
                break;
            }

            if (command.action == "ui") {
                TerminalUI ui(file_manager);
                ui.run();

                std::cout << "\n";
                continue;
            }

            if (command.action == "help") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: help"
                    );
                }

                print_help();
            }

            else if (command.action == "pwd") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: pwd"
                    );
                }

                file_manager.print_working_directory();
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

            else if (command.action == "ls") {
                bool show_hidden = false;
                std::string sort_option = "name";
                std::string filter = "all";

                for (
                    const std::string& argument
                    : command.arguments
                ) {
                    if (argument == "-a") {
                        show_hidden = true;
                    }
                    else if (argument == "--files") {
                        filter = "files";
                    }
                    else if (argument == "--dirs") {
                        filter = "dirs";
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

            else if (command.action == "size") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: size <file>"
                    );
                }

                file_manager.show_file_size(
                    command.arguments[0]
                );
            }

            else if (command.action == "type") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: type <file>"
                    );
                }

                file_manager.show_file_type(
                    command.arguments[0]
                );
            }

            else if (command.action == "modified") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: modified <file>"
                    );
                }

                file_manager.show_modified_time(
                    command.arguments[0]
                );
            }

            else if (command.action == "info") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: info <file>"
                    );
                }

                file_manager.show_info(
                    command.arguments[0]
                );
            }

            else if (command.action == "tree") {
                if (
                    command.arguments.size() > 1
                ) {
                    throw std::invalid_argument(
                        "Usage: tree [path]"
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

            else if (command.action == "du") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: du <path>"
                    );
                }

                file_manager.show_directory_size(
                    command.arguments[0]
                );
            }

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
                        "Usage: findsize <minimum_bytes> [path]"
                    );
                }

                if (
                    !is_unsigned_integer(
                        command.arguments[0]
                    )
                ) {
                    throw std::invalid_argument(
                        "Minimum size must be a non-negative integer."
                    );
                }

                std::uintmax_t minimum_size;

                try {
                    minimum_size =
                        std::stoull(
                            command.arguments[0]
                        );
                }
                catch (...) {
                    throw std::invalid_argument(
                        "Minimum size is too large."
                    );
                }

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

            else if (command.action == "perm") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: perm <path>"
                    );
                }

                file_manager.show_permissions(
                    command.arguments[0]
                );
            }

            else if (command.action == "chmod") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: chmod <mode> <path>"
                    );
                }

                file_manager.change_permissions(
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
                    command.arguments.size() == 3
                    && command.arguments[0] == "-s"
                ) {
                    file_manager.create_symbolic_link(
                        command.arguments[1],
                        command.arguments[2]
                    );
                }
                else {
                    throw std::invalid_argument(
                        "Usage: ln <target> <link> "
                        "or ln -s <target> <link>"
                    );
                }
            }

            else if (command.action == "readlink") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: readlink <path>"
                    );
                }

                file_manager.show_link_target(
                    command.arguments[0]
                );
            }

            else {
                std::cout
                    << "Unknown command: "
                    << command.action
                    << "\n";

                std::cout
                    << "Type 'help' for available commands.\n";
            }
        }
        catch (const std::invalid_argument& exception) {
            std::cout
                << "Error: "
                << exception.what()
                << "\n";
        }
        catch (const fs::filesystem_error& exception) {
            std::cout
                << "Filesystem error: "
                << exception.what()
                << "\n";
        }
        catch (const std::runtime_error& exception) {
            std::cout
                << "Error: "
                << exception.what()
                << "\n";
        }
        catch (const std::exception& exception) {
            std::cout
                << "Unexpected error: "
                << exception.what()
                << "\n";
        }
    }
}

int main() {
    FileManager file_manager;

    run_command_mode(file_manager);

    return 0;
}