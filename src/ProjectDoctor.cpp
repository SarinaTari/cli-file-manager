#include "ProjectDoctor.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

ProjectDoctor::CheckResult ProjectDoctor::check_file(
    const fs::path& path,
    const std::string& name,
    bool warning_if_missing
) {
    std::error_code error;

    if (fs::is_regular_file(path, error)) {
        return {
            name,
            "[OK]"
        };
    }

    if (warning_if_missing) {
        return {
            name,
            "[WARN]"
        };
    }

    return {
        name,
        "[ERROR]"
    };
}

ProjectDoctor::CheckResult ProjectDoctor::check_directory(
    const fs::path& path,
    const std::string& name,
    bool warning_if_missing
) {
    std::error_code error;

    if (fs::is_directory(path, error)) {
        return {
            name,
            "[OK]"
        };
    }

    if (warning_if_missing) {
        return {
            name,
            "[WARN]"
        };
    }

    return {
        name,
        "[ERROR]"
    };
}

ProjectDoctor::CheckResult ProjectDoctor::check_git(
    const fs::path& path
) {
    std::error_code error;

    if (fs::is_directory(path / ".git", error)) {
        return {
            "Git repository detected",
            "[OK]"
        };
    }

    return {
        "Git repository detected",
        "[WARN]"
    };
}

ProjectDoctor::CheckResult ProjectDoctor::check_git_status(
    const fs::path& path
) {
    std::error_code error;

    if (!fs::is_directory(path / ".git", error)) {
        return {
            "Git working tree",
            "[WARN]"
        };
    }

    const std::string command =
        "cd \"" +
        path.string() +
        "\" && git diff --quiet && git diff --cached --quiet";

    const int result =
        std::system(command.c_str());

    if (result == 0) {
        return {
            "Git working tree clean",
            "[OK]"
        };
    }

    return {
        "Git working tree clean",
        "[WARN]"
    };
}

ProjectDoctor::CheckResult ProjectDoctor::check_build_directory(
    const fs::path& path
) {
    std::error_code error;

    if (fs::is_directory(path / "build", error)) {
        return {
            "Build directory present",
            "[INFO]"
        };
    }

    return {
        "Build directory present",
        "[INFO]"
    };
}

void ProjectDoctor::print_result(
    const CheckResult& result
) {
    std::cout
        << result.status
        << " "
        << result.name
        << "\n";
}

void ProjectDoctor::check(
    const fs::path& path
) {
    std::error_code error;

    if (!fs::exists(path, error)) {
        throw std::runtime_error(
            "Path does not exist: " +
            path.string()
        );
    }

    if (!fs::is_directory(path, error)) {
        throw std::runtime_error(
            "Project path must be a directory."
        );
    }

    std::vector<CheckResult> results;

    results.push_back(
        check_file(
            path / "CMakeLists.txt",
            "CMakeLists.txt found",
            false
        )
    );

    results.push_back(
        check_directory(
            path / "src",
            "src directory found",
            false
        )
    );

    results.push_back(
        check_directory(
            path / "include",
            "include directory found",
            false
        )
    );

    results.push_back(
        check_directory(
            path / "tests",
            "tests directory found",
            true
        )
    );

    results.push_back(
        check_file(
            path / "README.md",
            "README.md found",
            true
        )
    );

    results.push_back(
        check_git(path)
    );

    results.push_back(
        check_git_status(path)
    );

    results.push_back(
        check_build_directory(path)
    );

    std::size_t passed = 0;
    std::size_t warnings = 0;
    std::size_t errors = 0;

    std::cout
        << "========================================\n"
        << "        PROJECT HEALTH CHECK\n"
        << "========================================\n\n";

    std::cout
        << "Project: "
        << path.filename().string()
        << "\n\n";

    for (const CheckResult& result : results) {
        print_result(result);

        if (result.status == "[OK]") {
            ++passed;
        }
        else if (result.status == "[WARN]") {
            ++warnings;
        }
        else if (result.status == "[ERROR]") {
            ++errors;
        }
    }

    std::cout
        << "\nSummary:\n";

    std::cout
        << "  Checks: "
        << results.size()
        << "\n";

    std::cout
        << "  Passed: "
        << passed
        << "\n";

    std::cout
        << "  Warnings: "
        << warnings
        << "\n";

    std::cout
        << "  Errors: "
        << errors
        << "\n";

    std::cout
        << "\n========================================\n";
}