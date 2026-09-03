#include "FileManager.h"

#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>

FileManager::FileManager()
    : current_directory(fs::current_path()) {
}

// ============================================================
// Navigation
// ============================================================

void FileManager::list_directory(
    bool show_hidden,
    const std::string& sort_option,
    const std::string& filter
) const {

    try {
        std::vector<fs::directory_entry> entries;

        for (const auto& entry :
             fs::directory_iterator(
                 current_directory,
                 fs::directory_options::skip_permission_denied
             )) {

            if (!show_hidden && is_hidden(entry.path())) {
                continue;
            }

            if (
                filter == "files"
                && !fs::is_regular_file(entry.path())
            ) {
                continue;
            }

            if (
                filter == "dirs"
                && !fs::is_directory(entry.path())
            ) {
                continue;
            }

            entries.push_back(entry);
        }

        if (
            sort_option != "name"
            && sort_option != "name-desc"
            && sort_option != "size"
            && sort_option != "size-desc"
        ) {
            throw std::invalid_argument(
                "Invalid sort option."
            );
        }

        if (
            filter != "all"
            && filter != "files"
            && filter != "dirs"
        ) {
            throw std::invalid_argument(
                "Invalid filter."
            );
        }

        if (
            sort_option == "name"
            || sort_option == "name-desc"
        ) {
            std::sort(
                entries.begin(),
                entries.end(),
                [](const auto& a, const auto& b) {
                    return a.path().filename().string()
                        < b.path().filename().string();
                }
            );

            if (sort_option == "name-desc") {
                std::reverse(
                    entries.begin(),
                    entries.end()
                );
            }
        }
        else {
            std::sort(
                entries.begin(),
                entries.end(),
                [](const auto& a, const auto& b) {

                    std::uintmax_t size_a = 0;
                    std::uintmax_t size_b = 0;

                    std::error_code ec1;
                    std::error_code ec2;

                    if (a.is_regular_file(ec1)) {
                        size_a = a.file_size(ec1);
                    }

                    if (b.is_regular_file(ec2)) {
                        size_b = b.file_size(ec2);
                    }

                    return size_a < size_b;
                }
            );

            if (sort_option == "size-desc") {
                std::reverse(
                    entries.begin(),
                    entries.end()
                );
            }
        }

        for (const auto& entry : entries) {

            std::cout << std::left
                      << std::setw(8);

            if (fs::is_symlink(entry.path())) {
                std::cout << "[LINK]";
            }
            else if (fs::is_directory(entry.path())) {
                std::cout << "[DIR]";
            }
            else {
                std::cout << "[FILE]";
            }

            std::cout << " ";

            if (fs::is_regular_file(entry.path())) {
                std::error_code ec;
                auto size = fs::file_size(
                    entry.path(),
                    ec
                );

                if (!ec) {
                    std::cout
                        << std::right
                        << std::setw(10)
                        << size
                        << " bytes ";
                }
                else {
                    std::cout
                        << std::right
                        << std::setw(16)
                        << "? ";
                }
            }
            else {
                std::cout
                    << std::right
                    << std::setw(16)
                    << " ";
            }

            std::cout
                << " "
                << entry.path().filename().string()
                << '\n';
        }

    }
    catch (const fs::filesystem_error& error) {
        throw std::runtime_error(
            std::string("Cannot list directory: ")
            + error.what()
        );
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

    ensure_exists(target, "Directory");

    std::error_code ec;

    if (!fs::is_directory(target, ec) || ec) {
        throw std::runtime_error(
            "Not a directory: " + target.string()
        );
    }

    current_directory = fs::weakly_canonical(target);
}

void FileManager::go_back() {
    fs::path parent = current_directory.parent_path();

    if (parent.empty()) {
        return;
    }

    current_directory = parent;
}

// ============================================================
// Basic filesystem operations
// ============================================================

void FileManager::make_directory(
    const std::string& name
) {
    fs::path target = resolve_path(name);

    ensure_not_exists(
        target,
        "Directory"
    );

    fs::path parent = target.parent_path();

    if (!parent.empty()) {
        ensure_exists(parent, "Parent directory");

        if (!fs::is_directory(parent)) {
            throw std::runtime_error(
                "Parent is not a directory: "
                + parent.string()
            );
        }
    }

    if (!fs::create_directory(target)) {
        throw std::runtime_error(
            "Failed to create directory: "
            + target.string()
        );
    }

    std::cout
        << "Directory created: "
        << target.filename()
        << '\n';
}

void FileManager::create_file(
    const std::string& name
) {
    fs::path target = resolve_path(name);

    ensure_not_exists(
        target,
        "File"
    );

    fs::path parent = target.parent_path();

    if (!parent.empty()) {
        ensure_exists(parent, "Parent directory");

        if (!fs::is_directory(parent)) {
            throw std::runtime_error(
                "Parent is not a directory: "
                + parent.string()
            );
        }
    }

    std::ofstream file(target);

    if (!file) {
        throw std::runtime_error(
            "Failed to create file: "
            + target.string()
        );
    }

    file.close();

    std::cout
        << "File created: "
        << target.filename()
        << '\n';
}

void FileManager::rename_item(
    const std::string& old_name,
    const std::string& new_name
) {
    fs::path source = resolve_path(old_name);
    fs::path destination = resolve_path(new_name);

    ensure_exists(source, "Source");
    ensure_not_exists(destination, "Destination");

    if (is_same_path(source, destination)) {
        throw std::runtime_error(
            "Source and destination are the same."
        );
    }

    fs::rename(source, destination);

    std::cout
        << "Renamed successfully.\n";
}

void FileManager::copy_item(
    const std::string& source_name,
    const std::string& destination_name
) {
    fs::path source = resolve_path(source_name);
    fs::path destination = resolve_path(destination_name);

    ensure_exists(source, "Source");
    ensure_not_exists(destination, "Destination");

    if (is_same_path(source, destination)) {
        throw std::runtime_error(
            "Source and destination are the same."
        );
    }

    if (fs::is_directory(source)) {
        ensure_not_inside(source, destination);

        fs::copy(
            source,
            destination,
            fs::copy_options::recursive
        );
    }
    else {
        fs::copy_file(
            source,
            destination
        );
    }

    std::cout
        << "Copied successfully.\n";
}

void FileManager::move_item(
    const std::string& source_name,
    const std::string& destination_name
) {
    fs::path source = resolve_path(source_name);
    fs::path destination = resolve_path(destination_name);

    ensure_exists(source, "Source");
    ensure_not_exists(destination, "Destination");

    if (is_same_path(source, destination)) {
        throw std::runtime_error(
            "Source and destination are the same."
        );
    }

    if (fs::is_directory(source)) {
        ensure_not_inside(source, destination);
    }

    fs::rename(source, destination);

    std::cout
        << "Moved successfully.\n";
}

void FileManager::remove_item(
    const std::string& name
) {
    fs::path target = resolve_path(name);

    ensure_exists(target, "Target");
    ensure_not_dangerous_path(target);

    if (is_same_path(target, current_directory)) {
        throw std::runtime_error(
            "Cannot delete the current directory."
        );
    }

    if (fs::is_directory(target)) {

        std::cout
            << "\nWARNING: Recursive deletion\n"
            << "You are about to delete:\n"
            << "  " << target << '\n'
            << "\nThis will delete the directory and "
               "everything inside it.\n"
            << "Continue? (y/n): ";

        std::string answer;
        std::getline(std::cin, answer);

        if (
            answer != "y"
            && answer != "Y"
        ) {
            std::cout
                << "Deletion cancelled.\n";
            return;
        }

        std::error_code ec;

        fs::remove_all(target, ec);

        if (ec) {
            throw std::runtime_error(
                "Failed to recursively delete: "
                + std::string(ec.message())
            );
        }

        std::cout
            << "Directory deleted.\n";
    }
    else {
        std::error_code ec;

        fs::remove(target, ec);

        if (ec) {
            throw std::runtime_error(
                "Failed to delete: "
                + std::string(ec.message())
            );
        }

        std::cout
            << "Item deleted.\n";
    }
}

// ============================================================
// File information
// ============================================================

void FileManager::show_file_size(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(target, "File");

    if (!fs::is_regular_file(target)) {
        throw std::runtime_error(
            "Size is only available for regular files."
        );
    }

    std::cout
        << "Size: "
        << fs::file_size(target)
        << " bytes\n";
}

void FileManager::show_file_type(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(target, "Item");

    if (fs::is_symlink(target)) {
        std::cout << "Type: symbolic link\n";
    }
    else if (fs::is_directory(target)) {
        std::cout << "Type: directory\n";
    }
    else if (fs::is_regular_file(target)) {
        std::cout << "Type: regular file\n";
    }
    else {
        std::cout << "Type: other\n";
    }
}

void FileManager::show_modified_time(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(target, "Item");

    std::cout
        << "Modified: "
        << format_time(fs::last_write_time(target))
        << '\n';
}

void FileManager::show_info(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(target, "Item");

    std::cout
        << "Name: "
        << target.filename()
        << '\n';

    std::cout
        << "Path: "
        << target
        << '\n';

    show_file_type(name);

    if (
        fs::is_regular_file(target)
    ) {
        std::cout
            << "Size: "
            << fs::file_size(target)
            << " bytes\n";
    }

    std::cout
        << "Permissions: "
        << permission_string(target)
        << '\n';

    std::cout
        << "Modified: "
        << format_time(fs::last_write_time(target))
        << '\n';

    if (fs::is_symlink(target)) {
        std::cout
            << "Link target: "
            << fs::read_symlink(target)
            << '\n';
    }
}

// ============================================================
// Recursive operations
// ============================================================

void FileManager::show_tree(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(target, "Path");

    std::cout
        << target.filename().string()
        << '\n';

    if (!fs::is_directory(target)) {
        return;
    }

    print_tree_recursive(target, "");
}

void FileManager::show_directory_size(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(target, "Path");

    if (!fs::is_directory(target)) {
        throw std::runtime_error(
            "du requires a directory."
        );
    }

    std::uintmax_t total =
        calculate_directory_size(target);

    std::cout
        << "Total size: "
        << total
        << " bytes\n";
}

// ============================================================
// Search
// ============================================================

void FileManager::find_by_name(
    const std::string& name,
    const std::string& path
) const {
    fs::path root = resolve_path(path);

    ensure_exists(root, "Search path");

    bool found = false;

    try {
        for (
            const auto& entry :
            fs::recursive_directory_iterator(
                root,
                fs::directory_options::skip_permission_denied
            )
        ) {
            if (
                entry.path().filename().string()
                == name
            ) {
                std::cout
                    << entry.path()
                    << '\n';

                found = true;
            }
        }
    }
    catch (const fs::filesystem_error& error) {
        throw std::runtime_error(
            std::string("Search failed: ")
            + error.what()
        );
    }

    if (!found) {
        std::cout
            << "No matching files found.\n";
    }
}

void FileManager::find_by_extension(
    const std::string& extension,
    const std::string& path
) const {
    fs::path root = resolve_path(path);

    ensure_exists(root, "Search path");

    std::string normalized = extension;

    if (
        !normalized.empty()
        && normalized[0] != '.'
    ) {
        normalized =
            "." + normalized;
    }

    bool found = false;

    try {
        for (
            const auto& entry :
            fs::recursive_directory_iterator(
                root,
                fs::directory_options::skip_permission_denied
            )
        ) {
            if (
                entry.path().extension().string()
                == normalized
            ) {
                std::cout
                    << entry.path()
                    << '\n';

                found = true;
            }
        }
    }
    catch (const fs::filesystem_error& error) {
        throw std::runtime_error(
            std::string("Extension search failed: ")
            + error.what()
        );
    }

    if (!found) {
        std::cout
            << "No matching files found.\n";
    }
}

void FileManager::find_by_size(
    std::uintmax_t minimum_size,
    const std::string& path
) const {
    fs::path root = resolve_path(path);

    ensure_exists(root, "Search path");

    bool found = false;

    try {
        for (
            const auto& entry :
            fs::recursive_directory_iterator(
                root,
                fs::directory_options::skip_permission_denied
            )
        ) {
            std::error_code ec;

            if (
                entry.is_regular_file(ec)
                && !ec
            ) {
                std::uintmax_t size =
                    entry.file_size(ec);

                if (
                    !ec
                    && size >= minimum_size
                ) {
                    std::cout
                        << entry.path()
                        << " ("
                        << size
                        << " bytes)\n";

                    found = true;
                }
            }
        }
    }
    catch (const fs::filesystem_error& error) {
        throw std::runtime_error(
            std::string("Size search failed: ")
            + error.what()
        );
    }

    if (!found) {
        std::cout
            << "No matching files found.\n";
    }
}

// ============================================================
// Unix permissions and links
// ============================================================

void FileManager::show_permissions(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(target, "Item");

    std::cout
        << "Permissions: "
        << permission_string(target)
        << '\n';
}

void FileManager::change_permissions(
    const std::string& mode,
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(target, "Item");

    if (
        mode.length() != 3
    ) {
        throw std::invalid_argument(
            "Permission mode must contain exactly "
            "3 octal digits, e.g. 755."
        );
    }

    for (char character : mode) {
        if (
            character < '0'
            || character > '7'
        ) {
            throw std::invalid_argument(
                "Permission mode must contain "
                "only octal digits (0-7)."
            );
        }
    }

    unsigned long numeric_mode =
        std::stoul(mode, nullptr, 8);

    if (numeric_mode > 0777) {
        throw std::invalid_argument(
            "Permission mode must be between 000 and 777."
        );
    }

    if (
        ::chmod(
            target.c_str(),
            static_cast<mode_t>(numeric_mode)
        )
        != 0
    ) {
        throw std::runtime_error(
            "Failed to change permissions."
        );
    }

    std::cout
        << "Permissions changed to "
        << mode
        << ".\n";

    std::cout
        << "Permissions: "
        << permission_string(target)
        << '\n';
}

void FileManager::create_hard_link(
    const std::string& target_name,
    const std::string& link_name
) const {
    fs::path target = resolve_path(target_name);
    fs::path link_path = resolve_path(link_name);

    ensure_exists(target, "Target");
    ensure_not_exists(link_path, "Link");

    if (fs::is_directory(target)) {
        throw std::runtime_error(
            "Hard links to directories are not allowed."
        );
    }

    fs::create_hard_link(
        target,
        link_path
    );

    std::cout
        << "Hard link created.\n";
}

void FileManager::create_symbolic_link(
    const std::string& target_name,
    const std::string& link_name
) const {
    fs::path target = resolve_path(target_name);
    fs::path link_path = resolve_path(link_name);

    ensure_exists(target, "Target");
    ensure_not_exists(link_path, "Link");

    // Keep the target relative to the link when the
    // user supplied a relative target.
    fs::path stored_target;

    if (fs::path(target_name).is_absolute()) {
        stored_target = target;
    }
    else {
        stored_target = target_name;
    }

    fs::create_symlink(
        stored_target,
        link_path
    );

    std::cout
        << "Symbolic link created.\n";
}

void FileManager::show_link_target(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(target, "Link");

    if (!fs::is_symlink(target)) {
        throw std::runtime_error(
            "Not a symbolic link: "
            + target.string()
        );
    }

    std::cout
        << "Link target: "
        << fs::read_symlink(target)
        << '\n';
}

// ============================================================
// Helper functions
// ============================================================

std::string FileManager::format_time(
    const fs::file_time_type& file_time
) const {
    using namespace std::chrono;

    auto system_time =
        time_point_cast<
            system_clock::duration
        >(
            file_time
            - fs::file_time_type::clock::now()
            + system_clock::now()
        );

    std::time_t time =
        system_clock::to_time_t(system_time);

    std::tm* local_time =
        std::localtime(&time);

    if (local_time == nullptr) {
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
    if (path.empty()) {
        throw std::invalid_argument(
            "Path cannot be empty."
        );
    }

    fs::path input(path);

    if (input.is_absolute()) {
        return input.lexically_normal();
    }

    return (
        current_directory / input
    ).lexically_normal();
}

std::string FileManager::permission_string(
    const fs::path& path
) const {
    struct stat file_info {};

    if (
        ::lstat(
            path.c_str(),
            &file_info
        )
        != 0
    ) {
        return "?????????";
    }

    std::string result;

    if (S_ISDIR(file_info.st_mode)) {
        result += 'd';
    }
    else if (S_ISLNK(file_info.st_mode)) {
        result += 'l';
    }
    else {
        result += '-';
    }

    result +=
        (file_info.st_mode & S_IRUSR)
        ? 'r' : '-';

    result +=
        (file_info.st_mode & S_IWUSR)
        ? 'w' : '-';

    result +=
        (file_info.st_mode & S_IXUSR)
        ? 'x' : '-';

    result +=
        (file_info.st_mode & S_IRGRP)
        ? 'r' : '-';

    result +=
        (file_info.st_mode & S_IWGRP)
        ? 'w' : '-';

    result +=
        (file_info.st_mode & S_IXGRP)
        ? 'x' : '-';

    result +=
        (file_info.st_mode & S_IROTH)
        ? 'r' : '-';

    result +=
        (file_info.st_mode & S_IWOTH)
        ? 'w' : '-';

    result +=
        (file_info.st_mode & S_IXOTH)
        ? 'x' : '-';

    return result;
}

bool FileManager::is_hidden(
    const fs::path& path
) const {
    std::string name =
        path.filename().string();

    return (
        !name.empty()
        && name[0] == '.'
    );
}

void FileManager::print_tree_recursive(
    const fs::path& path,
    const std::string& prefix
) const {
    std::vector<fs::directory_entry> entries;

    std::error_code ec;

    for (
        const auto& entry :
        fs::directory_iterator(
            path,
            fs::directory_options::skip_permission_denied,
            ec
        )
    ) {
        if (ec) {
            break;
        }

        entries.push_back(entry);
    }

    std::sort(
        entries.begin(),
        entries.end(),
        [](const auto& a, const auto& b) {
            return a.path().filename().string()
                < b.path().filename().string();
        }
    );

    for (std::size_t i = 0;
         i < entries.size();
         ++i) {

        const auto& entry = entries[i];

        bool last =
            i == entries.size() - 1;

        std::cout
            << prefix
            << (last ? "└── " : "├── ")
            << entry.path().filename().string()
            << '\n';

        // Never recursively enter symbolic links.
        if (
            fs::is_directory(entry.path())
            && !fs::is_symlink(entry.path())
        ) {
            print_tree_recursive(
                entry.path(),
                prefix + (
                    last
                    ? "    "
                    : "│   "
                )
            );
        }
    }
}

std::uintmax_t FileManager::calculate_directory_size(
    const fs::path& path
) const {
    std::uintmax_t total = 0;

    std::error_code ec;

    for (
        const auto& entry :
        fs::recursive_directory_iterator(
            path,
            fs::directory_options::skip_permission_denied,
            ec
        )
    ) {
        if (ec) {
            break;
        }

        if (fs::is_symlink(entry.path())) {
            continue;
        }

        if (fs::is_regular_file(entry.path())) {

            std::error_code size_error;

            std::uintmax_t size =
                entry.file_size(size_error);

            if (!size_error) {
                total += size;
            }
        }
    }

    return total;
}

// ============================================================
// Phase 9 safety helpers
// ============================================================

void FileManager::ensure_exists(
    const fs::path& path,
    const std::string& description
) const {
    std::error_code ec;

    bool exists =
        fs::exists(
            path,
            ec
        );

    if (ec) {
        throw std::runtime_error(
            description
            + " cannot be accessed: "
            + ec.message()
        );
    }

    if (!exists) {
        throw std::runtime_error(
            description
            + " does not exist: "
            + path.string()
        );
    }
}

void FileManager::ensure_not_exists(
    const fs::path& path,
    const std::string& description
) const {
    std::error_code ec;

    bool exists =
        fs::exists(
            path,
            ec
        );

    // Important:
    // broken symlinks are not reported by exists(),
    // so check symlink status as well.
    if (
        !exists
        && fs::is_symlink(path)
    ) {
        exists = true;
    }

    if (ec) {
        throw std::runtime_error(
            description
            + " cannot be checked: "
            + ec.message()
        );
    }

    if (exists) {
        throw std::runtime_error(
            description
            + " already exists: "
            + path.string()
        );
    }
}

void FileManager::ensure_not_dangerous_path(
    const fs::path& path
) const {
    fs::path normalized =
        path.lexically_normal();

    if (
        normalized == "."
        || normalized == ".."
    ) {
        throw std::runtime_error(
            "Refusing to delete '.' or '..'."
        );
    }

    fs::path absolute_path;

    try {
        absolute_path =
            fs::absolute(normalized)
            .lexically_normal();
    }
    catch (...) {
        throw std::runtime_error(
            "Could not safely resolve path."
        );
    }

    fs::path root =
        absolute_path.root_path();

    if (absolute_path == root) {
        throw std::runtime_error(
            "Refusing to delete the filesystem root."
        );
    }

    if (absolute_path == current_directory) {
        throw std::runtime_error(
            "Refusing to delete the current directory."
        );
    }
}

void FileManager::ensure_not_inside(
    const fs::path& source,
    const fs::path& destination
) const {
    fs::path source_absolute =
        fs::absolute(source)
        .lexically_normal();

    fs::path destination_absolute =
        fs::absolute(destination)
        .lexically_normal();

    auto source_it =
        source_absolute.begin();

    auto destination_it =
        destination_absolute.begin();

    while (
        source_it != source_absolute.end()
        && destination_it != destination_absolute.end()
        && *source_it == *destination_it
    ) {
        ++source_it;
        ++destination_it;
    }

    if (
        source_it == source_absolute.end()
    ) {
        throw std::runtime_error(
            "Cannot copy or move a directory "
            "into itself or one of its children."
        );
    }
}

bool FileManager::is_same_path(
    const fs::path& first,
    const fs::path& second
) const {
    std::error_code ec1;
    std::error_code ec2;

    fs::path first_canonical =
        fs::weakly_canonical(
            first,
            ec1
        );

    fs::path second_canonical =
        fs::weakly_canonical(
            second,
            ec2
        );

    if (!ec1 && !ec2) {
        return first_canonical
            == second_canonical;
    }

    return (
        first.lexically_normal()
        == second.lexically_normal()
    );
}