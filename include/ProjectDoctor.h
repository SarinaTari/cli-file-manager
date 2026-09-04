#pragma once

#include <filesystem>
#include <string>

class ProjectDoctor {
public:
    static void check(
        const std::filesystem::path& path
    );

private:
    struct CheckResult {
        std::string name;
        std::string status;
    };

    static CheckResult check_file(
        const std::filesystem::path& path,
        const std::string& name,
        bool warning_if_missing
    );

    static CheckResult check_directory(
        const std::filesystem::path& path,
        const std::string& name,
        bool warning_if_missing
    );

    static CheckResult check_git(
        const std::filesystem::path& path
    );

    static CheckResult check_git_status(
        const std::filesystem::path& path
    );

    static CheckResult check_build_directory(
        const std::filesystem::path& path
    );

    static void print_result(
        const CheckResult& result
    );
};