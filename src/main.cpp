#include "CommandParser.h"
#include "FileManager.h"

#include <iostream>
#include <string>

void show_help() {
    std::cout << "\n"
              << "Available commands:\n"
              << "\n"
              << "  ls                         List directory contents\n"
              << "  pwd                        Show current directory\n"
              << "  cd <dir>                   Change directory\n"
              << "  back                       Go to parent directory\n"
              << "\n"
              << "  mkdir <name>               Create directory\n"
              << "  touch <name>               Create empty file\n"
              << "  rename <old> <new>         Rename file or directory\n"
              << "  cp <source> <destination>  Copy file\n"
              << "  mv <source> <destination>  Move file or directory\n"
              << "  rm <name>                  Remove file or empty directory\n"
              << "\n"
              << "  size <file>                Show file size\n"
              << "  type <name>                Show item type\n"
              << "  modified <name>            Show modification time\n"
              << "  info <name>                Show detailed information\n"
              << "\n"
              << "  help                       Show this help message\n"
              << "  q                          Quit\n"
              << '\n';
}

int main() {
    FileManager file_manager;

    std::string input;

    std::cout << "CLI File Manager\n";
    std::cout << "Type 'help' for available commands.\n\n";

    while (true) {
        std::cout << "> ";

        if (!std::getline(std::cin, input)) {
            break;
        }

        Command command = CommandParser::parse(input);

        if (command.action.empty()) {
            continue;
        }

        if (command.action == "q" ||
            command.action == "quit") {
            std::cout << "Goodbye!\n";
            break;
        }

        else if (command.action == "help") {
            show_help();
        }

        else if (command.action == "ls") {
            if (!command.arguments.empty()) {
                std::cout << "Usage: ls\n";
            }
            else {
                file_manager.list_directory();
            }
        }

        else if (command.action == "pwd") {
            if (!command.arguments.empty()) {
                std::cout << "Usage: pwd\n";
            }
            else {
                file_manager.print_working_directory();
            }
        }

        else if (command.action == "cd") {
            if (command.arguments.size() != 1) {
                std::cout << "Usage: cd <directory>\n";
            }
            else {
                file_manager.change_directory(
                    command.arguments[0]
                );
            }
        }

        else if (command.action == "back") {
            if (!command.arguments.empty()) {
                std::cout << "Usage: back\n";
            }
            else {
                file_manager.go_back();
            }
        }

        else if (command.action == "mkdir") {
            if (command.arguments.size() != 1) {
                std::cout << "Usage: mkdir <name>\n";
            }
            else {
                file_manager.make_directory(
                    command.arguments[0]
                );
            }
        }

        else if (command.action == "touch") {
            if (command.arguments.size() != 1) {
                std::cout << "Usage: touch <name>\n";
            }
            else {
                file_manager.create_file(
                    command.arguments[0]
                );
            }
        }

        else if (command.action == "rename") {
            if (command.arguments.size() != 2) {
                std::cout << "Usage: rename <old> <new>\n";
            }
            else {
                file_manager.rename_item(
                    command.arguments[0],
                    command.arguments[1]
                );
            }
        }

        else if (command.action == "cp") {
            if (command.arguments.size() != 2) {
                std::cout << "Usage: cp <source> <destination>\n";
            }
            else {
                file_manager.copy_file(
                    command.arguments[0],
                    command.arguments[1]
                );
            }
        }

        else if (command.action == "mv") {
            if (command.arguments.size() != 2) {
                std::cout << "Usage: mv <source> <destination>\n";
            }
            else {
                file_manager.move_item(
                    command.arguments[0],
                    command.arguments[1]
                );
            }
        }

        else if (command.action == "rm") {
            if (command.arguments.size() != 1) {
                std::cout << "Usage: rm <name>\n";
            }
            else {
                file_manager.remove_item(
                    command.arguments[0]
                );
            }
        }

        else if (command.action == "size") {
            if (command.arguments.size() != 1) {
                std::cout << "Usage: size <file>\n";
            }
            else {
                file_manager.show_file_size(
                    command.arguments[0]
                );
            }
        }

        else if (command.action == "type") {
            if (command.arguments.size() != 1) {
                std::cout << "Usage: type <name>\n";
            }
            else {
                file_manager.show_file_type(
                    command.arguments[0]
                );
            }
        }

        else if (command.action == "modified") {
            if (command.arguments.size() != 1) {
                std::cout << "Usage: modified <name>\n";
            }
            else {
                file_manager.show_modified_time(
                    command.arguments[0]
                );
            }
        }

        else if (command.action == "info") {
            if (command.arguments.size() != 1) {
                std::cout << "Usage: info <name>\n";
            }
            else {
                file_manager.show_info(
                    command.arguments[0]
                );
            }
        }

        else {
            std::cout
                << "Unknown command: "
                << command.action
                << '\n';

            std::cout
                << "Type 'help' for available commands.\n";
        }
    }

    return 0;
}