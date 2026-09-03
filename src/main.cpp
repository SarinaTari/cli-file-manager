#include "CommandParser.h"
#include "FileManager.h"

#include <cstdint>
#include <iostream>
#include <string>

void show_help() {
    std::cout << "\n";
    std::cout << "Available commands:\n\n";

    std::cout << "Navigation:\n";
    std::cout << "  ls                         List directory contents\n";
    std::cout << "  ls --files                 Show only files\n";
    std::cout << "  ls --dirs                  Show only directories\n";
    std::cout << "  ls --name                  Sort by name\n";
    std::cout << "  ls --size                  Sort by size\n";
    std::cout << "  ls --name-desc             Sort by name descending\n";
    std::cout << "  ls --size-desc             Sort by size descending\n";
    std::cout << "  pwd                        Show current directory\n";
    std::cout << "  cd <directory>             Change directory\n";
    std::cout << "  back                       Go to parent directory\n\n";

    std::cout << "File operations:\n";
    std::cout << "  mkdir <directory>          Create directory\n";
    std::cout << "  touch <file>               Create file\n";
    std::cout << "  rename <old> <new>         Rename item\n";
    std::cout << "  cp <source> <destination>  Copy file/directory\n";
    std::cout << "  mv <source> <destination>  Move file/directory\n";
    std::cout << "  rm <path>                  Remove file/directory\n\n";

    std::cout << "Information:\n";
    std::cout << "  size <file>                Show file size\n";
    std::cout << "  type <path>                Show item type\n";
    std::cout << "  modified <path>            Show modification time\n";
    std::cout << "  info <path>                Show item information\n\n";

    std::cout << "Recursive operations:\n";
    std::cout << "  tree [path]                Show directory tree\n";
    std::cout << "  du <path>                  Show total size\n\n";

    std::cout << "Search:\n";
    std::cout << "  find <name> [path]         Find item by name\n";
    std::cout << "  findext <ext> [path]       Find files by extension\n";
    std::cout << "  findsize <bytes> [path]    Find files by minimum size\n\n";

    std::cout << "Other:\n";
    std::cout << "  help                       Show this help\n";
    std::cout << "  q                          Quit\n";
    std::cout << "  quit                       Quit\n\n";
}

int main() {
    FileManager file_manager;

    std::cout << "CLI File Manager\n";
    std::cout << "\n";
    std::cout
        << "Type 'help' for available commands.\n";
    std::cout << "\n";

    std::string input;

    while (true) {

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

        if (command.action == "q" ||
            command.action == "quit") {

            break;
        }

        else if (command.action == "help") {

            if (!command.arguments.empty()) {

                std::cout
                    << "Usage: help\n";

                continue;
            }

            show_help();
        }

        else if (command.action == "ls") {

            if (command.arguments.size() > 1) {

                std::cout
                    << "Usage: ls [option]\n";

                continue;
            }

            if (command.arguments.empty()) {

                file_manager.list_directory();
            }

            else {

                std::string option =
                    command.arguments[0];

                if (option != "--files" &&
                    option != "--dirs" &&
                    option != "--name" &&
                    option != "--size" &&
                    option != "--name-desc" &&
                    option != "--size-desc") {

                    std::cout
                        << "Unknown ls option: "
                        << option
                        << "\n";

                    continue;
                }

                file_manager.list_directory(
                    option
                );
            }
        }

        else if (command.action == "pwd") {

            if (!command.arguments.empty()) {

                std::cout
                    << "Usage: pwd\n";

                continue;
            }

            file_manager.print_working_directory();
        }

        else if (command.action == "cd") {

            if (command.arguments.size() != 1) {

                std::cout
                    << "Usage: cd <directory>\n";

                continue;
            }

            file_manager.change_directory(
                command.arguments[0]
            );
        }

        else if (command.action == "back") {

            if (!command.arguments.empty()) {

                std::cout
                    << "Usage: back\n";

                continue;
            }

            file_manager.go_back();
        }

        else if (command.action == "mkdir") {

            if (command.arguments.size() != 1) {

                std::cout
                    << "Usage: mkdir <directory>\n";

                continue;
            }

            file_manager.make_directory(
                command.arguments[0]
            );
        }

        else if (command.action == "touch") {

            if (command.arguments.size() != 1) {

                std::cout
                    << "Usage: touch <file>\n";

                continue;
            }

            file_manager.create_file(
                command.arguments[0]
            );
        }

        else if (command.action == "rename") {

            if (command.arguments.size() != 2) {

                std::cout
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

                std::cout
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

                std::cout
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

                std::cout
                    << "Usage: rm <path>\n";

                continue;
            }

            file_manager.remove_item(
                command.arguments[0]
            );
        }

        else if (command.action == "size") {

            if (command.arguments.size() != 1) {

                std::cout
                    << "Usage: size <file>\n";

                continue;
            }

            file_manager.show_file_size(
                command.arguments[0]
            );
        }

        else if (command.action == "type") {

            if (command.arguments.size() != 1) {

                std::cout
                    << "Usage: type <path>\n";

                continue;
            }

            file_manager.show_file_type(
                command.arguments[0]
            );
        }

        else if (command.action == "modified") {

            if (command.arguments.size() != 1) {

                std::cout
                    << "Usage: modified <path>\n";

                continue;
            }

            file_manager.show_modified_time(
                command.arguments[0]
            );
        }

        else if (command.action == "info") {

            if (command.arguments.size() != 1) {

                std::cout
                    << "Usage: info <path>\n";

                continue;
            }

            file_manager.show_info(
                command.arguments[0]
            );
        }

        else if (command.action == "tree") {

            if (command.arguments.size() > 1) {

                std::cout
                    << "Usage: tree [path]\n";

                continue;
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

                std::cout
                    << "Usage: du <path>\n";

                continue;
            }

            file_manager.show_directory_size(
                command.arguments[0]
            );
        }

        else if (command.action == "find") {

            if (command.arguments.size() < 1 ||
                command.arguments.size() > 2) {

                std::cout
                    << "Usage: find <name> [path]\n";

                continue;
            }

            if (command.arguments.size() == 1) {

                file_manager.find(
                    command.arguments[0]
                );
            }

            else {

                file_manager.find(
                    command.arguments[0],
                    command.arguments[1]
                );
            }
        }

        else if (command.action == "findext") {

            if (command.arguments.size() < 1 ||
                command.arguments.size() > 2) {

                std::cout
                    << "Usage: findext <extension> [path]\n";

                continue;
            }

            if (command.arguments.size() == 1) {

                file_manager.find_extension(
                    command.arguments[0]
                );
            }

            else {

                file_manager.find_extension(
                    command.arguments[0],
                    command.arguments[1]
                );
            }
        }

        else if (command.action == "findsize") {

            if (command.arguments.size() < 1 ||
                command.arguments.size() > 2) {

                std::cout
                    << "Usage: findsize <minimum_bytes> [path]\n";

                continue;
            }

            try {

                std::uintmax_t minimum_size =
                    std::stoull(
                        command.arguments[0]
                    );

                if (command.arguments.size() == 1) {

                    file_manager.find_size(
                        minimum_size
                    );
                }

                else {

                    file_manager.find_size(
                        minimum_size,
                        command.arguments[1]
                    );
                }
            }

            catch (const std::exception&) {

                std::cout
                    << "Error: invalid size.\n";
            }
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

    std::cout
        << "Goodbye!\n";

    return 0;
}