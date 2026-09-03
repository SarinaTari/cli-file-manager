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
    void list_directory(
        bool show_hidden = false,
        const std::string& sort_option = "name",
        const std::string& filter = "all"
    ) const;

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

    // Search
    void find_by_name(
        const std::string& name,
        const std::string& path = "."
    ) const;

    void find_by_extension(
        const std::string& extension,
        const std::string& path = "."
    ) const;

    void find_by_size(
        std::uintmax_t minimum_size,
        const std::string& path = "."
    ) const;

    // Unix features
    void show_permissions(const std::string& name) const;

    void change_permissions(
        const std::string& mode,
        const std::string& name
    ) const;

    void create_hard_link(
        const std::string& target,
        const std::string& link_name
    ) const;

    void create_symbolic_link(
        const std::string& target,
        const std::string& link_name
    ) const;

    void show_link_target(const std::string& name) const;

private:
    std::string format_time(
        const fs::file_time_type& file_time
    ) const;

    fs::path resolve_path(const std::string& path) const;

    std::string permission_string(
        const fs::path& path
    ) const;

    bool is_hidden(const fs::path& path) const;

    void print_tree_recursive(
        const fs::path& path,
        const std::string& prefix
    ) const;

    std::uintmax_t calculate_directory_size(
        const fs::path& path
    ) const;

    // Phase 9 safety helpers
    void ensure_exists(
        const fs::path& path,
        const std::string& description
    ) const;

    void ensure_not_exists(
        const fs::path& path,
        const std::string& description
    ) const;

    void ensure_not_dangerous_path(
        const fs::path& path
    ) const;

    void ensure_not_inside(
        const fs::path& source,
        const fs::path& destination
    ) const;

    bool is_same_path(
        const fs::path& first,
        const fs::path& second
    ) const;
};