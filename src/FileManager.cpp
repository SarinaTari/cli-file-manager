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
    : current_directory(fs::current_path()),
      undo_manager(history_manager) {

}

void FileManager::list_directory(
    bool show_hidden,
    const std::string& sort_option,
    const std::string& filter
) const {
    std::vector<fs::directory_entry> entries;

    std::error_code ec;

    fs::directory_iterator iterator(
        current_directory,
        fs::directory_options::skip_permission_denied,
        ec
    );

    if (ec) {
        throw std::runtime_error(
            "Unable to read directory: " + ec.message()
        );
    }

    for (const auto& entry : iterator) {
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
        filter != "all"
        && filter != "files"
        && filter != "dirs"
    ) {
        throw std::invalid_argument(
            "Invalid filter."
        );
    }

    if (
        sort_option != "name"
        && sort_option != "size"
        && sort_option != "name-desc"
        && sort_option != "size-desc"
    ) {
        throw std::invalid_argument(
            "Invalid sort option."
        );
    }

    std::sort(
        entries.begin(),
        entries.end(),
        [&](const fs::directory_entry& a,
            const fs::directory_entry& b) {

            if (
                sort_option == "name"
                || sort_option == "name-desc"
            ) {
                std::string a_name =
                    a.path().filename().string();

                std::string b_name =
                    b.path().filename().string();

                if (sort_option == "name") {
                    return a_name < b_name;
                }

                return a_name > b_name;
            }

            std::error_code a_ec;
            std::error_code b_ec;

            std::uintmax_t a_size = 0;
            std::uintmax_t b_size = 0;

            if (a.is_regular_file(a_ec)) {
                a_size = a.file_size(a_ec);
            }

            if (b.is_regular_file(b_ec)) {
                b_size = b.file_size(b_ec);
            }

            if (sort_option == "size") {
                return a_size < b_size;
            }

            return a_size > b_size;
        }
    );

    for (const auto& entry : entries) {
        std::string type;

        std::error_code ec;

        if (entry.is_symlink(ec)) {
            type = "[LINK]";
        }
        else if (entry.is_directory(ec)) {
            type = "[DIR] ";
        }
        else {
            type = "[FILE]";
        }

        std::cout
            << type
            << " "
            << entry.path().filename().string();

        if (entry.is_regular_file(ec)) {
            std::uintmax_t size = entry.file_size(ec);

            if (!ec) {
                std::cout
                    << " (" << size << " bytes)";
            }
        }

        std::cout << '\n';
    }
}

void FileManager::print_working_directory() const {
    std::cout
        << "Current directory: \""
        << current_directory
        << "\"\n";
}

fs::path FileManager::get_current_directory() const {
    return current_directory;
}

void FileManager::change_directory(
    const std::string& name
) {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Directory does not exist"
    );

    std::error_code ec;

    if (!fs::is_directory(target, ec)) {
        throw std::runtime_error(
            "Not a directory: " + target.string()
        );
    }

    fs::path canonical =
        fs::weakly_canonical(target, ec);

    if (ec) {
        throw std::runtime_error(
            "Unable to resolve directory: "
            + ec.message()
        );
    }

    current_directory = canonical;
}

void FileManager::go_back() {
    fs::path parent =
        current_directory.parent_path();

    if (parent == current_directory) {
        return;
    }

    current_directory = parent;
}

void FileManager::make_directory(
    const std::string& name
) {
    fs::path target = resolve_path(name);

    ensure_not_exists(
        target,
        "Directory already exists"
    );

    fs::path parent = target.parent_path();

    if (!parent.empty()) {
        ensure_exists(
            parent,
            "Parent directory does not exist"
        );
    }

    std::error_code ec;

    if (!fs::create_directory(target, ec)) {
        if (ec) {
            throw std::runtime_error(
                "Unable to create directory: "
                + ec.message()
            );
        }

        throw std::runtime_error(
            "Unable to create directory."
        );
    }

    std::cout
        << "Directory created: \""
        << target.filename()
        << "\"\n";

    history_manager.record(
        "create_directory",
        target.string()
    );
}

void FileManager::create_file(
    const std::string& name
) {
    fs::path target = resolve_path(name);

    ensure_not_exists(
        target,
        "File already exists"
    );

    fs::path parent = target.parent_path();

    if (!parent.empty()) {
        ensure_exists(
            parent,
            "Parent directory does not exist"
        );
    }

    std::ofstream file(target);

    if (!file) {
        throw std::runtime_error(
            "Unable to create file: "
            + target.string()
        );
    }

    std::cout
        << "File created: \""
        << target.filename()
        << "\"\n";

    history_manager.record(
        "create_file",
        target.string()
    );
}

void FileManager::rename_item(
    const std::string& old_name,
    const std::string& new_name
) {
    fs::path source = resolve_path(old_name);
    fs::path destination = resolve_path(new_name);

    ensure_exists(
        source,
        "Source does not exist"
    );

    ensure_not_exists(
        destination,
        "Destination already exists"
    );

    fs::path parent = destination.parent_path();

    if (!parent.empty()) {
        ensure_exists(
            parent,
            "Destination parent does not exist"
        );
    }

    if (is_same_path(source, destination)) {
        throw std::runtime_error(
            "Source and destination are the same."
        );
    }

    std::error_code ec;

    fs::rename(source, destination, ec);

    if (ec) {
        throw std::runtime_error(
            "Unable to rename item: "
            + ec.message()
        );
    }

    std::cout
        << "Renamed \""
        << source.filename()
        << "\" to \""
        << destination.filename()
        << "\"\n";

    history_manager.record(
        "rename",
        source.string(),
        destination.string()
    );
}

void FileManager::copy_item(
    const std::string& source,
    const std::string& destination
) {
    fs::path source_path =
        resolve_path(source);

    fs::path destination_path =
        resolve_path(destination);

    ensure_exists(
        source_path,
        "Source does not exist"
    );

    /*
     * Check this BEFORE checking whether the
     * destination already exists.
     *
     * This gives the user the more meaningful
     * error when copying a directory into itself.
     */
    ensure_not_inside(
        source_path,
        destination_path
    );

    ensure_not_exists(
        destination_path,
        "Destination already exists"
    );

    std::error_code ec;

    if (fs::is_directory(source_path, ec)) {
        fs::copy(
            source_path,
            destination_path,
            fs::copy_options::recursive,
            ec
        );
    }
    else {
        fs::copy_file(
            source_path,
            destination_path,
            fs::copy_options::none,
            ec
        );
    }

    if (ec) {
        throw std::runtime_error(
            "Unable to copy item: "
            + ec.message()
        );
    }

    std::cout
        << "Copied: \""
        << source_path.filename()
        << "\" -> \""
        << destination_path
        << "\"\n";
}

void FileManager::move_item(
    const std::string& source,
    const std::string& destination
) {
    fs::path source_path =
        resolve_path(source);

    fs::path destination_path =
        resolve_path(destination);

    ensure_exists(
        source_path,
        "Source does not exist"
    );

    /*
     * Check this BEFORE checking whether the
     * destination already exists.
     */
    ensure_not_inside(
        source_path,
        destination_path
    );

    ensure_not_exists(
        destination_path,
        "Destination already exists"
    );

    std::error_code ec;

    fs::rename(
        source_path,
        destination_path,
        ec
    );

    if (ec) {
        throw std::runtime_error(
            "Unable to move item: "
            + ec.message()
        );
    }

    std::cout
        << "Moved: \""
        << source_path.filename()
        << "\" -> \""
        << destination_path
        << "\"\n";

    history_manager.record(
        "move",
        source_path.string(),
        destination_path.string()
    );
}

void FileManager::remove_item(
    const std::string& name
) {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    ensure_not_dangerous_path(target);

    std::error_code ec;

    if (fs::is_directory(target, ec)) {
        std::cout
            << "Warning: this will recursively delete directory \""
            << target.filename()
            << "\".\n";

        std::cout
            << "Are you sure? [y/N]: ";

        std::string confirmation;
        std::getline(std::cin, confirmation);

        if (
            confirmation != "y"
            && confirmation != "Y"
        ) {
            std::cout
                << "Deletion cancelled.\n";

            return;
        }

        fs::remove_all(target, ec);

        if (ec) {
            throw std::runtime_error(
                "Unable to remove directory: "
                + ec.message()
            );
        }
    }
    else {
        fs::remove(target, ec);

        if (ec) {
            throw std::runtime_error(
                "Unable to remove item: "
                + ec.message()
            );
        }
    }

    std::cout
        << "Removed: \""
        << target.filename()
        << "\"\n";
}

void FileManager::safe_remove(
    const std::string& path
) const {
    if (path == "." || path == "..") {
        throw std::runtime_error(
            "Refusing to delete a dangerous path: " +
            path
        );
    }

    SafeDelete::remove(
        resolve_path(path)
    );
}

void FileManager::show_file_size(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    std::error_code ec;

    if (!fs::is_regular_file(target, ec)) {
        throw std::runtime_error(
            "Not a regular file: " + target.string()
        );
    }

    std::uintmax_t size =
        fs::file_size(target, ec);

    if (ec) {
        throw std::runtime_error(
            "Unable to read file size: "
            + ec.message()
        );
    }

    std::cout
        << "Size: "
        << size
        << " bytes\n";
}

void FileManager::show_file_type(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    std::error_code ec;

    if (fs::is_directory(target, ec)) {
        std::cout << "Type: Directory\n";
    }
    else if (fs::is_symlink(target, ec)) {
        std::cout << "Type: Symbolic Link\n";
    }
    else if (fs::is_regular_file(target, ec)) {
        std::cout << "Type: Regular File\n";
    }
    else {
        std::cout << "Type: Other\n";
    }
}

void FileManager::show_modified_time(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    std::error_code ec;

    auto time = fs::last_write_time(
        target,
        ec
    );

    if (ec) {
        throw std::runtime_error(
            "Unable to read modification time: "
            + ec.message()
        );
    }

    std::cout
        << "Modified: "
        << format_time(time)
        << "\n";
}

void FileManager::show_info(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    std::cout
        << "Name: "
        << target.filename()
        << "\n";

    std::cout
        << "Path: "
        << target
        << "\n";

    std::cout
        << "Type: ";

    show_file_type(name);

    std::error_code ec;

    if (fs::is_regular_file(target, ec)) {
        std::uintmax_t size =
            fs::file_size(target, ec);

        if (!ec) {
            std::cout
                << "Size: "
                << size
                << " bytes\n";
        }
    }

    std::cout
        << "Permissions: "
        << permission_string(target)
        << "\n";

    auto time =
        fs::last_write_time(target, ec);

    if (!ec) {
        std::cout
            << "Modified: "
            << format_time(time)
            << "\n";
    }

    if (fs::is_symlink(target, ec)) {
        std::error_code link_ec;

        fs::path link_target =
            fs::read_symlink(target, link_ec);

        if (!link_ec) {
            std::cout
                << "Link target: "
                << link_target
                << "\n";
        }
    }
}

void FileManager::show_tree(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    std::cout
        << target.filename().string()
        << "\n";

    print_tree_recursive(
        target,
        ""
    );
}

void FileManager::show_directory_size(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    if (!fs::is_directory(target)) {
        throw std::runtime_error(
            "Not a directory: " + target.string()
        );
    }

    std::uintmax_t size =
        calculate_directory_size(target);

    std::cout
        << "Directory: "
        << target
        << "\n";

    std::cout
        << "Total size: "
        << size
        << " bytes\n";
}

void FileManager::find_by_name(
    const std::string& name,
    const std::string& path
) const {
    fs::path root = resolve_path(path);

    ensure_exists(
        root,
        "Search path does not exist"
    );

    bool found = false;

    std::error_code ec;

    fs::recursive_directory_iterator iterator(
        root,
        fs::directory_options::skip_permission_denied,
        ec
    );

    for (
        ;
        iterator != fs::recursive_directory_iterator();
        iterator.increment(ec)
    ) {
        if (ec) {
            ec.clear();
            continue;
        }

        if (
            iterator->path().filename().string()
            == name
        ) {
            std::cout
                << iterator->path()
                << "\n";

            found = true;
        }
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

    ensure_exists(
        root,
        "Search path does not exist"
    );

    std::string normalized = extension;

    if (
        !normalized.empty()
        && normalized[0] != '.'
    ) {
        normalized =
            "." + normalized;
    }

    bool found = false;

    std::error_code ec;

    fs::recursive_directory_iterator iterator(
        root,
        fs::directory_options::skip_permission_denied,
        ec
    );

    for (
        ;
        iterator != fs::recursive_directory_iterator();
        iterator.increment(ec)
    ) {
        if (ec) {
            ec.clear();
            continue;
        }

        if (!iterator->is_regular_file(ec)) {
            continue;
        }

        if (
            iterator->path().extension().string()
            == normalized
        ) {
            std::cout
                << iterator->path()
                << "\n";

            found = true;
        }
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

    ensure_exists(
        root,
        "Search path does not exist"
    );

    bool found = false;

    std::error_code ec;

    fs::recursive_directory_iterator iterator(
        root,
        fs::directory_options::skip_permission_denied,
        ec
    );

    for (
        ;
        iterator != fs::recursive_directory_iterator();
        iterator.increment(ec)
    ) {
        if (ec) {
            ec.clear();
            continue;
        }

        if (!iterator->is_regular_file(ec)) {
            continue;
        }

        std::uintmax_t size =
            iterator->file_size(ec);

        if (ec) {
            ec.clear();
            continue;
        }

        if (size >= minimum_size) {
            std::cout
                << iterator->path()
                << " ("
                << size
                << " bytes)\n";

            found = true;
        }
    }

    if (!found) {
        std::cout
            << "No matching files found.\n";
    }
}

void FileManager::show_permissions(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    std::cout
        << "Permissions: "
        << permission_string(target)
        << "\n";
}

void FileManager::change_permissions(
    const std::string& mode,
    const std::string& name
) const {
    if (mode.size() != 3) {
        throw std::invalid_argument(
            "Permission mode must contain exactly 3 octal digits."
        );
    }

    for (char character : mode) {
        if (
            character < '0'
            || character > '7'
        ) {
            throw std::invalid_argument(
                "Permission mode must contain only octal digits (0-7)."
            );
        }
    }

    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    mode_t permissions =
        static_cast<mode_t>(
            std::stoul(mode, nullptr, 8)
        );

    if (::chmod(target.c_str(), permissions) != 0) {
        throw std::runtime_error(
            "Unable to change permissions."
        );
    }

    std::cout
        << "Permissions changed to "
        << mode
        << "\n";
}

void FileManager::create_hard_link(
    const std::string& target,
    const std::string& link_name
) const {
    fs::path target_path =
        resolve_path(target);

    fs::path link_path =
        resolve_path(link_name);

    ensure_exists(
        target_path,
        "Target does not exist"
    );

    ensure_not_exists(
        link_path,
        "Link already exists"
    );

    if (fs::is_directory(target_path)) {
        throw std::runtime_error(
            "Hard links to directories are not supported."
        );
    }

    std::error_code ec;

    fs::create_hard_link(
        target_path,
        link_path,
        ec
    );

    if (ec) {
        throw std::runtime_error(
            "Unable to create hard link: "
            + ec.message()
        );
    }

    std::cout
        << "Hard link created: \""
        << link_path.filename()
        << "\"\n";
}

void FileManager::create_symbolic_link(
    const std::string& target,
    const std::string& link_name
) const {
    fs::path target_path =
        resolve_path(target);

    fs::path link_path =
        resolve_path(link_name);

    ensure_exists(
        target_path,
        "Target does not exist"
    );

    ensure_not_exists(
        link_path,
        "Link already exists"
    );

    fs::path stored_target;

    if (fs::path(target).is_absolute()) {
        stored_target = target_path;
    }
    else {
        stored_target = fs::path(target);
    }

    std::error_code ec;

    fs::create_symlink(
        stored_target,
        link_path,
        ec
    );

    if (ec) {
        throw std::runtime_error(
            "Unable to create symbolic link: "
            + ec.message()
        );
    }

    std::cout
        << "Symbolic link created: \""
        << link_path.filename()
        << "\"\n";
}

void FileManager::show_link_target(
    const std::string& name
) const {
    fs::path target = resolve_path(name);

    ensure_exists(
        target,
        "Path does not exist"
    );

    if (!fs::is_symlink(target)) {
        throw std::runtime_error(
            "Not a symbolic link: "
            + target.string()
        );
    }

    std::error_code ec;

    fs::path link_target =
        fs::read_symlink(target, ec);

    if (ec) {
        throw std::runtime_error(
            "Unable to read symbolic link: "
            + ec.message()
        );
    }

    std::cout
        << link_target
        << "\n";
}

std::string FileManager::format_time(
    const fs::file_time_type& file_time
) const {
    using namespace std::chrono;

    auto system_time =
        time_point_cast<system_clock::duration>(
            file_time
            - fs::file_time_type::clock::now()
            + system_clock::now()
        );

    std::time_t time =
        system_clock::to_time_t(system_time);

    std::tm local_time{};

    localtime_r(
        &time,
        &local_time
    );

    std::ostringstream output;

    output
        << std::put_time(
            &local_time,
            "%Y-%m-%d %H:%M:%S"
        );

    return output.str();
}

fs::path FileManager::resolve_path(
    const std::string& path
) const {
    fs::path input(path);

    if (input.is_absolute()) {
        return input;
    }

    return current_directory / input;
}

std::string FileManager::permission_string(
    const fs::path& path
) const {
    struct stat information{};

    if (
        ::lstat(
            path.c_str(),
            &information
        ) != 0
    ) {
        throw std::runtime_error(
            "Unable to read permissions."
        );
    }

    std::string result;

    if (S_ISDIR(information.st_mode)) {
        result += 'd';
    }
    else if (S_ISLNK(information.st_mode)) {
        result += 'l';
    }
    else {
        result += '-';
    }

    result +=
        (information.st_mode & S_IRUSR) ? 'r' : '-';

    result +=
        (information.st_mode & S_IWUSR) ? 'w' : '-';

    result +=
        (information.st_mode & S_IXUSR) ? 'x' : '-';

    result +=
        (information.st_mode & S_IRGRP) ? 'r' : '-';

    result +=
        (information.st_mode & S_IWGRP) ? 'w' : '-';

    result +=
        (information.st_mode & S_IXGRP) ? 'x' : '-';

    result +=
        (information.st_mode & S_IROTH) ? 'r' : '-';

    result +=
        (information.st_mode & S_IWOTH) ? 'w' : '-';

    result +=
        (information.st_mode & S_IXOTH) ? 'x' : '-';

    return result;
}

bool FileManager::is_hidden(
    const fs::path& path
) const {
    std::string name =
        path.filename().string();

    return !name.empty()
        && name[0] == '.';
}

void FileManager::print_tree_recursive(
    const fs::path& path,
    const std::string& prefix
) const {
    std::error_code ec;

    if (!fs::is_directory(path, ec)) {
        return;
    }

    std::vector<fs::directory_entry> entries;

    for (
        fs::directory_iterator iterator(
            path,
            fs::directory_options::skip_permission_denied,
            ec
        );
        !ec && iterator != fs::directory_iterator();
        iterator.increment(ec)
    ) {
        if (iterator->is_symlink(ec)) {
            entries.push_back(*iterator);
            continue;
        }

        entries.push_back(*iterator);
    }

    std::sort(
        entries.begin(),
        entries.end(),
        [](const fs::directory_entry& a,
           const fs::directory_entry& b) {
            return a.path().filename().string()
                < b.path().filename().string();
        }
    );

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const auto& entry = entries[i];

        bool last =
            i == entries.size() - 1;

        std::cout
            << prefix
            << (last ? "└── " : "├── ")
            << entry.path().filename().string()
            << "\n";

        if (entry.is_directory(ec)
            && !entry.is_symlink(ec)) {

            print_tree_recursive(
                entry.path(),
                prefix + (last ? "    " : "│   ")
            );
        }
    }
}

std::uintmax_t FileManager::calculate_directory_size(
    const fs::path& path
) const {
    std::uintmax_t total = 0;

    std::error_code ec;

    fs::recursive_directory_iterator iterator(
        path,
        fs::directory_options::skip_permission_denied,
        ec
    );

    for (
        ;
        iterator != fs::recursive_directory_iterator();
        iterator.increment(ec)
    ) {
        if (ec) {
            ec.clear();
            continue;
        }

        if (iterator->is_symlink(ec)) {
            continue;
        }

        if (iterator->is_regular_file(ec)) {
            std::uintmax_t size =
                iterator->file_size(ec);

            if (!ec) {
                total += size;
            }
        }
    }

    return total;
}

void FileManager::ensure_exists(
    const fs::path& path,
    const std::string& description
) const {
    std::error_code ec;

    bool exists =
        fs::exists(path, ec);

    if (ec) {
        throw std::runtime_error(
            description + ": " + ec.message()
        );
    }

    if (!exists) {
        throw std::runtime_error(
            description + ": " + path.string()
        );
    }
}

void FileManager::ensure_not_exists(
    const fs::path& path,
    const std::string& description
) const {
    std::error_code ec;

    bool exists =
        fs::exists(path, ec);

    if (exists) {
        throw std::runtime_error(
            description + ": " + path.string()
        );
    }

    if (ec) {
        ec.clear();

        struct stat information{};

        if (
            ::lstat(
                path.c_str(),
                &information
            ) == 0
        ) {
            throw std::runtime_error(
                description + ": " + path.string()
            );
        }
    }
}

void FileManager::ensure_not_dangerous_path(
    const fs::path& path
) const {
    fs::path absolute_path =
        fs::absolute(path);

    std::error_code ec;

    fs::path canonical_path =
        fs::weakly_canonical(
            absolute_path,
            ec
        );

    if (ec) {
        canonical_path =
            absolute_path.lexically_normal();
    }

    fs::path current =
        fs::weakly_canonical(
            current_directory,
            ec
        );

    if (ec) {
        current =
            fs::absolute(
                current_directory
            ).lexically_normal();
    }

    /*
     * Never allow removing the current directory.
     */
    if (canonical_path == current) {
        throw std::runtime_error(
            "Refusing to remove the current directory."
        );
    }

    /*
     * Never allow removing the parent of
     * the current directory.
     */
    if (canonical_path == current.parent_path()) {
        throw std::runtime_error(
            "Refusing to remove the parent directory."
        );
    }

    /*
     * Never allow removing the filesystem root.
     */
    if (canonical_path == fs::path("/")) {
        throw std::runtime_error(
            "Refusing to remove the filesystem root."
        );
    }
}

void FileManager::ensure_not_inside(
    const fs::path& source,
    const fs::path& destination
) const {
    fs::path source_normalized =
        source.lexically_normal();

    fs::path destination_normalized =
        destination.lexically_normal();

    auto source_it =
        source_normalized.begin();

    auto destination_it =
        destination_normalized.begin();

    for (
        ;
        source_it != source_normalized.end();
        ++source_it,
        ++destination_it
    ) {
        if (
            destination_it
            == destination_normalized.end()
        ) {
            return;
        }

        if (*source_it != *destination_it) {
            return;
        }
    }

    throw std::runtime_error(
        "Destination cannot be inside the source directory."
    );
}

bool FileManager::is_same_path(
    const fs::path& first,
    const fs::path& second
) const {
    std::error_code first_ec;
    std::error_code second_ec;

    fs::path first_canonical =
        fs::weakly_canonical(
            first,
            first_ec
        );

    fs::path second_canonical =
        fs::weakly_canonical(
            second,
            second_ec
        );

    if (!first_ec && !second_ec) {
        return first_canonical == second_canonical;
    }

    return first.lexically_normal()
        == second.lexically_normal();
}

void FileManager::show_history() const {
    history_manager.show();
}

void FileManager::undo() {
    undo_manager.undo(
        current_directory
    );
}

void FileManager::create_snapshot() {
    snapshot_manager.create(
        current_directory
    );
}

void FileManager::show_snapshot() const {
    snapshot_manager.show();
}

void FileManager::show_snapshot_diff() {
    snapshot_manager.diff(
        current_directory
    );
}

void FileManager::clear_snapshot() {
    snapshot_manager.clear();

    std::cout
        << "Snapshot cleared.\n";
}

void FileManager::show_project_dashboard(
    const std::string& path
) const {
    ProjectDashboard::show(
        resolve_path(path)
    );
}