#include "FileManager.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

FileManager::FileManager()
    : current_directory(fs::current_path()) {
}

// ============================================================
// Navigation
// ============================================================

void FileManager::list_directory() const {
    try {
        for (const auto& entry :
             fs::directory_iterator(current_directory)) {

            std::cout << entry.path().filename().string();

            if (fs::is_directory(entry.path())) {
                std::cout << "/";
            }
            else if (fs::is_regular_file(entry.path())) {
                std::cout << "  ["
                          << fs::file_size(entry.path())
                          << " bytes]";
            }
            else if (fs::is_symlink(entry.path())) {
                std::cout << "  [symlink]";
            }

            std::cout << '\n';
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error listing directory: "
                  << e.what() << '\n';
    }
}

void FileManager::print_working_directory() const {
    std::cout << current_directory << '\n';
}

void FileManager::change_directory(
    const std::string& name
) {
    fs::path new_path = current_directory / name;

    if (!fs::exists(new_path)) {
        std::cerr << "Error: directory does not exist.\n";
        return;
    }

    if (!fs::is_directory(new_path)) {
        std::cerr << "Error: path is not a directory.\n";
        return;
    }

    try {
        current_directory = fs::canonical(new_path);
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error changing directory: "
                  << e.what() << '\n';
    }
}

void FileManager::go_back() {
    if (current_directory.has_parent_path()) {
        current_directory = current_directory.parent_path();
    }
}

// ============================================================
// Basic filesystem operations
// ============================================================

void FileManager::make_directory(
    const std::string& name
) {
    fs::path path = current_directory / name;

    if (fs::exists(path)) {
        std::cerr << "Error: item already exists.\n";
        return;
    }

    try {
        if (fs::create_directory(path)) {
            std::cout << "Directory created: "
                      << name << '\n';
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error creating directory: "
                  << e.what() << '\n';
    }
}

void FileManager::create_file(
    const std::string& name
) {
    fs::path path = current_directory / name;

    if (fs::exists(path)) {
        std::cerr << "Error: item already exists.\n";
        return;
    }

    std::ofstream file(path);

    if (!file) {
        std::cerr << "Error: could not create file.\n";
        return;
    }

    std::cout << "File created: "
              << name << '\n';
}

void FileManager::rename_item(
    const std::string& old_name,
    const std::string& new_name
) {
    fs::path old_path = current_directory / old_name;
    fs::path new_path = current_directory / new_name;

    if (!fs::exists(old_path)) {
        std::cerr << "Error: source does not exist.\n";
        return;
    }

    if (fs::exists(new_path)) {
        std::cerr << "Error: destination already exists.\n";
        return;
    }

    try {
        fs::rename(old_path, new_path);

        std::cout << "Renamed "
                  << old_name
                  << " -> "
                  << new_name
                  << '\n';
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error renaming item: "
                  << e.what() << '\n';
    }
}

// ============================================================
// Copy
// ============================================================

void FileManager::copy_item(
    const std::string& source,
    const std::string& destination
) {
    fs::path source_path = current_directory / source;
    fs::path destination_path = current_directory / destination;

    if (!fs::exists(source_path)) {
        std::cerr << "Error: source does not exist.\n";
        return;
    }

    if (fs::exists(destination_path)) {
        std::cerr << "Error: destination already exists.\n";
        return;
    }

    try {
        if (fs::is_regular_file(source_path)) {
            fs::copy_file(
                source_path,
                destination_path
            );
        }
        else if (fs::is_directory(source_path)) {
            fs::copy(
                source_path,
                destination_path,
                fs::copy_options::recursive
            );
        }
        else {
            std::cerr << "Error: unsupported filesystem item.\n";
            return;
        }

        std::cout << "Copied "
                  << source
                  << " -> "
                  << destination
                  << '\n';
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error copying item: "
                  << e.what() << '\n';
    }
}

// ============================================================
// Move
// ============================================================

void FileManager::move_item(
    const std::string& source,
    const std::string& destination
) {
    fs::path source_path = current_directory / source;
    fs::path destination_path = current_directory / destination;

    if (!fs::exists(source_path)) {
        std::cerr << "Error: source does not exist.\n";
        return;
    }

    if (fs::exists(destination_path)) {
        std::cerr << "Error: destination already exists.\n";
        return;
    }

    try {
        fs::rename(source_path, destination_path);

        std::cout << "Moved "
                  << source
                  << " -> "
                  << destination
                  << '\n';
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error moving item: "
                  << e.what() << '\n';
    }
}

// ============================================================
// Remove
// ============================================================

void FileManager::remove_item(
    const std::string& name
) {
    fs::path path = current_directory / name;

    if (!fs::exists(path)) {
        std::cerr << "Error: item does not exist.\n";
        return;
    }

    try {
        if (fs::is_directory(path)) {
            fs::remove_all(path);
        }
        else {
            fs::remove(path);
        }

        std::cout << "Removed: "
                  << name << '\n';
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error removing item: "
                  << e.what() << '\n';
    }
}

// ============================================================
// File information
// ============================================================

void FileManager::show_file_size(
    const std::string& name
) const {
    fs::path path = current_directory / name;

    if (!fs::exists(path)) {
        std::cerr << "Error: item does not exist.\n";
        return;
    }

    if (!fs::is_regular_file(path)) {
        std::cerr << "Error: item is not a regular file.\n";
        return;
    }

    try {
        std::cout << "Size: "
                  << fs::file_size(path)
                  << " bytes\n";
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error getting file size: "
                  << e.what() << '\n';
    }
}

void FileManager::show_file_type(
    const std::string& name
) const {
    fs::path path = current_directory / name;

    if (!fs::exists(path)) {
        std::cerr << "Error: item does not exist.\n";
        return;
    }

    if (fs::is_directory(path)) {
        std::cout << "Type: directory\n";
    }
    else if (fs::is_regular_file(path)) {
        std::cout << "Type: regular file\n";
    }
    else if (fs::is_symlink(path)) {
        std::cout << "Type: symbolic link\n";
    }
    else {
        std::cout << "Type: other\n";
    }
}

std::string FileManager::format_time(
    const fs::file_time_type& file_time
) const {
    auto system_time =
        std::chrono::time_point_cast<
            std::chrono::system_clock::duration
        >(
            file_time
            - fs::file_time_type::clock::now()
            + std::chrono::system_clock::now()
        );

    std::time_t time =
        std::chrono::system_clock::to_time_t(system_time);

    std::tm* local_time = std::localtime(&time);

    if (!local_time) {
        return "Unknown";
    }

    std::ostringstream output;

    output << std::put_time(
        local_time,
        "%Y-%m-%d %H:%M:%S"
    );

    return output.str();
}

void FileManager::show_modified_time(
    const std::string& name
) const {
    fs::path path = current_directory / name;

    if (!fs::exists(path)) {
        std::cerr << "Error: item does not exist.\n";
        return;
    }

    try {
        std::cout << "Last modified: "
                  << format_time(
                         fs::last_write_time(path)
                     )
                  << '\n';
    }
    catch (const fs::filesystem_error& e) {
        std::cerr
            << "Error getting modification time: "
            << e.what()
            << '\n';
    }
}

void FileManager::show_info(
    const std::string& name
) const {
    fs::path path = current_directory / name;

    if (!fs::exists(path)) {
        std::cerr << "Error: item does not exist.\n";
        return;
    }

    std::cout << "Name: "
              << path.filename().string()
              << '\n';

    std::cout << "Path: "
              << path
              << '\n';

    if (fs::is_directory(path)) {
        std::cout << "Type: directory\n";
    }
    else if (fs::is_regular_file(path)) {
        std::cout << "Type: regular file\n";

        try {
            std::cout << "Size: "
                      << fs::file_size(path)
                      << " bytes\n";
        }
        catch (const fs::filesystem_error&) {
            std::cout << "Size: unavailable\n";
        }
    }
    else if (fs::is_symlink(path)) {
        std::cout << "Type: symbolic link\n";
    }
    else {
        std::cout << "Type: other\n";
    }

    try {
        std::cout << "Last modified: "
                  << format_time(
                         fs::last_write_time(path)
                     )
                  << '\n';
    }
    catch (const fs::filesystem_error&) {
        std::cout
            << "Last modified: unavailable\n";
    }
}

// ============================================================
// Recursive tree
// ============================================================

void FileManager::show_tree(
    const std::string& name
) const {
    fs::path path = current_directory / name;

    if (!fs::exists(path)) {
        std::cerr << "Error: path does not exist.\n";
        return;
    }

    std::cout << path.filename().string();

    if (fs::is_directory(path)) {
        std::cout << "/";
    }

    std::cout << '\n';

    if (fs::is_directory(path)) {
        print_tree_recursive(path, "");
    }
}

void FileManager::print_tree_recursive(
    const fs::path& path,
    const std::string& prefix
) const {
    try {
        std::vector<fs::directory_entry> entries;

        for (const auto& entry :
             fs::directory_iterator(path)) {
            entries.push_back(entry);
        }

        for (std::size_t i = 0;
             i < entries.size();
             ++i) {

            const auto& entry = entries[i];

            bool is_last =
                (i == entries.size() - 1);

            std::cout << prefix;

            if (is_last) {
                std::cout << "└── ";
            }
            else {
                std::cout << "├── ";
            }

            std::cout
                << entry.path().filename().string();

            if (fs::is_directory(entry.path())) {
                std::cout << "/";
            }

            std::cout << '\n';

            if (fs::is_directory(entry.path())) {
                std::string new_prefix =
                    prefix +
                    (is_last ? "    " : "│   ");

                print_tree_recursive(
                    entry.path(),
                    new_prefix
                );
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr
            << "Error reading directory: "
            << e.what()
            << '\n';
    }
}

// ============================================================
// Recursive directory size
// ============================================================

std::uintmax_t FileManager::calculate_directory_size(
    const fs::path& path
) const {
    std::uintmax_t total_size = 0;

    try {
        if (fs::is_regular_file(path)) {
            return fs::file_size(path);
        }

        if (!fs::is_directory(path)) {
            return 0;
        }

        for (const auto& entry :
             fs::directory_iterator(path)) {

            if (fs::is_regular_file(entry.path())) {
                total_size += fs::file_size(entry.path());
            }
            else if (fs::is_directory(entry.path())) {
                total_size +=
                    calculate_directory_size(
                        entry.path()
                    );
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr
            << "Error calculating directory size: "
            << e.what()
            << '\n';
    }

    return total_size;
}

void FileManager::show_directory_size(
    const std::string& name
) const {
    fs::path path = current_directory / name;

    if (!fs::exists(path)) {
        std::cerr << "Error: path does not exist.\n";
        return;
    }

    std::uintmax_t total_size =
        calculate_directory_size(path);

    std::cout
        << "Total size: "
        << total_size
        << " bytes\n";
}