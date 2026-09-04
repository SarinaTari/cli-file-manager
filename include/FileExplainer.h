#pragma once

#include <filesystem>

class FileExplainer {
public:
    static void explain(
        const std::filesystem::path& path
    );

private:
    static std::string detect_file_role(
        const std::filesystem::path& path
    );

    static std::string detect_language(
        const std::filesystem::path& path
    );

    static std::filesystem::path find_related_header(
        const std::filesystem::path& path
    );

    static std::string get_git_status(
        const std::filesystem::path& path
    );

    static bool is_git_tracked(
        const std::filesystem::path& path
    );

    static std::string format_size(
        std::uintmax_t size
    );
};