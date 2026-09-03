#include "FileManager.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

FileManager::FileManager()
    : current_directory(fs::current_path()) {
}

// ============================================================
// Navigation
// ============================================================

void FileManager::list_directory(
    bool show_hidden,
    const std::string& sort_option
) const {

    try {
        std::vector<fs::directory_entry> entries;

        for (const auto& entry :
             fs::directory_iterator(current_directory)) {

            if (!show_hidden && is_hidden(entry.path())) {
                continue;
            }

            entries.push_back(entry);
        }

        if (sort_option == "name") {

            std::sort(
                entries.begin(),
                entries.end(),
                [](const auto& a, const auto& b) {
                    return a.path().filename().string()
                         < b.path().filename().string();
                }
            );
        }
        else if (sort_option == "name-desc") {

            std::sort(
                entries.begin(),
                entries.end(),
                [](const auto& a, const auto& b) {
                    return a.path().filename().string()
                         > b.path().filename().string();
                }
            );
        }
        else if (sort_option == "size") {

            std::sort(
                entries.begin(),
                entries.end(),
                [](const auto& a, const auto& b) {

                    std::uintmax_t size_a = 0;
                    std::uintmax_t size_b = 0;

                    if (fs::is_regular_file(a.path())) {
                        size_a = fs::file_size(a.path());
                    }

                    if (fs::is_regular_file(b.path())) {
                        size_b = fs::file_size(b.path());
                    }

                    return size_a < size_b;
                }
            );
        }
        else if (sort_option == "size-desc") {

            std::sort(
                entries.begin(),
                entries.end(),
                [](const auto& a, const auto& b) {

                    std::uintmax_t size_a = 0;
                    std::uintmax_t size_b = 0;

                    if (fs::is_regular_file(a.path())) {
                        size_a = fs::file_size(a.path());
                    }

                    if (fs::is_regular_file(b.path())) {
                        size_b = fs::file_size(b.path());
                    }

                    return size_a > size_b;
                }
            );
        }

        for (const auto& entry : entries) {

            std::cout
                << std::left
                << std::setw(30)
                << entry.path().filename().string();

            if (fs::is_directory(entry.path())) {
                std::cout << " [DIR]";
            }
            else if (fs::is_symlink(entry.path())) {
                std::cout << " [LINK]";
            }
            else if (fs::is_regular_file(entry.path())) {

                try {
                    std::cout
                        << " "
                        << fs::file_size(entry.path())
                        << " bytes";
                }
                catch (...) {
                    std::cout << " [FILE]";
                }
            }

            std::cout << '\n';
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error listing directory: "
            << error.what()
            << '\n';
    }
}

void FileManager::print_working_directory() const {

    std::cout
        << "Current directory: "
        << current_directory
        << '\n';
}

void FileManager::change_directory(
    const std::string& name
) {
    fs::path target = resolve_path(name);

    try {
        if (!fs::exists(target)) {
            std::cerr
                << "Error: directory does not exist.\n";
            return;
        }

        if (!fs::is_directory(target)) {
            std::cerr
                << "Error: not a directory.\n";
            return;
        }

        current_directory = fs::canonical(target);
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error changing directory: "
            << error.what()
            << '\n';
    }
}

void FileManager::go_back() {

    if (current_directory.has_parent_path()) {
        current_directory =
            current_directory.parent_path();
    }
}

// ============================================================
// Basic filesystem operations
// ============================================================

void FileManager::make_directory(
    const std::string& name
) {
    fs::path path = resolve_path(name);

    try {
        if (fs::exists(path)) {
            std::cerr
                << "Error: item already exists.\n";
            return;
        }

        if (fs::create_directory(path)) {
            std::cout
                << "Directory created: "
                << path
                << '\n';
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error creating directory: "
            << error.what()
            << '\n';
    }
}

void FileManager::create_file(
    const std::string& name
) {
    fs::path path = resolve_path(name);

    try {
        if (fs::exists(path)) {
            std::cerr
                << "Error: item already exists.\n";
            return;
        }

        std::ofstream file(path);

        if (!file) {
            std::cerr
                << "Error: could not create file.\n";
            return;
        }

        std::cout
            << "File created: "
            << path
            << '\n';
    }
    catch (const std::exception& error) {
        std::cerr
            << "Error creating file: "
            << error.what()
            << '\n';
    }
}

void FileManager::rename_item(
    const std::string& old_name,
    const std::string& new_name
) {
    fs::path old_path =
        resolve_path(old_name);

    fs::path new_path =
        resolve_path(new_name);

    try {
        if (!fs::exists(old_path)) {
            std::cerr
                << "Error: source does not exist.\n";
            return;
        }

        if (fs::exists(new_path)) {
            std::cerr
                << "Error: destination already exists.\n";
            return;
        }

        fs::rename(old_path, new_path);

        std::cout
            << "Renamed successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error renaming item: "
            << error.what()
            << '\n';
    }
}

void FileManager::copy_item(
    const std::string& source,
    const std::string& destination
) {
    fs::path source_path =
        resolve_path(source);

    fs::path destination_path =
        resolve_path(destination);

    try {
        if (!fs::exists(source_path)) {
            std::cerr
                << "Error: source does not exist.\n";
            return;
        }

        if (fs::exists(destination_path)) {
            std::cerr
                << "Error: destination already exists.\n";
            return;
        }

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

        std::cout
            << "Copied successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error copying item: "
            << error.what()
            << '\n';
    }
}

void FileManager::move_item(
    const std::string& source,
    const std::string& destination
) {
    fs::path source_path =
        resolve_path(source);

    fs::path destination_path =
        resolve_path(destination);

    try {
        if (!fs::exists(source_path)) {
            std::cerr
                << "Error: source does not exist.\n";
            return;
        }

        if (fs::exists(destination_path)) {
            std::cerr
                << "Error: destination already exists.\n";
            return;
        }

        fs::rename(
            source_path,
            destination_path
        );

        std::cout
            << "Moved successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error moving item: "
            << error.what()
            << '\n';
    }
}

void FileManager::remove_item(
    const std::string& name
) {
    fs::path path = resolve_path(name);

    try {
        if (!fs::exists(path)) {
            std::cerr
                << "Error: item does not exist.\n";
            return;
        }

        if (fs::is_directory(path)) {

            std::cout
                << "Warning: this will recursively delete "
                << path
                << ".\n";

            std::cout
                << "Continue? (y/n): ";

            std::string answer;
            std::getline(std::cin, answer);

            if (answer != "y" && answer != "Y") {
                std::cout
                    << "Operation cancelled.\n";
                return;
            }

            fs::remove_all(path);
        }
        else {
            fs::remove(path);
        }

        std::cout
            << "Removed successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error removing item: "
            << error.what()
            << '\n';
    }
}

// ============================================================
// File information
// ============================================================

void FileManager::show_file_size(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    try {
        if (!fs::exists(path)) {
            std::cerr
                << "Error: item does not exist.\n";
            return;
        }

        if (!fs::is_regular_file(path)) {
            std::cerr
                << "Error: item is not a regular file.\n";
            return;
        }

        std::cout
            << "Size: "
            << fs::file_size(path)
            << " bytes\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error getting file size: "
            << error.what()
            << '\n';
    }
}

void FileManager::show_file_type(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    try {
        if (fs::is_symlink(path)) {
            std::cout
                << "Type: Symbolic link\n";
        }
        else if (fs::is_directory(path)) {
            std::cout
                << "Type: Directory\n";
        }
        else if (fs::is_regular_file(path)) {
            std::cout
                << "Type: Regular file\n";
        }
        else {
            std::cout
                << "Type: Other\n";
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error getting file type: "
            << error.what()
            << '\n';
    }
}

void FileManager::show_modified_time(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    try {
        if (!fs::exists(path)) {
            std::cerr
                << "Error: item does not exist.\n";
            return;
        }

        std::cout
            << "Modified: "
            << format_time(fs::last_write_time(path))
            << '\n';
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error getting modification time: "
            << error.what()
            << '\n';
    }
}

void FileManager::show_info(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    try {
        if (!fs::exists(path) &&
            !fs::is_symlink(path)) {

            std::cerr
                << "Error: item does not exist.\n";
            return;
        }

        std::cout
            << "Name: "
            << path.filename()
            << '\n';

        if (fs::is_symlink(path)) {

            std::cout
                << "Type: Symbolic link\n";

            std::cout
                << "Symlink target: "
                << fs::read_symlink(path)
                << '\n';
        }
        else if (fs::is_directory(path)) {

            std::cout
                << "Type: Directory\n";
        }
        else if (fs::is_regular_file(path)) {

            std::cout
                << "Type: Regular file\n";

            std::cout
                << "Size: "
                << fs::file_size(path)
                << " bytes\n";
        }
        else {

            std::cout
                << "Type: Other\n";
        }

        std::cout
            << "Permissions: "
            << permission_string(path)
            << '\n';

        if (!fs::is_symlink(path)) {
            std::cout
                << "Modified: "
                << format_time(
                    fs::last_write_time(path)
                )
                << '\n';
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error getting information: "
            << error.what()
            << '\n';
    }
}

// ============================================================
// Recursive operations
// ============================================================

void FileManager::show_tree(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    try {
        if (!fs::exists(path)) {
            std::cerr
                << "Error: path does not exist.\n";
            return;
        }

        std::cout
            << path.filename()
            << '\n';

        print_tree_recursive(
            path,
            ""
        );
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error displaying tree: "
            << error.what()
            << '\n';
    }
}

void FileManager::print_tree_recursive(
    const fs::path& path,
    const std::string& prefix
) const {

    if (!fs::is_directory(path)) {
        return;
    }

    std::vector<fs::directory_entry> entries;

    for (const auto& entry :
         fs::directory_iterator(path)) {

        entries.push_back(entry);
    }

    for (std::size_t i = 0;
         i < entries.size();
         ++i) {

        const bool last =
            (i == entries.size() - 1);

        std::cout
            << prefix
            << (last ? "└── " : "├── ")
            << entries[i].path().filename()
            << '\n';

        if (fs::is_directory(entries[i].path()) &&
            !fs::is_symlink(entries[i].path())) {

            print_tree_recursive(
                entries[i].path(),
                prefix + (last ? "    " : "│   ")
            );
        }
    }
}

void FileManager::show_directory_size(
    const std::string& name
) const {
    fs::path path = resolve_path(name);

    try {
        if (!fs::exists(path)) {
            std::cerr
                << "Error: path does not exist.\n";
            return;
        }

        std::uintmax_t size =
            calculate_directory_size(path);

        std::cout
            << "Total size: "
            << size
            << " bytes\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error calculating directory size: "
            << error.what()
            << '\n';
    }
}

std::uintmax_t FileManager::calculate_directory_size(
    const fs::path& path
) const {

    if (fs::is_regular_file(path)) {
        return fs::file_size(path);
    }

    if (!fs::is_directory(path)) {
        return 0;
    }

    std::uintmax_t total = 0;

    for (const auto& entry :
         fs::directory_iterator(path)) {

        if (fs::is_symlink(entry.path())) {
            continue;
        }

        total += calculate_directory_size(
            entry.path()
        );
    }

    return total;
}

// ============================================================
// Search
// ============================================================

void FileManager::find_by_name(
    const std::string& name,
    const std::string& path_string
) const {

    fs::path start =
        resolve_path(path_string);

    try {
        if (!fs::exists(start)) {
            std::cerr
                << "Error: search path does not exist.\n";
            return;
        }

        bool found = false;

        for (const auto& entry :
             fs::recursive_directory_iterator(
                 start,
                 fs::directory_options::skip_permission_denied
             )) {

            if (entry.path().filename() == name) {

                std::cout
                    << entry.path()
                    << '\n';

                found = true;
            }
        }

        if (!found) {
            std::cout
                << "No matching items found.\n";
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error searching: "
            << error.what()
            << '\n';
    }
}

void FileManager::find_by_extension(
    const std::string& extension,
    const std::string& path_string
) const {

    fs::path start =
        resolve_path(path_string);

    try {
        if (!fs::exists(start)) {
            std::cerr
                << "Error: search path does not exist.\n";
            return;
        }

        std::string wanted = extension;

        if (!wanted.empty() &&
            wanted[0] != '.') {

            wanted = "." + wanted;
        }

        bool found = false;

        for (const auto& entry :
             fs::recursive_directory_iterator(
                 start,
                 fs::directory_options::skip_permission_denied
             )) {

            if (!entry.is_regular_file()) {
                continue;
            }

            if (entry.path().extension() == wanted) {

                std::cout
                    << entry.path()
                    << '\n';

                found = true;
            }
        }

        if (!found) {
            std::cout
                << "No matching files found.\n";
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error searching: "
            << error.what()
            << '\n';
    }
}

void FileManager::find_by_size(
    std::uintmax_t minimum_size,
    const std::string& path_string
) const {

    fs::path start =
        resolve_path(path_string);

    try {
        if (!fs::exists(start)) {
            std::cerr
                << "Error: search path does not exist.\n";
            return;
        }

        bool found = false;

        for (const auto& entry :
             fs::recursive_directory_iterator(
                 start,
                 fs::directory_options::skip_permission_denied
             )) {

            if (!entry.is_regular_file()) {
                continue;
            }

            if (entry.file_size() >= minimum_size) {

                std::cout
                    << entry.path()
                    << " ("
                    << entry.file_size()
                    << " bytes)"
                    << '\n';

                found = true;
            }
        }

        if (!found) {
            std::cout
                << "No matching files found.\n";
        }
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error searching by size: "
            << error.what()
            << '\n';
    }
}

// ============================================================
// Phase 8: Permissions
// ============================================================

void FileManager::show_permissions(
    const std::string& name
) const {

    fs::path path =
        resolve_path(name);

    try {
        if (!fs::exists(path) &&
            !fs::is_symlink(path)) {

            std::cerr
                << "Error: item does not exist.\n";
            return;
        }

        std::cout
            << "Permissions: "
            << permission_string(path)
            << '\n';
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error reading permissions: "
            << error.what()
            << '\n';
    }
}

void FileManager::change_permissions(
    const std::string& mode,
    const std::string& name
) const {

    fs::path path =
        resolve_path(name);

    try {
        if (!fs::exists(path)) {
            std::cerr
                << "Error: item does not exist.\n";
            return;
        }

        if (mode.empty()) {
            std::cerr
                << "Error: invalid permission mode.\n";
            return;
        }

        unsigned int numeric_mode = 0;

        try {
            std::size_t position = 0;

            numeric_mode =
                std::stoul(
                    mode,
                    &position,
                    8
                );

            if (position != mode.size() ||
                numeric_mode > 0777) {

                std::cerr
                    << "Error: permission mode must be "
                    << "an octal value between 000 and 777.\n";

                return;
            }
        }
        catch (...) {
            std::cerr
                << "Error: invalid permission mode.\n";

            return;
        }

        if (::chmod(
                path.c_str(),
                static_cast<mode_t>(numeric_mode)
            ) != 0) {

            std::perror("chmod");
            return;
        }

        std::cout
            << "Permissions changed successfully.\n";

        std::cout
            << "New permissions: "
            << permission_string(path)
            << '\n';
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error changing permissions: "
            << error.what()
            << '\n';
    }
}

std::string FileManager::permission_string(
    const fs::path& path
) const {

    struct stat file_stat {};

    if (lstat(
            path.c_str(),
            &file_stat
        ) != 0) {

        return "?????????";
    }

    std::string result;

    result +=
        (file_stat.st_mode & S_IRUSR)
            ? 'r' : '-';

    result +=
        (file_stat.st_mode & S_IWUSR)
            ? 'w' : '-';

    result +=
        (file_stat.st_mode & S_IXUSR)
            ? 'x' : '-';

    result +=
        (file_stat.st_mode & S_IRGRP)
            ? 'r' : '-';

    result +=
        (file_stat.st_mode & S_IWGRP)
            ? 'w' : '-';

    result +=
        (file_stat.st_mode & S_IXGRP)
            ? 'x' : '-';

    result +=
        (file_stat.st_mode & S_IROTH)
            ? 'r' : '-';

    result +=
        (file_stat.st_mode & S_IWOTH)
            ? 'w' : '-';

    result +=
        (file_stat.st_mode & S_IXOTH)
            ? 'x' : '-';

    return result;
}

// ============================================================
// Phase 8: Links
// ============================================================

void FileManager::create_hard_link(
    const std::string& target,
    const std::string& link_name
) const {

    fs::path target_path =
        resolve_path(target);

    fs::path link_path =
        resolve_path(link_name);

    try {
        if (!fs::exists(target_path)) {
            std::cerr
                << "Error: target does not exist.\n";
            return;
        }

        if (fs::exists(link_path)) {
            std::cerr
                << "Error: link name already exists.\n";
            return;
        }

        if (fs::is_directory(target_path)) {
            std::cerr
                << "Error: hard links to directories "
                << "are not supported.\n";
            return;
        }

        fs::create_hard_link(
            target_path,
            link_path
        );

        std::cout
            << "Hard link created successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error creating hard link: "
            << error.what()
            << '\n';
    }
}

void FileManager::create_symbolic_link(
    const std::string& target,
    const std::string& link_name
) const {

    fs::path target_path =
        resolve_path(target);

    fs::path link_path =
        resolve_path(link_name);

    try {
        if (fs::exists(link_path)) {
            std::cerr
                << "Error: link name already exists.\n";
            return;
        }

        fs::create_symlink(
            target,
            link_path
        );

        std::cout
            << "Symbolic link created successfully.\n";
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error creating symbolic link: "
            << error.what()
            << '\n';
    }
}

void FileManager::show_link_target(
    const std::string& name
) const {

    fs::path path =
        resolve_path(name);

    try {
        if (!fs::is_symlink(path)) {
            std::cerr
                << "Error: item is not a symbolic link.\n";
            return;
        }

        std::cout
            << "Symlink target: "
            << fs::read_symlink(path)
            << '\n';
    }
    catch (const fs::filesystem_error& error) {
        std::cerr
            << "Error reading symbolic link: "
            << error.what()
            << '\n';
    }
}

// ============================================================
// Helpers
// ============================================================

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
        return "Unknown";
    }

    std::ostringstream output;

    output
        << std::put_time(
            local_time,
            "%Y-%m-%d %H:%M:%S"
        );

    return output.str();
}

fs::path FileManager::resolve_path(
    const std::string& path
) const {

    fs::path input(path);

    if (input.is_absolute()) {
        return input.lexically_normal();
    }

    return (
        current_directory / input
    ).lexically_normal();
}

bool FileManager::is_hidden(
    const fs::path& path
) const {

    std::string filename =
        path.filename().string();

    return
        !filename.empty() &&
        filename[0] == '.';
}