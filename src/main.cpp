#include "CommandParser.h"
#include "DuplicateDetector.h"
#include "FileManager.h"
#include "StorageAnalyzer.h"
#include "TerminalUI.h"

#include <cctype>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

bool is_unsigned_integer(
    const std::string& value
) {
    if (value.empty()) {
        return false;
    }

    for (char character : value) {
        if (
            !std::isdigit(
                static_cast<unsigned char>(character)
            )
        ) {
            return false;
        }
    }

    return true;
}

void print_help() {
    std::cout
        << "\nAvailable commands:\n\n"

        << "Navigation:\n"
        << "  ls [options] [filter]       List directory contents\n"
        << "  pwd                         Show current directory\n"
        << "  cd <directory>              Change directory\n"
        << "  back                        Go to parent directory\n\n"

        << "File operations:\n"
        << "  mkdir <name>                Create directory\n"
        << "  touch <name>                Create file\n"
        << "  rename <old> <new>          Rename item\n"
        << "  cp <source> <destination>   Copy item\n"
        << "  mv <source> <destination>   Move item\n"
        << "  rm <name>                   Remove item\n\n"

        << "Information:\n"
        << "  size <name>                 Show file size\n"
        << "  type <name>                 Show file type\n"
        << "  modified <name>             Show modification time\n"
        << "  info <name>                 Show detailed information\n"
        << "  tree [name]                 Show directory tree\n"
        << "  du [name]                   Show directory size\n\n"

        << "Search:\n"
        << "  find <name> [path]          Find by name\n"
        << "  findext <extension> [path]  Find by extension\n"
        << "  findsize <size> [path]      Find files above size\n\n"

        << "Permissions and links:\n"
        << "  perm <name>                 Show permissions\n"
        << "  chmod <mode> <name>         Change permissions\n"
        << "  ln <target> <link>          Create hard link\n"
        << "  symlink <target> <link>     Create symbolic link\n"
        << "  linktarget <name>           Show symbolic link target\n\n"

        << "Phase 12 - Advanced features:\n"
        << "  analyze [path]              Analyze storage usage\n"
        << "  duplicates [path]           Find duplicate files\n\n"

        << "Interface:\n"
        << "  ui                          Launch interactive terminal UI\n"
        << "  help                        Show this help\n"
        << "  q                           Exit the program\n"
        << "  quit                        Exit the program\n"
        << "  exit                        Exit the program\n\n";
}

void run_command_mode(
    FileManager& file_manager
) {
    std::string input;

    std::cout
        << "CLI File Manager\n";

    std::cout
        << "Type 'help' for available commands.\n\n";

    while (true) {
        std::cout
            << "> ";

        if (!std::getline(std::cin, input)) {
            break;
        }

        try {
            Command command =
                CommandParser::parse(input);

            if (command.action.empty()) {
                continue;
            }

            /*
             * Quit commands
             */
            if (
                command.action == "q"
                || command.action == "quit"
                || command.action == "exit"
            ) {
                break;
            }

            /*
             * Help
             */
            else if (command.action == "help") {
                if (!command.arguments.empty()) {
                    std::cout
                        << "Usage: help\n";
                    continue;
                }

                print_help();
            }

            /*
             * pwd
             */
            else if (command.action == "pwd") {
                if (!command.arguments.empty()) {
                    std::cout
                        << "Usage: pwd\n";
                    continue;
                }

                file_manager.print_working_directory();
            }

            /*
             * ls
             */
            else if (command.action == "ls") {
                bool show_hidden = false;
                std::string sort_option = "name";
                std::string filter = "all";

                for (
                    const auto& argument
                    : command.arguments
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
                        std::cout
                            << "Invalid ls option: "
                            << argument
                            << "\n";

                        continue;
                    }
                }

                file_manager.list_directory(
                    show_hidden,
                    sort_option,
                    filter
                );
            }

            /*
             * cd
             */
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

            /*
             * back
             */
            else if (command.action == "back") {
                if (!command.arguments.empty()) {
                    std::cout
                        << "Usage: back\n";
                    continue;
                }

                file_manager.go_back();
            }

            /*
             * mkdir
             */
            else if (command.action == "mkdir") {
                if (command.arguments.size() != 1) {
                    std::cout
                        << "Usage: mkdir <name>\n";
                    continue;
                }

                file_manager.make_directory(
                    command.arguments[0]
                );
            }

            /*
             * touch
             */
            else if (command.action == "touch") {
                if (command.arguments.size() != 1) {
                    std::cout
                        << "Usage: touch <name>\n";
                    continue;
                }

                file_manager.create_file(
                    command.arguments[0]
                );
            }

            /*
             * rename
             */
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

            /*
             * cp
             */
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

            /*
             * mv
             */
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

            /*
             * rm
             */
            else if (command.action == "rm") {
                if (command.arguments.size() != 1) {
                    std::cout
                        << "Usage: rm <name>\n";
                    continue;
                }

                file_manager.remove_item(
                    command.arguments[0]
                );
            }

            /*
             * size
             */
            else if (command.action == "size") {
                if (command.arguments.size() != 1) {
                    std::cout
                        << "Usage: size <name>\n";
                    continue;
                }

                file_manager.show_file_size(
                    command.arguments[0]
                );
            }

            /*
             * type
             */
            else if (command.action == "type") {
                if (command.arguments.size() != 1) {
                    std::cout
                        << "Usage: type <name>\n";
                    continue;
                }

                file_manager.show_file_type(
                    command.arguments[0]
                );
            }

            /*
             * modified
             */
            else if (command.action == "modified") {
                if (command.arguments.size() != 1) {
                    std::cout
                        << "Usage: modified <name>\n";
                    continue;
                }

                file_manager.show_modified_time(
                    command.arguments[0]
                );
            }

            /*
             * info
             */
            else if (command.action == "info") {
                if (command.arguments.size() != 1) {
                    std::cout
                        << "Usage: info <name>\n";
                    continue;
                }

                file_manager.show_info(
                    command.arguments[0]
                );
            }

            /*
             * tree
             */
            else if (command.action == "tree") {
                if (command.arguments.size() > 1) {
                    std::cout
                        << "Usage: tree [name]\n";
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

            /*
             * du
             */
            else if (command.action == "du") {
                if (command.arguments.size() > 1) {
                    std::cout
                        << "Usage: du [name]\n";
                    continue;
                }

                if (command.arguments.empty()) {
                    file_manager.show_directory_size(".");
                }
                else {
                    file_manager.show_directory_size(
                        command.arguments[0]
                    );
                }
            }

            /*
             * find
             */
            else if (command.action == "find") {
                if (
                    command.arguments.empty()
                    || command.arguments.size() > 2
                ) {
                    std::cout
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

            /*
             * findext
             */
            else if (command.action == "findext") {
                if (
                    command.arguments.empty()
                    || command.arguments.size() > 2
                ) {
                    std::cout
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

            /*
             * findsize
             */
            else if (command.action == "findsize") {
                if (
                    command.arguments.empty()
                    || command.arguments.size() > 2
                ) {
                    std::cout
                        << "Usage: findsize <minimum-size> [path]\n";
                    continue;
                }

                if (
                    !is_unsigned_integer(
                        command.arguments[0]
                    )
                ) {
                    std::cout
                        << "Error: Minimum size must be "
                           "a non-negative integer.\n";

                    continue;
                }

                std::uintmax_t minimum_size = 0;

                try {
                    minimum_size =
                        std::stoull(
                            command.arguments[0]
                        );
                }
                catch (...) {
                    std::cout
                        << "Error: Minimum size must be "
                           "a non-negative integer.\n";

                    continue;
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

            /*
             * perm
             */
            else if (command.action == "perm") {
                if (command.arguments.size() != 1) {
                    std::cout
                        << "Usage: perm <name>\n";
                    continue;
                }

                file_manager.show_permissions(
                    command.arguments[0]
                );
            }

            /*
             * chmod
             */
            else if (command.action == "chmod") {
                if (command.arguments.size() != 2) {
                    std::cout
                        << "Usage: chmod <mode> <name>\n";
                    continue;
                }

                file_manager.change_permissions(
                    command.arguments[0],
                    command.arguments[1]
                );
            }

            /*
             * hard link
             */
            else if (command.action == "ln") {
                if (command.arguments.size() != 2) {
                    std::cout
                        << "Usage: ln <target> <link>\n";
                    continue;
                }

                file_manager.create_hard_link(
                    command.arguments[0],
                    command.arguments[1]
                );
            }

            /*
             * symbolic link
             */
            else if (command.action == "symlink") {
                if (command.arguments.size() != 2) {
                    std::cout
                        << "Usage: symlink <target> <link>\n";
                    continue;
                }

                file_manager.create_symbolic_link(
                    command.arguments[0],
                    command.arguments[1]
                );
            }

            /*
             * linktarget
             */
            else if (command.action == "linktarget") {
                if (command.arguments.size() != 1) {
                    std::cout
                        << "Usage: linktarget <name>\n";
                    continue;
                }

                file_manager.show_link_target(
                    command.arguments[0]
                );
            }

            /*
             * analyze
             */
            else if (command.action == "analyze") {
                if (command.arguments.size() > 1) {
                    std::cout
                        << "Usage: analyze [path]\n";
                    continue;
                }

                std::string path =
                    command.arguments.empty()
                    ? "."
                    : command.arguments[0];

                try {
                    fs::path resolved_path =
                        file_manager.get_current_directory()
                        / path;

                    StorageAnalyzer::analyze(
                        resolved_path
                    );
                }
                catch (
                    const std::exception& error
                ) {
                    std::cout
                        << "Error: "
                        << error.what()
                        << "\n";
                }
            }

            /*
             * duplicates
             */
            else if (command.action == "duplicates") {
                if (command.arguments.size() > 1) {
                    std::cout
                        << "Usage: duplicates [path]\n";
                    continue;
                }

                std::string path =
                    command.arguments.empty()
                    ? "."
                    : command.arguments[0];

                try {
                    fs::path resolved_path =
                        file_manager.get_current_directory()
                        / path;

                    DuplicateDetector::find_duplicates(
                        resolved_path
                    );
                }
                catch (
                    const std::exception& error
                ) {
                    std::cout
                        << "Error: "
                        << error.what()
                        << "\n";
                }
            }

            /*
             * Terminal UI
             */
            else if (command.action == "ui") {
                if (!command.arguments.empty()) {
                    std::cout
                        << "Usage: ui\n";
                    continue;
                }

                TerminalUI ui(file_manager);
                ui.run();
            }

            /*
             * Unknown command
             */
            else {
                std::cout
                    << "Unknown command: "
                    << command.action
                    << "\n";

                std::cout
                    << "Type 'help' to see "
                       "available commands.\n";
            }
        }
        catch (
            const std::exception& error
        ) {
            std::cout
                << "Error: "
                << error.what()
                << "\n";
        }
    }
}

} // namespace

int main() {
    FileManager file_manager;

    run_command_mode(
        file_manager
    );

    return 0;
}