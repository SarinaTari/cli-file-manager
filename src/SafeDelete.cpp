#include "SafeDelete.h"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

SafeDelete::DeleteInfo SafeDelete::analyze(
    const fs::path& path
) {
    DeleteInfo info;

    std::error_code error;

    if (fs::is_regular_file(path, error)) {
        info.files = 1;

        std::error_code size_error;

        info.total_size =
            fs::file_size(path, size_error);

        if (size_error) {
            info.total_size = 0;
        }

        return info;
    }

    if (!fs::is_directory(path, error)) {
        return info;
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

        std::error_code item_error;

        if (iterator->is_directory(item_error)) {
            if (!item_error) {
                ++info.directories;
            }
        }
        else if (iterator->is_regular_file(item_error)) {
            if (!item_error) {
                ++info.files;

                std::error_code size_error;

                const std::uintmax_t size =
                    iterator->file_size(size_error);

                if (!size_error) {
                    info.total_size += size;
                }
            }
        }

        iterator.increment(error);
    }

    return info;
}

void SafeDelete::collect_paths(
    const fs::path& path,
    std::vector<fs::path>& paths
) {
    paths.push_back(path);

    std::error_code error;

    if (!fs::is_directory(path, error)) {
        return;
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

        paths.push_back(iterator->path());

        iterator.increment(error);
    }
}

std::string SafeDelete::format_size(
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

bool SafeDelete::ask_confirmation() {
    std::cout
        << "\nContinue? [y/N]: ";

    std::string answer;

    std::getline(
        std::cin,
        answer
    );

    return
        answer == "y"
        || answer == "Y";
}

bool SafeDelete::is_dangerous_path(
    const fs::path& path
) {
    /*
     * Explicitly reject "." and "..".
     *
     * These paths represent the current directory
     * and parent directory and should never be
     * accepted by a destructive command.
     */
    if (path == "." || path == "..") {
        return true;
    }

    std::error_code error;

    const fs::path absolute =
        fs::absolute(path, error).lexically_normal();

    if (error) {
        return true;
    }

    /*
     * Never allow deletion of a filesystem root.
     */
    const fs::path root =
        absolute.root_path();

    return absolute == root;
}

void SafeDelete::remove(
    const fs::path& path
) {
    std::error_code error;

    if (!fs::exists(path, error)) {
        throw std::runtime_error(
            "Path does not exist: " +
            path.string()
        );
    }

    if (is_dangerous_path(path)) {
        throw std::runtime_error(
            "Refusing to delete a dangerous path: " +
            path.string()
        );
    }

    const DeleteInfo info =
        analyze(path);

    std::cout
        << "\n========================================\n"
        << "             SAFE DELETE\n"
        << "========================================\n\n";

    std::cout
        << "Target: "
        << path.string()
        << "\n";

    if (fs::is_directory(path, error)) {
        std::cout
            << "Type: Directory\n";
    }
    else {
        std::cout
            << "Type: File\n";
    }

    std::cout
        << "Files: "
        << info.files
        << "\n";

    std::cout
        << "Directories: "
        << info.directories
        << "\n";

    std::cout
        << "Total size: "
        << format_size(info.total_size)
        << "\n";

    std::cout
        << "\nWARNING: This operation cannot be undone.\n";

    if (!ask_confirmation()) {
        std::cout
            << "Deletion cancelled.\n";

        return;
    }

    std::error_code remove_error;

    fs::remove_all(
        path,
        remove_error
    );

    if (remove_error) {
        throw std::runtime_error(
            "Failed to delete: " +
            remove_error.message()
        );
    }

    std::cout
        << "Deleted successfully.\n";

    std::cout
        << "========================================\n";
}

