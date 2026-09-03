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

void FileManager::list_directory() const {
    try {
        for (const auto& entry : fs::directory_iterator(current_directory)) {
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

void FileManager::change_directory(const std::string& name) {
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

void FileManager::make_directory(const std::string& name) {
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

void FileManager::create_file(const std::string& name) {
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

void FileManager::copy_file(
    const std::string& source,
    const std::string& destination
) {
    fs::path source_path = current_directory / source;
    fs::path destination_path = current_directory / destination;

    if (!fs::exists(source_path)) {
        std::cerr << "Error: source does not exist.\n";
        return;
    }

    if (!fs::is_regular_file(source_path)) {
        std::cerr << "Error: source is not a regular file.\n";
        return;
    }

    if (fs::exists(destination_path)) {
        std::cerr << "Error: destination already exists.\n";
        return;
    }

    try {
        fs::copy_file(
            source_path,
            destination_path
        );

        std::cout << "Copied "
                  << source
                  << " -> "
                  << destination
                  << '\n';
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error copying file: "
                  << e.what() << '\n';
    }
}

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

void FileManager::remove_item(const std::string& name) {
    fs::path path = current_directory / name;

    if (!fs::exists(path)) {
        std::cerr << "Error: item does not exist.\n";
        return;
    }

    if (fs::is_directory(path) && !fs::is_empty(path)) {
        std::cerr << "Error: directory is not empty.\n";
        return;
    }

    try {
        if (fs::remove(path)) {
            std::cout << "Removed: "
                      << name << '\n';
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error removing item: "
                  << e.what() << '\n';
    }
}

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
        std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            file_time - fs::file_time_type::clock::now()
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
                  << format_time(fs::last_write_time(path))
                  << '\n';
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Error getting modification time: "
                  << e.what() << '\n';
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
                  << format_time(fs::last_write_time(path))
                  << '\n';
    }
    catch (const fs::filesystem_error&) {
        std::cout << "Last modified: unavailable\n";
    }
}