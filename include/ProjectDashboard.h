#pragma once

#include <cstdint>
#include <filesystem>

class ProjectDashboard {
public:
    static void show(
        const std::filesystem::path& path
    );

private:
    static bool is_source_file(
        const std::filesystem::path& path
    );

    static bool is_header_file(
        const std::filesystem::path& path
    );

    static std::uintmax_t calculate_size(
        const std::filesystem::path& path
    );

    static std::filesystem::path find_largest_file(
        const std::filesystem::path& path
    );

    static std::string detect_project_type(
        const std::filesystem::path& path
    );

    static std::string get_git_branch(
        const std::filesystem::path& path
    );

    static bool git_is_clean(
        const std::filesystem::path& path
    );

    static std::string format_size(
        std::uintmax_t size
    );

    static void show_language_breakdown(
        const std::filesystem::path& path
    );
};