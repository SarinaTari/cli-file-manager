#include "StorageAnalyzer.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <vector>

namespace {

struct FileInfo {
    fs::path path;
    std::uintmax_t size;
};

}

void StorageAnalyzer::analyze(
    const fs::path& root
) {
    std::vector<FileInfo> files;

    std::uintmax_t total_size = 0;
    std::size_t directory_count = 0;
    std::size_t file_count = 0;

    std::error_code ec;

    if (!fs::exists(root, ec)) {
        throw std::runtime_error(
            "Path does not exist: "
            + root.string()
        );
    }

    fs::recursive_directory_iterator iterator(
        root,
        fs::directory_options::skip_permission_denied,
        ec
    );

    fs::recursive_directory_iterator end;

    while (iterator != end) {
        if (ec) {
            ec.clear();
            iterator.increment(ec);
            continue;
        }

        const auto& entry = *iterator;

        if (entry.is_directory(ec)) {
            ++directory_count;
        }
        else if (entry.is_regular_file(ec)) {
            ++file_count;

            auto size =
                entry.file_size(ec);

            if (!ec) {
                total_size += size;

                files.push_back({
                    entry.path(),
                    size
                });
            }
        }

        iterator.increment(ec);
    }

    std::sort(
        files.begin(),
        files.end(),
        [](const FileInfo& a, const FileInfo& b) {
            return a.size > b.size;
        }
    );

    std::cout
        << "Storage analysis for: "
        << root
        << "\n\n";

    std::cout
        << "Directories: "
        << directory_count
        << "\n";

    std::cout
        << "Files: "
        << file_count
        << "\n";

    std::cout
        << "Total size: "
        << total_size
        << " bytes\n";

    std::cout
        << "\nLargest files:\n";

    std::size_t limit =
        std::min<std::size_t>(
            10,
            files.size()
        );

    for (
        std::size_t i = 0;
        i < limit;
        ++i
    ) {
        std::cout
            << "  "
            << (i + 1)
            << ". "
            << files[i].path
            << " ("
            << files[i].size
            << " bytes)\n";
    }
}