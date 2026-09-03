#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class FileManager {
private:
    fs::path current_directory;

public:
    FileManager();

    void list_directory() const;
    void print_working_directory() const;

    void change_directory(const std::string& name);
    void go_back();

    void make_directory(const std::string& name);
    void create_file(const std::string& name);

    void rename_item(
        const std::string& old_name,
        const std::string& new_name
    );

    void copy_file(
        const std::string& source,
        const std::string& destination
    );

    void move_item(
        const std::string& source,
        const std::string& destination
    );

    void remove_item(const std::string& name);

    void show_file_size(const std::string& name) const;
    void show_file_type(const std::string& name) const;
    void show_modified_time(const std::string& name) const;
    void show_info(const std::string& name) const;

private:
    std::string format_time(
        const fs::file_time_type& file_time
    ) const;
};