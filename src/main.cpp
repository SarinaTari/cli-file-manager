#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>

namespace fs = std::filesystem;

void list_directory(const fs::path& directory) {

    for (const auto& entry : fs::directory_iterator(directory)) {

        if (entry.is_directory()) {
            std::cout << "[DIR]  ";
        }
        else if (entry.is_regular_file()) {
            std::cout << "[FILE] ";
        }

        std::cout << entry.path().filename() << '\n';
    }
}

void show_help() {

    std::cout << "\nCommands:\n";
    std::cout << "  ls              List directory\n";
    std::cout << "  pwd             Show current directory\n";
    std::cout << "  cd <dir>        Enter directory\n";
    std::cout << "  back            Go to parent directory\n";
    std::cout << "  help            Show help\n";
    std::cout << "  q               Quit\n";
}

int main() {

    fs::path current_directory = fs::current_path();

    std::cout << "CLI File Manager\n";

    while (true) {

        std::cout << "\nCurrent directory: "
                  << current_directory
                  << '\n';

        std::cout << "> ";

        std::string command;
        std::getline(std::cin, command);

        std::stringstream ss(command);

        std::string action;
        std::string argument;

        ss >> action;
        ss >> argument;

        if (action == "q") {
            break;
        }

        else if (action == "ls") {
            list_directory(current_directory);
        }

        else if (action == "pwd") {
            std::cout << current_directory << '\n';
        }

        else if (action == "help") {
            show_help();
        }

        else if (action == "cd") {

            if (argument.empty()) {
                std::cout << "Usage: cd <directory>\n";
                continue;
            }

            fs::path new_directory =
                current_directory / argument;

            if (!fs::exists(new_directory)) {
                std::cout << "Directory does not exist.\n";
                continue;
            }

            if (!fs::is_directory(new_directory)) {
                std::cout << "Not a directory.\n";
                continue;
            }

            current_directory =
                fs::canonical(new_directory);
        }

        else if (action == "back") {

            fs::path parent =
                current_directory.parent_path();

            if (parent == current_directory) {
                std::cout << "Already at root directory.\n";
            }
            else {
                current_directory = parent;
            }
        }

        else {
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }

    std::cout << "Goodbye!\n";

    return 0;
}