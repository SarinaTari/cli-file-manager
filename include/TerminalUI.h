#pragma once

#include "FileManager.h"

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class TerminalUI {
private:
    FileManager& file_manager;

    std::vector<fs::directory_entry> entries;

    std::size_t selected_index = 0;

    bool running = true;

    bool command_mode = false;

    std::string command_buffer;

    std::string status_message;

public:
    explicit TerminalUI(FileManager& manager);

    void run();

private:
    void refresh();

    void render();

    void render_header();

    void render_entries();

    void render_info();

    void render_footer();

    void handle_input();

    void handle_normal_mode(char key);

    void handle_command_mode(char key);

    void open_selected();

    void go_back();

    void execute_command();

    void clear_screen();

    void move_cursor_home();

    void hide_cursor();

    void show_cursor();

    std::string format_entry(
        const fs::directory_entry& entry
    ) const;

    std::string entry_type(
        const fs::directory_entry& entry
    ) const;
};