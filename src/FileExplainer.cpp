#include "FileExplainer.h"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace fs = std::filesystem;

std::string FileExplainer::detect_file_role(
    const fs::path& path
) {
    const std::string extension =
        path.extension().string();

    const std::string filename =
        path.filename().string();

    if (
        extension == ".cpp"
        || extension == ".cc"
        || extension == ".cxx"
        || extension == ".c"
    ) {
        return "Source code";
    }

    if (
        extension == ".h"
        || extension == ".hpp"
        || extension == ".hh"
        || extension == ".hxx"
    ) {
        return "Header / interface";
    }

    if (filename == "CMakeLists.txt") {
        return "Build configuration";
    }

    if (
        filename == "Makefile"
        || filename == "makefile"
    ) {
        return "Build configuration";
    }

    if (extension == ".py") {
        return "Python source code";
    }

    if (extension == ".js") {
        return "JavaScript source code";
    }

    if (extension == ".ts") {
        return "TypeScript source code";
    }

    if (extension == ".rs") {
        return "Rust source code";
    }

    if (extension == ".go") {
        return "Go source code";
    }

    if (
        filename == "README"
        || filename == "README.md"
        || filename == "README.txt"
    ) {
        return "Project documentation";
    }

    if (
        filename == ".gitignore"
        || filename == ".gitattributes"
    ) {
        return "Git configuration";
    }

    if (extension == ".json") {
        return "Configuration / data";
    }

    if (
        extension == ".yaml"
        || extension == ".yml"
    ) {
        return "Configuration";
    }

    if (extension == ".xml") {
        return "Configuration / data";
    }

    if (
        extension == ".sh"
        || extension == ".bash"
    ) {
        return "Shell script";
    }

    if (extension == ".sql") {
        return "SQL source";
    }

    if (extension == ".txt") {
        return "Text document";
    }

    return "General file";
}

std::string FileExplainer::detect_language(
    const fs::path& path
) {
    const std::string extension =
        path.extension().string();

    if (
        extension == ".cpp"
        || extension == ".cc"
        || extension == ".cxx"
    ) {
        return "C++";
    }

    if (extension == ".c") {
        return "C";
    }

    if (
        extension == ".h"
        || extension == ".hpp"
        || extension == ".hh"
        || extension == ".hxx"
    ) {
        return "C/C++ header";
    }

    if (extension == ".py") {
        return "Python";
    }

    if (extension == ".js") {
        return "JavaScript";
    }

    if (extension == ".ts") {
        return "TypeScript";
    }

    if (extension == ".rs") {
        return "Rust";
    }

    if (extension == ".go") {
        return "Go";
    }

    if (extension == ".sql") {
        return "SQL";
    }

    if (
        extension == ".sh"
        || extension == ".bash"
    ) {
        return "Shell";
    }

    return "Unknown";
}

fs::path FileExplainer::find_related_header(
    const fs::path& path
) {
    const std::string extension =
        path.extension().string();

    if (
        extension != ".cpp"
        && extension != ".cc"
        && extension != ".cxx"
        && extension != ".c"
    ) {
        return {};
    }

    /*
     * First, inspect local #include statements.
     *
     * Example:
     *
     * #include "../include/test.h"
     *
     * This lets the file explainer discover
     * headers even when their names do not match
     * the source file.
     */
    std::ifstream source_file(path);

    if (source_file) {
        std::string line;

        while (std::getline(source_file, line)) {
            const std::string include_token =
                "#include";

            const std::size_t include_position =
                line.find(include_token);

            if (include_position == std::string::npos) {
                continue;
            }

            const std::size_t first_quote =
                line.find(
                    '"',
                    include_position + include_token.size()
                );

            if (first_quote == std::string::npos) {
                continue;
            }

            const std::size_t second_quote =
                line.find(
                    '"',
                    first_quote + 1
                );

            if (second_quote == std::string::npos) {
                continue;
            }

            const std::string included_file =
                line.substr(
                    first_quote + 1,
                    second_quote - first_quote - 1
                );

            fs::path included_path(
                included_file
            );

            /*
             * First try resolving the include relative
             * to the source file.
             */
            fs::path candidate =
                path.parent_path() / included_path;

            std::error_code error;

            if (
                fs::exists(candidate, error)
                && !error
                && fs::is_regular_file(candidate, error)
            ) {
                return candidate;
            }

            /*
             * If the include uses a path such as:
             *
             * include/test.h
             *
             * also try resolving it from the
             * project root.
             */
            candidate =
                path.parent_path()
                .parent_path()
                / included_path;

            error.clear();

            if (
                fs::exists(candidate, error)
                && !error
                && fs::is_regular_file(candidate, error)
            ) {
                return candidate;
            }

            /*
             * Finally, search an include directory
             * located next to the source directory.
             */
            candidate =
                path.parent_path()
                .parent_path()
                / "include"
                / included_path.filename();

            error.clear();

            if (
                fs::exists(candidate, error)
                && !error
                && fs::is_regular_file(candidate, error)
            ) {
                return candidate;
            }
        }
    }

    /*
     * Fallback:
     *
     * If the source does not contain a local include
     * that we can resolve, look for a header with the
     * same name.
     *
     * Example:
     *
     * FileManager.cpp → FileManager.h
     */
    const fs::path directory =
        path.parent_path();

    const std::string stem =
        path.stem().string();

    const fs::path candidates[] = {
        directory / (stem + ".h"),
        directory / (stem + ".hpp"),
        directory.parent_path()
            / "include"
            / (stem + ".h"),
        directory.parent_path()
            / "include"
            / (stem + ".hpp")
    };

    for (const fs::path& candidate : candidates) {
        std::error_code error;

        if (
            fs::exists(candidate, error)
            && !error
            && fs::is_regular_file(candidate, error)
        ) {
            return candidate;
        }
    }

    return {};
}

bool FileExplainer::is_git_tracked(
    const fs::path& path
) {
    const fs::path directory =
        fs::is_directory(path)
            ? path
            : path.parent_path();

    const std::string command =
        "git -C \"" +
        directory.string() +
        "\" ls-files --error-unmatch \"" +
        path.filename().string() +
        "\" > /dev/null 2>&1";

    return std::system(command.c_str()) == 0;
}

std::string FileExplainer::get_git_status(
    const fs::path& path
) {
    const fs::path directory =
        fs::is_directory(path)
            ? path
            : path.parent_path();

    const std::string command =
        "git -C \"" +
        directory.string() +
        "\" status --short -- \"" +
        path.filename().string() +
        "\"";

    FILE* pipe =
        popen(
            command.c_str(),
            "r"
        );

    if (!pipe) {
        return "Unavailable";
    }

    char buffer[256];

    std::string output;

    while (
        fgets(
            buffer,
            sizeof(buffer),
            pipe
        )
    ) {
        output += buffer;
    }

    pclose(pipe);

    if (output.empty()) {
        return "Clean";
    }

    return "Modified";
}

std::string FileExplainer::format_size(
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

void FileExplainer::explain(
    const fs::path& path
) {
    std::error_code error;

    if (!fs::exists(path, error)) {
        throw std::runtime_error(
            "Path does not exist: " +
            path.string()
        );
    }

    if (!fs::is_regular_file(path, error)) {
        throw std::runtime_error(
            "why currently supports files only: " +
            path.string()
        );
    }

    std::cout
        << "\n========================================\n"
        << "             FILE EXPLAINER\n"
        << "========================================\n\n";

    std::cout
        << "Path: "
        << path.string()
        << "\n";

    std::cout
        << "Name: "
        << path.filename().string()
        << "\n";

    std::cout
        << "Type: "
        << detect_file_role(path)
        << "\n";

    std::cout
        << "Language: "
        << detect_language(path)
        << "\n";

    std::cout
        << "\nLocation:\n";

    std::cout
        << "  Directory: "
        << (
            path.parent_path().empty()
                ? "."
                : path.parent_path().string()
        )
        << "\n";

    std::cout
        << "  Extension: "
        << (
            path.extension().empty()
                ? "(none)"
                : path.extension().string()
        )
        << "\n";

    std::error_code size_error;

    const std::uintmax_t size =
        fs::file_size(
            path,
            size_error
        );

    std::cout
        << "\nSize:\n";

    if (size_error) {
        std::cout
            << "  Size: unavailable\n";
    }
    else {
        std::cout
            << "  Size: "
            << format_size(size)
            << "\n";
    }

    std::cout
        << "\nProject Role:\n";

    std::cout
        << "  "
        << detect_file_role(path)
        << "\n";

    const fs::path related_header =
        find_related_header(path);

    if (!related_header.empty()) {
        std::cout
            << "\nRelated Files:\n"
            << "  Header: "
            << related_header.string()
            << "\n";
    }

    std::cout
        << "\nGit:\n";

    if (is_git_tracked(path)) {
        std::cout
            << "  Tracked: Yes\n";
        std::cout
            << "  Status: "
            << get_git_status(path)
            << "\n";
    }
    else {
        std::cout
            << "  Tracked: No\n";
    }

    std::cout
        << "\nPurpose:\n";

    const std::string role =
        detect_file_role(path);

    if (role == "Source code") {
        std::cout
            << "  Contains implementation code\n"
            << "  for the project.\n";
    }
    else if (role == "Header / interface") {
        std::cout
            << "  Defines interfaces, declarations,\n"
            << "  types, or reusable components.\n";
    }
    else if (role == "Build configuration") {
        std::cout
            << "  Defines how the project is built.\n";
    }
    else if (role == "Project documentation") {
        std::cout
            << "  Provides documentation and project\n"
            << "  information for developers.\n";
    }
    else if (role == "Git configuration") {
        std::cout
            << "  Controls Git behavior for the project.\n";
    }
    else if (
        role == "Configuration"
        || role == "Configuration / data"
    ) {
        std::cout
            << "  Stores configuration or structured\n"
            << "  project data.\n";
    }
    else {
        std::cout
            << "  General project file.\n";
    }

    std::cout
        << "\n========================================\n";
}