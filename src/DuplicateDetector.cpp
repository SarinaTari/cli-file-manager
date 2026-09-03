#include "DuplicateDetector.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <system_error>

void DuplicateDetector::collect_files(
    const fs::path& root,
    std::vector<fs::path>& files
) {
    std::error_code ec;

    /*
     * Root doesn't exist.
     */
    if (!fs::exists(root, ec)) {
        return;
    }

    /*
     * If the user gave us a single file,
     * add it directly.
     */
    if (fs::is_regular_file(root, ec)) {
        files.push_back(root);
        return;
    }

    /*
     * We only recursively scan directories.
     */
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

        std::error_code entry_error;

        if (
            iterator->is_regular_file(
                entry_error
            )
        ) {
            files.push_back(
                iterator->path()
            );
        }

        iterator.increment(ec);
    }
}


bool DuplicateDetector::files_equal(
    const fs::path& first,
    const fs::path& second
) {
    std::error_code ec;

    /*
     * First compare file sizes.
     *
     * Different sizes means the files
     * cannot possibly be identical.
     */
    std::uintmax_t first_size =
        fs::file_size(
            first,
            ec
        );

    if (ec) {
        return false;
    }

    std::uintmax_t second_size =
        fs::file_size(
            second,
            ec
        );

    if (ec) {
        return false;
    }

    if (first_size != second_size) {
        return false;
    }

    /*
     * Open both files in binary mode.
     */
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

    /*
     * Compare the contents in chunks.
     */
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

        /*
         * Different number of bytes read.
         */
        if (first_count != second_count) {
            return false;
        }

        /*
         * Both files reached EOF.
         */
        if (first_count == 0) {
            break;
        }

        /*
         * Compare the bytes.
         */
        if (
            !std::equal(
                first_buffer,
                first_buffer + first_count,
                second_buffer
            )
        ) {
            return false;
        }
    }

    return true;
}


void DuplicateDetector::find_duplicates(
    const fs::path& root
) {
    std::vector<fs::path> files;

    /*
     * Step 1:
     * Collect every regular file.
     */
    collect_files(
        root,
        files
    );

    /*
     * Step 2:
     * Group files by size.
     *
     * Files with different sizes cannot
     * be duplicates.
     */
    std::map<
        std::uintmax_t,
        std::vector<fs::path>
    > files_by_size;

    for (const auto& file : files) {
        std::error_code ec;

        std::uintmax_t size =
            fs::file_size(
                file,
                ec
            );

        if (!ec) {
            files_by_size[size].push_back(
                file
            );
        }
    }

    bool found_duplicates = false;

    /*
     * Step 3:
     * For every size group containing at
     * least two files, calculate hashes.
     */
    for (
        const auto& size_group
        : files_by_size
    ) {
        const auto& candidates =
            size_group.second;

        if (candidates.size() < 2) {
            continue;
        }

        std::map<
            std::string,
            std::vector<fs::path>
        > files_by_hash;

        for (const auto& file : candidates) {
            try {
                std::string hash =
                    FileHasher::hash_file(
                        file
                    );

                files_by_hash[hash].push_back(
                    file
                );
            }
            catch (
                const std::exception&
            ) {
                /*
                 * Ignore files that cannot
                 * be opened or hashed.
                 */
            }
        }

        /*
         * Step 4:
         * Files with the same size and hash
         * are potential duplicates.
         */
        for (
            const auto& hash_group
            : files_by_hash
        ) {
            const std::string& hash =
                hash_group.first;

            const auto& candidates_with_hash =
                hash_group.second;

            if (
                candidates_with_hash.size()
                < 2
            ) {
                continue;
            }

            /*
             * Hashes are a fast way to find
             * candidates, but we perform an
             * actual byte-for-byte comparison
             * before declaring duplicates.
             */
            const fs::path& first =
                candidates_with_hash[0];

            std::vector<fs::path>
                duplicate_group;

            for (
                const auto& file
                : candidates_with_hash
            ) {
                if (
                    files_equal(
                        first,
                        file
                    )
                ) {
                    duplicate_group.push_back(
                        file
                    );
                }
            }

            if (
                duplicate_group.size() < 2
            ) {
                continue;
            }

            found_duplicates = true;

            std::cout
                << "\nDuplicate group "
                << "(size: "
                << size_group.first
                << " bytes)\n";

            std::cout
                << "Hash: "
                << hash
                << "\n";

            for (
                const auto& file
                : duplicate_group
            ) {
                std::cout
                    << "  "
                    << file
                    << "\n";
            }
        }
    }

    /*
     * Nothing duplicated.
     */
    if (!found_duplicates) {
        std::cout
            << "No duplicate files found.\n";
    }
}