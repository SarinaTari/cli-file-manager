#include "FileManager.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

FileManager::FileManager()
    : current_directory(fs::current_path()) {
}

fs::path FileManager::resolve_path(
    const std::string& path
) const {
    fs::path input_path(path);

    if (input_path.is_absolute()) {
        return input_path.lexically_normal();
    }

    return (current_directory / input_path).lexically_normal();
}

void FileManager::list_directory() const {
    try {
        for (const auto& entry :
             fs::directory_iterator(current_directory)) {

            if (entry.is_directory()) {
                std::cout
                    << entry.path().filename().string()
                    << "/\n";
            }
            else if (entry.is_regular_file()) {
                std::cout
                    << entry.path().filename().string()
                    << "  ["
                    << fs::file_size(entry.path())
                    << " bytes]\n";
            }
            else {
                std::cout
                    << entry.path().filename().string()
                    << "\n";
            }
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::print_working_directory() const {
    std::cout << current_directory << "\n";
}

void FileManager::change_directory(
    const std::string& name
) {
    fs::path target = resolve_path(name);

    if (!fs::exists(target)) {
        std::cout << "Error: directory does not exist.\n";
        return;
    }

    if (!fs::is_directory(target)) {
        std::cout << "Error: path is not a directory.\n";
        return;
    }

    try {
        current_directory = fs::canonical(target);
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::go_back() {
    if (current_directory.has_parent_path()) {
        current_directory = current_directory.parent_path();
    }
}

void FileManager::make_directory(
    const std::string& name
) {
    fs::path path = resolve_path(name);

    if (fs::exists(path)) {
        std::cout << "Error: item already exists.\n";
        return;
    }

    try {
        if (fs::create_directories(path)) {
            std::cout << "Directory created: "
                      << path.filename().string()
                      << "\n";
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::create_file(
    const std::string& name
) {
    fs::path path = resolve_path(name);

    if (fs::exists(path)) {
        std::cout << "Error: item already exists.\n";
        return;
    }

    try {
        std::ofstream file(path);

        if (!file) {
            std::cout << "Error: could not create file.\n";
            return;
        }

        std::cout << "File created: "
                  << path.filename().string()
                  << "\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::rename_item(
    const std::string& old_name,
    const std::string& new_name
) {
    fs::path old_path = resolve_path(old_name);
    fs::path new_path = resolve_path(new_name);

    if (!fs::exists(old_path)) {
        std::cout << "Error: source does not exist.\n";
        return;
    }

    if (fs::exists(new_path)) {
        std::cout << "Error: destination already exists.\n";
        return;
    }

    try {
        fs::rename(old_path, new_path);

        std::cout << "Renamed successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::copy_item(
    const std::string& source,
    const std::string& destination
) {
    fs::path source_path = resolve_path(source);
    fs::path destination_path = resolve_path(destination);

    if (!fs::exists(source_path)) {
        std::cout << "Error: source does not exist.\n";
        return;
    }

    if (source_path == destination_path) {
        std::cout << "Error: source and destination are the same.\n";
        return;
    }

    if (fs::exists(destination_path)) {
        std::cout << "Error: destination already exists.\n";
        return;
    }

    try {
        if (fs::is_directory(source_path)) {
            fs::copy(
                source_path,
                destination_path,
                fs::copy_options::recursive
            );
        }
        else {
            fs::copy_file(
                source_path,
                destination_path
            );
        }

        std::cout << "Copied successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::move_item(
    const std::string& source,
    const std::string& destination
) {
    fs::path source_path = resolve_path(source);
    fs::path destination_path = resolve_path(destination);

    if (!fs::exists(source_path)) {
        std::cout << "Error: source does not exist.\n";
        return;
    }

    if (source_path == destination_path) {
        std::cout << "Error: source and destination are the same.\n";
        return;
    }

    if (fs::exists(destination_path)) {
        std::cout << "Error: destination already exists.\n";
        return;
    }

    try {
        fs::rename(source_path, destination_path);

        std::cout << "Moved successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::remove_item(
    const std::string& name
) {
    fs::path path = resolve_path(name);

    if (!fs::exists(path)) {
        std::cout << "Error: item does not exist.\n";
        return;
    }

    if (name == "." || name == "..") {
        std::cout << "Error: invalid path.\n";
        return;
    }

    if (path == current_directory) {
        std::cout << "Error: cannot remove the current directory.\n";
        return;
    }

    try {
        if (fs::is_directory(path)) {
            fs::remove_all(path);
        }
        else {
            fs::remove(path);
        }

        std::cout << "Removed successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::show_file_size(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    if (!fs::exists(path)) {
        std::cout << "Error: item does not exist.\n";
        return;
    }

    if (!fs::is_regular_file(path)) {
        std::cout << "Error: item is not a regular file.\n";
        return;
    }

    try {
        std::cout << "Size: "
                  << fs::file_size(path)
                  << " bytes\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::show_file_type(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    if (!fs::exists(path)) {
        std::cout << "Error: item does not exist.\n";
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

void FileManager::show_modified_time(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    if (!fs::exists(path)) {
        std::cout << "Error: item does not exist.\n";
        return;
    }

    try {
        std::cout << "Modified: "
                  << format_time(fs::last_write_time(path))
                  << "\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

void FileManager::show_info(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    if (!fs::exists(path)) {
        std::cout << "Error: item does not exist.\n";
        return;
    }

    std::cout << "Name: "
              << path.filename().string()
              << "\n";

    std::cout << "Path: "
              << fs::absolute(path)
              << "\n";

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
        catch (...) {
            std::cout << "Size: unavailable\n";
        }
    }
    else {
        std::cout << "Type: other\n";
    }

    try {
        std::cout << "Modified: "
                  << format_time(fs::last_write_time(path))
                  << "\n";
    }
    catch (...) {
        std::cout << "Modified: unavailable\n";
    }
}

void FileManager::show_tree(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    if (!fs::exists(path)) {
        std::cout << "Error: path does not exist.\n";
        return;
    }

    std::cout << path.filename().string() << "\n";

    if (fs::is_directory(path)) {
        print_tree_recursive(path, "");
    }
}

void FileManager::print_tree_recursive(
    const fs::path& path,
    const std::string& prefix
) const {
    std::vector<fs::directory_entry> entries;

    try {
        for (const auto& entry :
             fs::directory_iterator(path)) {
            entries.push_back(entry);
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cout << prefix
                  << "Error: "
                  << error.what()
                  << "\n";
        return;
    }

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];

        bool is_last = (i == entries.size() - 1);

        std::cout << prefix;

        if (is_last) {
            std::cout << "└── ";
        }
        else {
            std::cout << "├── ";
        }

        std::cout
            << entry.path().filename().string();

        if (entry.is_directory()) {
            std::cout << "/";
        }

        std::cout << "\n";

        if (entry.is_directory()) {
            std::string new_prefix = prefix;

            if (is_last) {
                new_prefix += "    ";
            }
            else {
                new_prefix += "│   ";
            }

            print_tree_recursive(
                entry.path(),
                new_prefix
            );
        }
    }
}

void FileManager::show_directory_size(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    if (!fs::exists(path)) {
        std::cout << "Error: path does not exist.\n";
        return;
    }

    try {
        if (fs::is_regular_file(path)) {
            std::cout << "Size: "
                      << fs::file_size(path)
                      << " bytes\n";
            return;
        }

        std::uintmax_t total =
            calculate_directory_size(path);

        std::cout << "Total size: "
                  << total
                  << " bytes\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error: "
                  << error.what()
                  << "\n";
    }
}

std::uintmax_t FileManager::calculate_directory_size(
    const fs::path& path
) const {
    std::uintmax_t total_size = 0;

    if (fs::is_regular_file(path)) {
        return fs::file_size(path);
    }

    if (!fs::is_directory(path)) {
        return 0;
    }

    try {
        for (const auto& entry :
             fs::directory_iterator(path)) {

            if (fs::is_regular_file(entry.path())) {
                total_size +=
                    fs::file_size(entry.path());
            }
            else if (fs::is_directory(entry.path())) {
                total_size +=
                    calculate_directory_size(
                        entry.path()
                    );
            }
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cout << "Error calculating size: "
                  << error.what()
                  << "\n";
    }

    return total_size;
}

std::string FileManager::format_time(
    const fs::file_time_type& file_time
) const {
    auto system_time =
        std::chrono::time_point_cast<
            std::chrono::system_clock::duration
        >(
            file_time -
            fs::file_time_type::clock::now() +
            std::chrono::system_clock::now()
        );

    std::time_t time =
        std::chrono::system_clock::to_time_t(
            system_time
        );

    std::tm* local_time =
        std::localtime(&time);

    if (!local_time) {
        return "unknown";
    }

    std::ostringstream output;

    output << std::put_time(
        local_time,
        "%Y-%m-%d %H:%M:%S"
    );

    return output.str();
}