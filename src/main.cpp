#include "CommandParser.h"
#include "FileManager.h"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

void print_help() {
    std::cout << R"(
================ CLI FILE MANAGER ================

Navigation
----------
pwd
cd <path>
back
ls
ls -a
ls --files
ls --dirs
ls --name
ls --size
ls --name-desc
ls --size-desc

File Operations
---------------
mkdir <name>
touch <name>
rename <old> <new>
cp <source> <destination>
mv <source> <destination>
rm <path>

Information
-----------
size <file>
type <path>
modified <path>
info <path>

Recursive Operations
--------------------
tree [path]
du <path>

Search
------
find <name> [path]
findext <extension> [path]
findsize <minimum_bytes> [path]

Unix Permissions
----------------
perm <path>
chmod <mode> <path>

Links
-----
link <target> <link>
ln <target> <link>
ln -s <target> <link>
readlink <path>

Other
-----
help
quit
exit
q

Examples
--------
ls -a
ls --files
find main.cpp
findext cpp
findsize 100000 .
chmod 755 script.sh
perm script.sh
ln -s file.txt shortcut.txt
readlink shortcut.txt

===================================================
)";
}

bool is_unsigned_integer(
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

int main() {
    FileManager file_manager;

    std::cout
        << "CLI File Manager\n"
        << "Type 'help' for commands.\n\n";

    while (true) {

        try {
            std::cout << "\nfm> ";

            std::string input;

            if (!std::getline(
                    std::cin,
                    input
                )) {
                std::cout
                    << "\nGoodbye.\n";
                break;
            }

            if (input.empty()) {
                continue;
            }

            Command command =
                CommandParser::parse(input);

            if (command.action.empty()) {
                continue;
            }

            const std::string& action =
                command.action;

            // =================================================
            // Exit
            // =================================================

            if (
                action == "q"
                || action == "quit"
                || action == "exit"
            ) {
                std::cout
                    << "Goodbye.\n";
                break;
            }

            // =================================================
            // Help
            // =================================================

            if (action == "help") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "help does not accept arguments."
                    );
                }

                print_help();
                continue;
            }

            // =================================================
            // pwd
            // =================================================

            if (action == "pwd") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: pwd"
                    );
                }

                file_manager.print_working_directory();
                continue;
            }

            // =================================================
            // back
            // =================================================

            if (action == "back") {
                if (!command.arguments.empty()) {
                    throw std::invalid_argument(
                        "Usage: back"
                    );
                }

                file_manager.go_back();
                continue;
            }

            // =================================================
            // cd
            // =================================================

            if (action == "cd") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: cd <path>"
                    );
                }

                file_manager.change_directory(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // ls
            // =================================================

            if (action == "ls") {

                bool show_hidden = false;
                std::string sort_option = "name";
                std::string filter = "all";

                for (
                    const std::string& argument :
                    command.arguments
                ) {

                    if (argument == "-a") {
                        show_hidden = true;
                    }
                    else if (
                        argument == "--files"
                    ) {
                        filter = "files";
                    }
                    else if (
                        argument == "--dirs"
                    ) {
                        filter = "dirs";
                    }
                    else if (
                        argument == "--name"
                    ) {
                        sort_option = "name";
                    }
                    else if (
                        argument == "--size"
                    ) {
                        sort_option = "size";
                    }
                    else if (
                        argument == "--name-desc"
                    ) {
                        sort_option = "name-desc";
                    }
                    else if (
                        argument == "--size-desc"
                    ) {
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

                continue;
            }

            // =================================================
            // mkdir
            // =================================================

            if (action == "mkdir") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: mkdir <name>"
                    );
                }

                file_manager.make_directory(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // touch
            // =================================================

            if (action == "touch") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: touch <name>"
                    );
                }

                file_manager.create_file(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // rename
            // =================================================

            if (action == "rename") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: rename <old> <new>"
                    );
                }

                file_manager.rename_item(
                    command.arguments[0],
                    command.arguments[1]
                );

                continue;
            }

            // =================================================
            // cp
            // =================================================

            if (action == "cp") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: cp <source> <destination>"
                    );
                }

                file_manager.copy_item(
                    command.arguments[0],
                    command.arguments[1]
                );

                continue;
            }

            // =================================================
            // mv
            // =================================================

            if (action == "mv") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: mv <source> <destination>"
                    );
                }

                file_manager.move_item(
                    command.arguments[0],
                    command.arguments[1]
                );

                continue;
            }

            // =================================================
            // rm
            // =================================================

            if (action == "rm") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: rm <path>"
                    );
                }

                file_manager.remove_item(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // size
            // =================================================

            if (action == "size") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: size <file>"
                    );
                }

                file_manager.show_file_size(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // type
            // =================================================

            if (action == "type") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: type <path>"
                    );
                }

                file_manager.show_file_type(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // modified
            // =================================================

            if (action == "modified") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: modified <path>"
                    );
                }

                file_manager.show_modified_time(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // info
            // =================================================

            if (action == "info") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: info <path>"
                    );
                }

                file_manager.show_info(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // tree
            // =================================================

            if (action == "tree") {

                if (command.arguments.size() > 1) {
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

                continue;
            }

            // =================================================
            // du
            // =================================================

            if (action == "du") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: du <path>"
                    );
                }

                file_manager.show_directory_size(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // find
            // =================================================

            if (action == "find") {

                if (
                    command.arguments.empty()
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

                continue;
            }

            // =================================================
            // findext
            // =================================================

            if (action == "findext") {

                if (
                    command.arguments.empty()
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

                continue;
            }

            // =================================================
            // findsize
            // =================================================

            if (action == "findsize") {

                if (
                    command.arguments.size() < 1
                    || command.arguments.size() > 2
                ) {
                    throw std::invalid_argument(
                        "Usage: findsize <minimum_bytes> [path]"
                    );
                }

                const std::string& size_text =
                    command.arguments[0];

                if (
                    !is_unsigned_integer(size_text)
                ) {
                    throw std::invalid_argument(
                        "Minimum size must be a "
                        "non-negative integer."
                    );
                }

                std::uintmax_t minimum_size;

                try {
                    minimum_size =
                        std::stoull(
                            size_text
                        );
                }
                catch (...) {
                    throw std::invalid_argument(
                        "Invalid size value."
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

                continue;
            }

            // =================================================
            // perm
            // =================================================

            if (action == "perm") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: perm <path>"
                    );
                }

                file_manager.show_permissions(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // chmod
            // =================================================

            if (action == "chmod") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: chmod <mode> <path>"
                    );
                }

                file_manager.change_permissions(
                    command.arguments[0],
                    command.arguments[1]
                );

                continue;
            }

            // =================================================
            // link
            // =================================================

            if (action == "link") {
                if (command.arguments.size() != 2) {
                    throw std::invalid_argument(
                        "Usage: link <target> <link>"
                    );
                }

                file_manager.create_hard_link(
                    command.arguments[0],
                    command.arguments[1]
                );

                continue;
            }

            // =================================================
            // ln
            // =================================================

            if (action == "ln") {

                if (
                    command.arguments.size() == 2
                ) {
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
                        "Usage:\n"
                        "  ln <target> <link>\n"
                        "  ln -s <target> <link>"
                    );
                }

                continue;
            }

            // =================================================
            // readlink
            // =================================================

            if (action == "readlink") {
                if (command.arguments.size() != 1) {
                    throw std::invalid_argument(
                        "Usage: readlink <path>"
                    );
                }

                file_manager.show_link_target(
                    command.arguments[0]
                );

                continue;
            }

            // =================================================
            // Unknown command
            // =================================================

            std::cout
                << "Unknown command: "
                << action
                << '\n'
                << "Type 'help' to see available commands.\n";
        }
        catch (const std::invalid_argument& error) {
            std::cout
                << "Error: "
                << error.what()
                << '\n';
        }
        catch (const fs::filesystem_error& error) {
            std::cout
                << "Filesystem error: "
                << error.what()
                << '\n';
        }
        catch (const std::runtime_error& error) {
            std::cout
                << "Error: "
                << error.what()
                << '\n';
        }
        catch (const std::exception& error) {
            std::cout
                << "Unexpected error: "
                << error.what()
                << '\n';
        }
    }

    return 0;
}