#include "DuplicateDetector.h"

#include <fstream>
#include <iostream>
#include <map>
#include <system_error>
#include <vector>

bool DuplicateDetector::files_equal(
    const fs::path& first,
    const fs::path& second
) {
    std::ifstream first_file(
        first,
        std::ios::binary
    );

    std::ifstream second_file(
        second,
        std::ios::binary
    );

    if (!first_file || !second_file) {
        return false;
    }

    char first_buffer[8192];
    char second_buffer[8192];

    while (true) {
        first_file.read(
            first_buffer,
            sizeof(first_buffer)
        );

        second_file.read(
            second_buffer,
            sizeof(second_buffer)
        );

        std::streamsize first_count =
            first_file.gcount();

        std::streamsize second_count =
            second_file.gcount();

        if (first_count != second_count) {
            return false;
        }

        if (first_count == 0) {
            return true;
        }

        for (
            std::streamsize i = 0;
            i < first_count;
            ++i
        ) {
            if (
                first_buffer[i]
                != second_buffer[i]
            ) {
                return false;
            }
        }
    }
}

void DuplicateDetector::collect_files(
    const fs::path& root,
    std::vector<fs::path>& files
) {
    std::error_code ec;

    if (fs::is_regular_file(root, ec)) {
        files.push_back(root);
        return;
    }

    if (!fs::is_directory(root, ec)) {
        return;
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

        const fs::directory_entry& entry =
            *iterator;

        std::error_code entry_ec;

        if (
            entry.is_regular_file(entry_ec)
            && !entry.is_symlink(entry_ec)
        ) {
            files.push_back(
                entry.path()
            );
        }

        iterator.increment(ec);
    }
}

void DuplicateDetector::find_duplicates(
    const fs::path& root
) {
    std::error_code ec;

    if (!fs::exists(root, ec)) {
        std::cout
            << "No duplicate files found.\n";

        return;
    }

    std::vector<fs::path> files;

    collect_files(
        root,
        files
    );

    if (files.size() < 2) {
        std::cout
            << "No duplicate files found.\n";

        return;
    }

    /*
     * First group files by size.
     *
     * Two files with different sizes cannot be
     * duplicates, so there is no reason to compare
     * their contents.
     */
    std::map<
        std::uintmax_t,
        std::vector<fs::path>
    > size_groups;

    for (const fs::path& file : files) {
        std::error_code size_ec;

        std::uintmax_t size =
            fs::file_size(
                file,
                size_ec
            );

        if (!size_ec) {
            size_groups[size].push_back(
                file
            );
        }
    }

    /*
     * Compare files having the same size.
     *
     * The actual bytes are compared, so we never
     * report files as duplicates merely because of
     * a hash collision.
     */
    bool found_duplicates = false;

    for (
        const auto& size_group :
        size_groups
    ) {
        const std::vector<fs::path>& candidates =
            size_group.second;

        if (candidates.size() < 2) {
            continue;
        }

        std::vector<bool> already_reported(
            candidates.size(),
            false
        );

        for (
            std::size_t i = 0;
            i < candidates.size();
            ++i
        ) {
            if (already_reported[i]) {
                continue;
            }

            std::vector<std::size_t> duplicates;

            duplicates.push_back(i);

            for (
                std::size_t j = i + 1;
                j < candidates.size();
                ++j
            ) {
                if (already_reported[j]) {
                    continue;
                }

                if (
                    files_equal(
                        candidates[i],
                        candidates[j]
                    )
                ) {
                    duplicates.push_back(j);
                }
            }

            if (duplicates.size() < 2) {
                continue;
            }

            found_duplicates = true;

            std::cout
                << "Duplicate group"
                << " (size: "
                << size_group.first
                << " bytes)\n";

            for (
                std::size_t index :
                duplicates
            ) {
                std::cout
                    << "  "
                    << candidates[index]
                    << "\n";

                already_reported[index] = true;
            }

            std::cout << "\n";
        }
    }

    if (!found_duplicates) {
        std::cout
            << "No duplicate files found.\n";
    }
}