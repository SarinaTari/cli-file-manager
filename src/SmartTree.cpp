#include "SmartTree.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <vector>

namespace fs = std::filesystem;

std::string SmartTree::classify_directory(
    const fs::path& path
) {
    const std::string name =
        path.filename().string();

    if (name == "src") {
        return "SOURCE";
    }

    if (
        name == "include"
        || name == "headers"
    ) {
        return "HEADERS";
    }

    if (
        name == "tests"
        || name == "test"
    ) {
        return "TESTS";
    }

    if (
        name == "docs"
        || name == "documentation"
    ) {
        return "DOCUMENTATION";
    }

    if (name == "build") {
        return "BUILD OUTPUT";
    }

    if (name == ".git") {
        return "GIT METADATA";
    }

    if (
        name == "scripts"
        || name == "script"
    ) {
        return "SCRIPTS";
    }

    return "DIRECTORY";
}

std::string SmartTree::classify_file(
    const fs::path& path
) {
    const std::string name =
        path.filename().string();

    const std::string extension =
        path.extension().string();

    if (name == "CMakeLists.txt") {
        return "BUILD SYSTEM";
    }

    if (
        name == "Makefile"
        || name == "makefile"
    ) {
        return "BUILD SYSTEM";
    }

    if (
        name == "README"
        || name == "README.md"
        || name == "README.txt"
    ) {
        return "DOCUMENTATION";
    }

    if (
        name == ".gitignore"
        || name == ".gitattributes"
        || name == ".gitmodules"
    ) {
        return "GIT CONFIG";
    }

    if (
        name == "main.cpp"
        || name == "main.cc"
        || name == "main.cxx"
        || name == "main.c"
    ) {
        return "ENTRY POINT";
    }

    if (
        extension == ".cpp"
        || extension == ".cc"
        || extension == ".cxx"
        || extension == ".c"
    ) {
        return "SOURCE";
    }

    if (
        extension == ".h"
        || extension == ".hpp"
        || extension == ".hh"
        || extension == ".hxx"
    ) {
        return "INTERFACE";
    }

    if (extension == ".py") {
        return "PYTHON";
    }

    if (extension == ".js") {
        return "JAVASCRIPT";
    }

    if (extension == ".ts") {
        return "TYPESCRIPT";
    }

    if (extension == ".rs") {
        return "RUST";
    }

    if (extension == ".go") {
        return "GO";
    }

    if (
        extension == ".sh"
        || extension == ".bash"
    ) {
        return "SHELL SCRIPT";
    }

    if (extension == ".sql") {
        return "SQL";
    }

    if (extension == ".json") {
        return "CONFIG / DATA";
    }

    if (
        extension == ".yaml"
        || extension == ".yml"
    ) {
        return "CONFIGURATION";
    }

    if (extension == ".xml") {
        return "CONFIG / DATA";
    }

    if (extension == ".md") {
        return "DOCUMENTATION";
    }

    return "FILE";
}

std::string SmartTree::classify_path(
    const fs::path& path
) {
    std::error_code error;

    if (fs::is_directory(path, error)) {
        return classify_directory(path);
    }

    if (fs::is_regular_file(path, error)) {
        return classify_file(path);
    }

    return "SPECIAL";
}

void SmartTree::print_tree(
    const fs::path& path,
    const std::string& prefix,
    bool is_last,
    bool is_root
) {
    std::error_code error;

    const std::string name =
        is_root
            ? path.filename().string()
            : path.filename().string();

    const std::string connector =
        is_root
            ? ""
            : (is_last ? "└── " : "├── ");

    std::cout
        << prefix
        << connector
        << name;

    std::cout
        << "  ["
        << classify_path(path)
        << "]\n";

    if (!fs::is_directory(path, error)) {
        return;
    }

    std::vector<fs::directory_entry> entries;

    for (
        fs::directory_iterator iterator(
            path,
            fs::directory_options::skip_permission_denied,
            error
        );
        !error && iterator != fs::directory_iterator();
        iterator.increment(error)
    ) {
        entries.push_back(*iterator);
    }

    std::sort(
        entries.begin(),
        entries.end(),
        [](const fs::directory_entry& a,
           const fs::directory_entry& b) {

            std::error_code error_a;
            std::error_code error_b;

            const bool a_directory =
                a.is_directory(error_a);

            const bool b_directory =
                b.is_directory(error_b);

            if (a_directory != b_directory) {
                return a_directory > b_directory;
            }

            return a.path().filename().string()
                < b.path().filename().string();
        }
    );

    for (std::size_t index = 0;
         index < entries.size();
         ++index) {

        const bool child_is_last =
            index == entries.size() - 1;

        const std::string child_prefix =
            is_root
                ? ""
                : prefix +
                    (is_last ? "    " : "│   ");

        print_tree(
            entries[index].path(),
            child_prefix,
            child_is_last,
            false
        );
    }
}

void SmartTree::show(
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
            "tree --smart requires a directory: " +
            path.string()
        );
    }

    std::cout
        << "\n========================================\n"
        << "          SMART PROJECT TREE\n"
        << "========================================\n\n";

    print_tree(
        path,
        "",
        true,
        true
    );

    std::cout
        << "\n========================================\n";
}