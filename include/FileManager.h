#pragma once

#include <cstdint>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class FileManager {
private:
    fs::path current_directory;

public:
    FileManager();

    // Navigation
    void list_directory() const;
    void print_working_directory() const;
    void change_directory(const std::string& name);
    void go_back();

    // Basic filesystem operations
    void make_directory(const std::string& name);
    void create_file(const std::string& name);

    void rename_item(
        const std::string& old_name,
        const std::string& new_name
    );

    void copy_item(
        const std::string& source,
        const std::string& destination
    );

    void move_item(
        const std::string& source,
        const std::string& destination
    );

    void remove_item(const std::string& name);

    // File information
    void show_file_size(const std::string& name) const;
    void show_file_type(const std::string& name) const;
    void show_modified_time(const std::string& name) const;
    void show_info(const std::string& name) const;

    // Recursive operations
    void show_tree(const std::string& name = ".") const;
    void show_directory_size(const std::string& name) const;

private:
    std::string format_time(
        const fs::file_time_type& file_time
    ) const;

    fs::path resolve_path(
        const std::string& path
    ) const;

    void print_tree_recursive(
        const fs::path& path,
        const std::string& prefix
    ) const;

    std::uintmax_t calculate_directory_size(
        const fs::path& path
    ) const;
};