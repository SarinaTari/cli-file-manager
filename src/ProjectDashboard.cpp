#include "ProjectDashboard.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

bool ProjectDashboard::is_source_file(
    const fs::path& path
) {
    const std::string extension =
        path.extension().string();

    return
        extension == ".cpp"
        || extension == ".cc"
        || extension == ".cxx"
        || extension == ".c";
}

bool ProjectDashboard::is_header_file(
    const fs::path& path
) {
    const std::string extension =
        path.extension().string();

    return
        extension == ".h"
        || extension == ".hpp"
        || extension == ".hh"
        || extension == ".hxx";
}

std::uintmax_t ProjectDashboard::calculate_size(
    const fs::path& path
) {
    std::uintmax_t total = 0;

    std::error_code error;

    if (fs::is_regular_file(path, error)) {
        return fs::file_size(path, error);
    }

    if (!fs::is_directory(path, error)) {
        return 0;
    }

    fs::recursive_directory_iterator iterator(
        path,
        fs::directory_options::skip_permission_denied,
        error
    );

    fs::recursive_directory_iterator end;

    while (iterator != end) {
        if (error) {
            error.clear();
            iterator.increment(error);
            continue;
        }

        std::error_code file_error;

        if (
            iterator->is_regular_file(file_error)
            && !file_error
        ) {
            const std::uintmax_t size =
                iterator->file_size(file_error);

            if (!file_error) {
                total += size;
            }
        }

        iterator.increment(error);
    }

    return total;
}

fs::path ProjectDashboard::find_largest_file(
    const fs::path& path
) {
    fs::path largest;

    std::uintmax_t largest_size = 0;

    std::error_code error;

    fs::recursive_directory_iterator iterator(
        path,
        fs::directory_options::skip_permission_denied,
        error
    );

    fs::recursive_directory_iterator end;

    while (iterator != end) {
        if (error) {
            error.clear();
            iterator.increment(error);
            continue;
        }

        std::error_code file_error;

        if (
            iterator->is_regular_file(file_error)
            && !file_error
        ) {
            const std::uintmax_t size =
                iterator->file_size(file_error);

            if (
                !file_error
                && size > largest_size
            ) {
                largest_size = size;
                largest = iterator->path();
            }
        }

        iterator.increment(error);
    }

    return largest;
}

std::string ProjectDashboard::detect_project_type(
    const fs::path& path
) {
    std::error_code error;

    if (fs::exists(path / "CMakeLists.txt", error)) {
        return "C++ / CMake";
    }

    if (fs::exists(path / "Makefile", error)) {
        return "C/C++ / Make";
    }

    if (fs::exists(path / "Cargo.toml", error)) {
        return "Rust / Cargo";
    }

    if (fs::exists(path / "package.json", error)) {
        return "JavaScript / Node.js";
    }

    if (
        fs::exists(path / "pyproject.toml", error)
        || fs::exists(path / "requirements.txt", error)
        || fs::exists(path / "setup.py", error)
    ) {
        return "Python";
    }

    if (fs::exists(path / "go.mod", error)) {
        return "Go";
    }

    if (fs::exists(path / "pom.xml", error)) {
        return "Java / Maven";
    }

    if (
        fs::exists(path / "build.gradle", error)
        || fs::exists(path / "build.gradle.kts", error)
    ) {
        return "Java / Gradle";
    }

    return "Unknown";
}

std::string ProjectDashboard::get_git_branch(
    const fs::path& path
) {
    const fs::path head_file =
        path / ".git" / "HEAD";

    std::ifstream file(head_file);

    if (!file) {
        return "N/A";
    }

    std::string line;

    std::getline(file, line);

    const std::string prefix =
        "ref: refs/heads/";

    if (line.rfind(prefix, 0) == 0) {
        return line.substr(prefix.length());
    }

    return "Detached HEAD";
}

bool ProjectDashboard::git_is_clean(
    const fs::path& path
) {
    const fs::path git_dir =
        path / ".git";

    if (!fs::exists(git_dir)) {
        return false;
    }

    const fs::path index_file =
        git_dir / "index";

    if (!fs::exists(index_file)) {
        return false;
    }

    const std::string command =
        "cd \"" +
        path.string() +
        "\" && git diff --quiet && git diff --cached --quiet";

    return std::system(command.c_str()) == 0;
}

std::string ProjectDashboard::format_size(
    std::uintmax_t size
) {
    const char* units[] = {
        "B",
        "KB",
        "MB",
        "GB",
        "TB"
    };

    double value =
        static_cast<double>(size);

    int unit = 0;

    while (
        value >= 1024.0
        && unit < 4
    ) {
        value /= 1024.0;
        ++unit;
    }

    if (unit == 0) {
        return std::to_string(size) + " B";
    }

    std::ostringstream output;

    output
        << std::fixed
        << std::setprecision(1)
        << value
        << " "
        << units[unit];

    return output.str();
}

void ProjectDashboard::show(
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

    std::size_t source_files = 0;
    std::size_t header_files = 0;
    std::size_t total_files = 0;

    fs::recursive_directory_iterator iterator(
        path,
        fs::directory_options::skip_permission_denied,
        error
    );

    fs::recursive_directory_iterator end;

    while (iterator != end) {
        if (error) {
            error.clear();
            iterator.increment(error);
            continue;
        }

        std::error_code file_error;

        if (
            iterator->is_regular_file(file_error)
            && !file_error
        ) {
            ++total_files;

            if (is_source_file(iterator->path())) {
                ++source_files;
            }

            if (is_header_file(iterator->path())) {
                ++header_files;
            }
        }

        iterator.increment(error);
    }

    const std::uintmax_t total_size =
        calculate_size(path);

    const fs::path largest =
        find_largest_file(path);

    std::cout
        << "========================================\n"
        << "          PROJECT DASHBOARD\n"
        << "========================================\n\n";

    std::cout
        << "Project: "
        << path.filename().string()
        << "\n";

    std::cout
        << "Type: "
        << detect_project_type(path)
        << "\n\n";

    std::cout
        << "Files:\n";

    std::cout
        << "  C/C++ source files: "
        << source_files
        << "\n";

    std::cout
        << "  C/C++ header files: "
        << header_files
        << "\n";

    std::cout
        << "  Total files: "
        << total_files
        << "\n\n";

    std::cout
        << "Storage:\n";

    std::cout
        << "  Total: "
        << format_size(total_size)
        << "\n";

    if (!largest.empty()) {
        std::error_code largest_error;

        const std::uintmax_t largest_size =
            fs::file_size(
                largest,
                largest_error
            );

        std::cout
            << "  Largest: "
            << fs::relative(
                largest,
                path
            ).string()
            << " ("
            << format_size(largest_size)
            << ")\n";
    }
    else {
        std::cout
            << "  Largest: None\n";
    }

    std::cout << "\n";

    std::cout << "Git:\n";

    const bool is_git_repository =
        fs::exists(
            path / ".git",
            error
        );

    std::cout
        << "  Repository: ";

    if (is_git_repository) {
        std::cout
            << "Yes\n";

        std::cout
            << "  Branch: "
            << get_git_branch(path)
            << "\n";

        std::cout
            << "  Status: "
            << (
                git_is_clean(path)
                    ? "Clean"
                    : "Modified"
            )
            << "\n";
    }
    else {
        std::cout
            << "No\n";
    }

    show_language_breakdown(path);

    std::cout
        << "\n========================================\n";
}

void ProjectDashboard::show_language_breakdown(
    const fs::path& path
) {
    std::map<
        std::string,
        std::size_t
    > languages;

    std::error_code error;

    fs::recursive_directory_iterator iterator(
        path,
        fs::directory_options::skip_permission_denied,
        error
    );

    fs::recursive_directory_iterator end;

    while (iterator != end) {
        if (error) {
            error.clear();
            iterator.increment(error);
            continue;
        }

        std::error_code file_error;

        if (
            iterator->is_regular_file(file_error)
            && !file_error
        ) {
            const std::string extension =
                iterator->path()
                    .extension()
                    .string();

            if (
                extension == ".cpp"
                || extension == ".cc"
                || extension == ".cxx"
            ) {
                ++languages["C++"];
            }
            else if (extension == ".c") {
                ++languages["C"];
            }
            else if (
                extension == ".h"
                || extension == ".hh"
                || extension == ".hpp"
                || extension == ".hxx"
            ) {
                ++languages["C/C++ headers"];
            }
            else if (extension == ".py") {
                ++languages["Python"];
            }
            else if (
                extension == ".js"
                || extension == ".jsx"
            ) {
                ++languages["JavaScript"];
            }
            else if (
                extension == ".ts"
                || extension == ".tsx"
            ) {
                ++languages["TypeScript"];
            }
            else if (extension == ".java") {
                ++languages["Java"];
            }
            else if (extension == ".rs") {
                ++languages["Rust"];
            }
            else if (extension == ".go") {
                ++languages["Go"];
            }
        }

        iterator.increment(error);
    }

    std::cout
        << "\nLanguages:\n";

    if (languages.empty()) {
        std::cout
            << "  No recognized source files.\n";

        return;
    }

    for (const auto& [language, count] : languages) {
        std::cout
            << "  "
            << language
            << ": "
            << count
            << "\n";
    }
}