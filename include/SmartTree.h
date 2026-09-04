#pragma once

#include <filesystem>
#include <string>

class SmartTree {
public:
    static void show(
        const std::filesystem::path& path
    );

private:
    static std::string classify_directory(
        const std::filesystem::path& path
    );

    static std::string classify_file(
        const std::filesystem::path& path
    );

    static std::string classify_path(
        const std::filesystem::path& path
    );

    static void print_tree(
        const std::filesystem::path& path,
        const std::string& prefix,
        bool is_last,
        bool is_root
    );
};