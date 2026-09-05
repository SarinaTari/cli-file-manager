#include "TerminalUI.h"

#include "CommandParser.h"

#include <algorithm>
#include <iostream>

TerminalUI::TerminalUI(FileManager& manager)
    : file_manager(manager) {
}

void TerminalUI::run() {
    hide_cursor();

    refresh();

    while (running) {
        render();
        handle_input();
    }

    clear_screen();
    move_cursor_home();
    show_cursor();
}

void TerminalUI::refresh() {
    entries.clear();

    fs::path current =
        file_manager.get_current_directory();

    std::error_code ec;

    fs::directory_iterator iterator(
        current,
        fs::directory_options::skip_permission_denied,
        ec
    );

    if (ec) {
        status_message =
            "Unable to read directory: "
            + ec.message();

        return;
    }

    for (
        ;
        iterator != fs::directory_iterator();
        iterator.increment(ec)
    ) {
        if (ec) {
            ec.clear();
            continue;
        }

        entries.push_back(*iterator);
    }

    std::sort(
        entries.begin(),
        entries.end(),
        [](const fs::directory_entry& a,
           const fs::directory_entry& b) {

            std::error_code a_ec;
            std::error_code b_ec;

            bool a_directory =
                a.is_directory(a_ec);

            bool b_directory =
                b.is_directory(b_ec);

            if (a_directory != b_directory) {
                return a_directory > b_directory;
            }

            return a.path().filename().string()
                < b.path().filename().string();
        }
    );

    if (entries.empty()) {
        selected_index = 0;
    }
    else if (selected_index >= entries.size()) {
        selected_index = entries.size() - 1;
    }
}

void TerminalUI::render() {
    clear_screen();
    move_cursor_home();

    render_header();
    render_entries();
    render_info();
    render_footer();
}

void TerminalUI::render_header() {
    std::cout
        << "╔══════════════════════════════════════════════════════════════╗\n";

    std::cout
        << "║                    CLI FILE MANAGER                         ║\n";

    std::cout
        << "╠══════════════════════════════════════════════════════════════╣\n";

    std::cout
        << "║ "
        << file_manager.get_current_directory().string()
        << "\n";

    std::cout
        << "╠══════════════════════════════════════════════════════════════╣\n";
}

void TerminalUI::render_entries() {
    if (entries.empty()) {
        std::cout << "║\n";
        std::cout << "║   Directory is empty.\n";
        return;
    }

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];

        std::string marker =
            (i == selected_index) ? "> " : "  ";

        std::cout
            << "║ "
            << marker
            << format_entry(entry)
            << "\n";
    }
}

void TerminalUI::render_info() {
    std::cout
        << "╠══════════════════════════════════════════════════════════════╣\n";

    if (entries.empty()) {
        std::cout
            << "║ No item selected.\n";

        return;
    }

    const auto& entry =
        entries[selected_index];

    std::cout
        << "║ Selected: "
        << entry.path().filename().string()
        << "\n";

    std::cout
        << "║ Type: "
        << entry_type(entry)
        << "\n";

    std::error_code ec;

    if (entry.is_regular_file(ec)) {
        auto size =
            entry.file_size(ec);

        if (!ec) {
            std::cout
                << "║ Size: "
                << size
                << " bytes\n";
        }
    }
}

void TerminalUI::render_footer() {
    std::cout
        << "╠══════════════════════════════════════════════════════════════╣\n";

    if (command_mode) {
        std::cout
            << "║ : "
            << command_buffer
            << "\n";
    }
    else {
        std::cout
            << "║ ↑ ↓ Navigate   Enter Open   h Back   : Command   q Quit\n";
    }

    if (!status_message.empty()) {
        std::cout
            << "║ "
            << status_message
            << "\n";
    }

    std::cout
        << "╚══════════════════════════════════════════════════════════════╝\n";

    std::cout.flush();
}

void TerminalUI::handle_input() {
    char key;

    if (!std::cin.get(key)) {
        running = false;
        return;
    }

    if (command_mode) {
        handle_command_mode(key);
    }
    else {
        handle_normal_mode(key);
    }
}

void TerminalUI::handle_normal_mode(char key) {
    status_message.clear();

    if (key == 'q') {
        running = false;
        return;
    }

    if (key == ':') {
        command_mode = true;
        command_buffer.clear();
        return;
    }

    if (key == '\n') {
        open_selected();
        return;
    }

    if (key == 'h') {
        go_back();
        return;
    }

    if (key == '\033') {
        char bracket;

        if (!std::cin.get(bracket)) {
            return;
        }

        if (bracket != '[') {
            return;
        }

        char direction;

        if (!std::cin.get(direction)) {
            return;
        }

        if (direction == 'A') {
            if (selected_index > 0) {
                --selected_index;
            }
        }
        else if (direction == 'B') {
            if (
                !entries.empty()
                && selected_index + 1 < entries.size()
            ) {
                ++selected_index;
            }
        }

        return;
    }
}

void TerminalUI::handle_command_mode(char key) {
    if (key == '\n') {
        execute_command();
        return;
    }

    if (key == 27) {
        command_mode = false;
        command_buffer.clear();
        return;
    }

    if (
        key == 127
        || key == '\b'
    ) {
        if (!command_buffer.empty()) {
            command_buffer.pop_back();
        }

        return;
    }

    if (
        key >= 32
        && key <= 126
    ) {
        command_buffer += key;
    }
}

void TerminalUI::open_selected() {
    if (entries.empty()) {
        return;
    }

    const auto& entry =
        entries[selected_index];

    std::error_code ec;

    if (entry.is_directory(ec)) {
        try {
            file_manager.change_directory(
                entry.path().string()
            );

            selected_index = 0;

            refresh();
        }
        catch (const std::exception& exception) {
            status_message =
                std::string("Error: ")
                + exception.what();
        }
    }
    else {
        status_message =
            "Selected: "
            + entry.path().filename().string();
    }
}

void TerminalUI::go_back() {
    try {
        fs::path current =
            file_manager.get_current_directory();

        fs::path parent =
            current.parent_path();

        if (parent == current) {
            status_message =
                "Already at filesystem root.";

            return;
        }

        file_manager.go_back();

        selected_index = 0;

        refresh();
    }
    catch (const std::exception& exception) {
        status_message =
            std::string("Error: ")
            + exception.what();
    }
}

void TerminalUI::execute_command() {
    if (command_buffer.empty()) {
        command_mode = false;
        return;
    }

    try {
        Command command =
            CommandParser::parse(
                command_buffer
            );

        if (command.action.empty()) {
            command_mode = false;
            command_buffer.clear();
            return;
        }

        if (
            command.action == "q"
            || command.action == "quit"
            || command.action == "exit"
        ) {
            running = false;
            return;
        }

        if (
            command.action == "pwd"
            && command.arguments.empty()
        ) {
            status_message =
                file_manager
                    .get_current_directory()
                    .string();
        }
        else if (
            command.action == "cd"
            && command.arguments.size() == 1
        ) {
            file_manager.change_directory(
                command.arguments[0]
            );

            selected_index = 0;

            refresh();

            status_message =
                "Directory changed.";
        }
        else if (
            command.action == "back"
            && command.arguments.empty()
        ) {
            file_manager.go_back();

            selected_index = 0;

            refresh();

            status_message =
                "Moved to parent directory.";
        }
        else if (
            command.action == "mkdir"
            && command.arguments.size() == 1
        ) {
            file_manager.make_directory(
                command.arguments[0]
            );

            refresh();

            status_message =
                "Directory created.";
        }
        else if (
            command.action == "touch"
            && command.arguments.size() == 1
        ) {
            file_manager.create_file(
                command.arguments[0]
            );

            refresh();

            status_message =
                "File created.";
        }
        else if (
            command.action == "rm"
            && command.arguments.size() == 1
        ) {
            file_manager.remove_item(
                command.arguments[0]
            );

            refresh();

            status_message =
                "Item removed.";
        }
        else if (
            command.action == "rename"
            && command.arguments.size() == 2
        ) {
            file_manager.rename_item(
                command.arguments[0],
                command.arguments[1]
            );

            refresh();

            status_message =
                "Item renamed.";
        }
        else if (
            command.action == "cp"
            && command.arguments.size() == 2
        ) {
            file_manager.copy_item(
                command.arguments[0],
                command.arguments[1]
            );

            refresh();

            status_message =
                "Item copied.";
        }
        else if (
            command.action == "mv"
            && command.arguments.size() == 2
        ) {
            file_manager.move_item(
                command.arguments[0],
                command.arguments[1]
            );

            refresh();

            status_message =
                "Item moved.";
        }
        else if (
            command.action == "info"
            && command.arguments.size() == 1
        ) {
            file_manager.show_info(
                command.arguments[0]
            );
        }
        else if (
            command.action == "perm"
            && command.arguments.size() == 1
        ) {
            file_manager.show_permissions(
                command.arguments[0]
            );
        }
        else if (
            command.action == "tree"
            && (
                command.arguments.empty()
                || command.arguments.size() == 1
            )
        ) {
            if (command.arguments.empty()) {
                file_manager.show_tree();
            }
            else {
                file_manager.show_tree(
                    command.arguments[0]
                );
            }
        }
        else if (
            command.action == "du"
            && command.arguments.size() == 1
        ) {
            file_manager.show_directory_size(
                command.arguments[0]
            );
        }
        else {
            status_message =
                "Unknown or invalid command.";
        }
    }
    catch (const std::exception& exception) {
        status_message =
            std::string("Error: ")
            + exception.what();
    }

    command_buffer.clear();
    command_mode = false;

    refresh();
}

void TerminalUI::clear_screen() {
    std::cout << "\033[2J";
}

void TerminalUI::move_cursor_home() {
    std::cout << "\033[H";
}

void TerminalUI::hide_cursor() {
    std::cout << "\033[?25l";
}

void TerminalUI::show_cursor() {
    std::cout << "\033[?25h";
}

std::string TerminalUI::format_entry(
    const fs::directory_entry& entry
) const {
    std::string name =
        entry.path().filename().string();

    std::error_code ec;

    if (entry.is_directory(ec)) {
        return "[DIR]  " + name + "/";
    }

    if (entry.is_symlink(ec)) {
        return "[LINK] " + name;
    }

    return "[FILE]  " + name;
}

std::string TerminalUI::entry_type(
    const fs::directory_entry& entry
) const {
    std::error_code ec;

    if (entry.is_directory(ec)) {
        return "Directory";
    }

    if (entry.is_symlink(ec)) {
        return "Symbolic Link";
    }

    if (entry.is_regular_file(ec)) {
        return "Regular File";
    }

    return "Other";
}