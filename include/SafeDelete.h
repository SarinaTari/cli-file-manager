#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

class SafeDelete {
public:
    static void remove(
        const std::filesystem::path& path
    );

private:
    struct DeleteInfo {
        std::size_t files = 0;
        std::size_t directories = 0;
        std::uintmax_t total_size = 0;
    };

    static DeleteInfo analyze(
        const std::filesystem::path& path
    );

    static void collect_paths(
        const std::filesystem::path& path,
        std::vector<std::filesystem::path>& paths
    );

    static std::string format_size(
        std::uintmax_t size
    );

    static bool ask_confirmation();

    static bool is_dangerous_path(
        const std::filesystem::path& path
    );
};