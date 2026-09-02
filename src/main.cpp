#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>
#include <fstream>

namespace fs = std::filesystem;


// --------------------------------------------------
// List current directory
// --------------------------------------------------

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


// --------------------------------------------------
// Create directory
// --------------------------------------------------

void make_directory(const fs::path& current_directory,
                    const std::string& name) {

    fs::path new_directory =
        current_directory / name;

    if (fs::exists(new_directory)) {
        std::cout << "File or directory already exists.\n";
        return;
    }

    try {

        if (fs::create_directory(new_directory)) {
            std::cout << "Directory created.\n";
        }
        else {
            std::cout << "Failed to create directory.\n";
        }

    }
    catch (const fs::filesystem_error& e) {
        std::cout << "Failed to create directory: "
                  << e.what() << '\n';
    }
}


// --------------------------------------------------
// Create empty file
// --------------------------------------------------

void create_file(const fs::path& current_directory,
                 const std::string& name) {

    fs::path new_file =
        current_directory / name;

    if (fs::exists(new_file)) {
        std::cout << "File or directory already exists.\n";
        return;
    }

    std::ofstream file(new_file);

    if (file) {
        std::cout << "File created.\n";
    }
    else {
        std::cout << "Failed to create file.\n";
    }
}


// --------------------------------------------------
// Rename file or directory
// --------------------------------------------------

void rename_item(const fs::path& current_directory,
                 const std::string& old_name,
                 const std::string& new_name) {

    fs::path old_path =
        current_directory / old_name;

    fs::path new_path =
        current_directory / new_name;

    if (!fs::exists(old_path)) {
        std::cout << "Source does not exist.\n";
        return;
    }

    if (fs::exists(new_path)) {
        std::cout << "Destination already exists.\n";
        return;
    }

    try {

        fs::rename(old_path, new_path);

        std::cout << "Renamed successfully.\n";
    }
    catch (const fs::filesystem_error& e) {

        std::cout << "Rename failed: "
                  << e.what() << '\n';
    }
}


// --------------------------------------------------
// Copy file
// --------------------------------------------------

void copy_file(const fs::path& current_directory,
               const std::string& source,
               const std::string& destination) {

    fs::path source_path =
        current_directory / source;

    fs::path destination_path =
        current_directory / destination;

    if (!fs::exists(source_path)) {
        std::cout << "Source does not exist.\n";
        return;
    }

    if (!fs::is_regular_file(source_path)) {
        std::cout << "Source is not a regular file.\n";
        return;
    }

    if (fs::exists(destination_path)) {
        std::cout << "Destination already exists.\n";
        return;
    }

    try {

        fs::copy_file(
            source_path,
            destination_path
        );

        std::cout << "File copied successfully.\n";
    }
    catch (const fs::filesystem_error& e) {

        std::cout << "Copy failed: "
                  << e.what() << '\n';
    }
}


// --------------------------------------------------
// Move file or directory
// --------------------------------------------------

void move_item(const fs::path& current_directory,
               const std::string& source,
               const std::string& destination) {

    fs::path source_path =
        current_directory / source;

    fs::path destination_path =
        current_directory / destination;

    if (!fs::exists(source_path)) {
        std::cout << "Source does not exist.\n";
        return;
    }

    if (fs::exists(destination_path)) {
        std::cout << "Destination already exists.\n";
        return;
    }

    try {

        fs::rename(source_path, destination_path);

        std::cout << "Moved successfully.\n";
    }
    catch (const fs::filesystem_error& e) {

        std::cout << "Move failed: "
                  << e.what() << '\n';
    }
}


// --------------------------------------------------
// Remove file or empty directory
// --------------------------------------------------

void remove_item(const fs::path& current_directory,
                 const std::string& name) {

    fs::path target =
        current_directory / name;

    if (!fs::exists(target)) {
        std::cout << "File or directory does not exist.\n";
        return;
    }

    try {

        if (fs::is_directory(target)) {

            if (!fs::is_empty(target)) {
                std::cout << "Directory is not empty.\n";
                return;
            }
        }

        fs::remove(target);

        std::cout << "Removed successfully.\n";
    }
    catch (const fs::filesystem_error& e) {

        std::cout << "Remove failed: "
                  << e.what() << '\n';
    }
}


// --------------------------------------------------
// Help
// --------------------------------------------------

void show_help() {

    std::cout << "\nCommands:\n";

    std::cout << "  ls                  List directory\n";
    std::cout << "  pwd                 Show current directory\n";
    std::cout << "  cd <dir>            Enter directory\n";
    std::cout << "  back                Go to parent directory\n";

    std::cout << "  mkdir <name>        Create directory\n";
    std::cout << "  touch <name>        Create empty file\n";
    std::cout << "  rename <old> <new>  Rename file/directory\n";
    std::cout << "  cp <source> <dest>  Copy file\n";
    std::cout << "  mv <source> <dest>  Move file/directory\n";
    std::cout << "  rm <name>           Remove file/empty directory\n";

    std::cout << "  help                Show help\n";
    std::cout << "  q                   Quit\n";
}


// --------------------------------------------------
// Main
// --------------------------------------------------

int main() {

    fs::path current_directory =
        fs::current_path();

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
        std::string argument2;

        ss >> action;
        ss >> argument;
        ss >> argument2;


        // --------------------------------------------------
        // Quit
        // --------------------------------------------------

        if (action == "q") {
            break;
        }


        // --------------------------------------------------
        // List directory
        // --------------------------------------------------

        else if (action == "ls") {

            list_directory(current_directory);
        }


        // --------------------------------------------------
        // Print working directory
        // --------------------------------------------------

        else if (action == "pwd") {

            std::cout << current_directory << '\n';
        }


        // --------------------------------------------------
        // Help
        // --------------------------------------------------

        else if (action == "help") {

            show_help();
        }


        // --------------------------------------------------
        // Change directory
        // --------------------------------------------------

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

            try {

                current_directory =
                    fs::canonical(new_directory);
            }
            catch (const fs::filesystem_error& e) {

                std::cout << "Failed to change directory: "
                          << e.what() << '\n';
            }
        }


        // --------------------------------------------------
        // Go to parent directory
        // --------------------------------------------------

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


        // --------------------------------------------------
        // Make directory
        // --------------------------------------------------

        else if (action == "mkdir") {

            if (argument.empty()) {
                std::cout << "Usage: mkdir <directory>\n";
                continue;
            }

            make_directory(
                current_directory,
                argument
            );
        }


        // --------------------------------------------------
        // Create file
        // --------------------------------------------------

        else if (action == "touch") {

            if (argument.empty()) {
                std::cout << "Usage: touch <file>\n";
                continue;
            }

            create_file(
                current_directory,
                argument
            );
        }


        // --------------------------------------------------
        // Rename
        // --------------------------------------------------

        else if (action == "rename") {

            if (argument.empty() || argument2.empty()) {

                std::cout << "Usage: rename <old> <new>\n";
                continue;
            }

            rename_item(
                current_directory,
                argument,
                argument2
            );
        }


        // --------------------------------------------------
        // Copy
        // --------------------------------------------------

        else if (action == "cp") {

            if (argument.empty() || argument2.empty()) {

                std::cout
                    << "Usage: cp <source> <destination>\n";

                continue;
            }

            copy_file(
                current_directory,
                argument,
                argument2
            );
        }


        // --------------------------------------------------
        // Move
        // --------------------------------------------------

        else if (action == "mv") {

            if (argument.empty() || argument2.empty()) {

                std::cout
                    << "Usage: mv <source> <destination>\n";

                continue;
            }

            move_item(
                current_directory,
                argument,
                argument2
            );
        }


        // --------------------------------------------------
        // Remove
        // --------------------------------------------------

        else if (action == "rm") {

            if (argument.empty()) {

                std::cout
                    << "Usage: rm <file_or_directory>\n";

                continue;
            }

            remove_item(
                current_directory,
                argument
            );
        }


        // --------------------------------------------------
        // Unknown command
        // --------------------------------------------------

        else {

            std::cout
                << "Unknown command. Type 'help'.\n";
        }
    }


    std::cout << "Goodbye!\n";

    return 0;
}