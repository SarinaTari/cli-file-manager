#include "SnapshotManager.h"

#include <algorithm>
#include <iostream>
#include <system_error>

void SnapshotManager::create(
    const fs::path& root
) {
    snapshot_entries.clear();

    collect(root);

    std::cout
        << "Snapshot created.\n";

    std::cout
        << "Entries: "
        << snapshot_entries.size()
        << "\n";
}

void SnapshotManager::collect(
    const fs::path& root
) {
    std::error_code ec;

    if (!fs::exists(root, ec)) {
        return;
    }

    if (fs::is_regular_file(root, ec)) {
        snapshot_entries.push_back({
            root.string(),
            false,
            fs::file_size(root, ec)
        });

        return;
    }

    if (!fs::is_directory(root, ec)) {
        return;
    }

    snapshot_entries.push_back({
        root.string(),
        true,
        0
    });

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

        if (entry.is_directory(entry_ec)) {
            snapshot_entries.push_back({
                entry.path().string(),
                true,
                0
            });
        }

        else if (entry.is_regular_file(entry_ec)) {
            std::uintmax_t size =
                entry.file_size(entry_ec);

            if (entry_ec) {
                size = 0;
            }

            snapshot_entries.push_back({
                entry.path().string(),
                false,
                size
            });
        }

        iterator.increment(ec);
    }

    std::sort(
        snapshot_entries.begin(),
        snapshot_entries.end(),
        [](const SnapshotEntry& a,
           const SnapshotEntry& b) {
            return a.path < b.path;
        }
    );
}

void SnapshotManager::show() const {
    if (snapshot_entries.empty()) {
        std::cout
            << "No snapshot exists.\n";

        return;
    }

    std::cout
        << "Snapshot:\n\n";

    for (
        const SnapshotEntry& entry :
        snapshot_entries
    ) {
        std::cout
            << "  "
            << (
                entry.is_directory
                ? "[DIR] "
                : "[FILE] "
            )
            << entry.path;

        if (!entry.is_directory) {
            std::cout
                << " ("
                << entry.size
                << " bytes)";
        }

        std::cout << "\n";
    }
}

void SnapshotManager::diff(
    const fs::path& root
) const {
    if (snapshot_entries.empty()) {
        std::cout
            << "No snapshot exists.\n";

        return;
    }

    SnapshotManager current;

    current.create(root);

    std::cout
        << "\nSnapshot differences:\n";

    bool differences = false;

    for (
        const SnapshotEntry& old_entry :
        snapshot_entries
    ) {
        auto iterator =
            std::find_if(
                current.snapshot_entries.begin(),
                current.snapshot_entries.end(),
                [&](const SnapshotEntry& current_entry) {
                    return current_entry.path
                        == old_entry.path;
                }
            );

        if (
            iterator
            == current.snapshot_entries.end()
        ) {
            std::cout
                << "  Removed: "
                << old_entry.path
                << "\n";

            differences = true;

            continue;
        }

        if (
            !old_entry.is_directory
            && old_entry.size
                != iterator->size
        ) {
            std::cout
                << "  Modified: "
                << old_entry.path
                << "\n";

            differences = true;
        }
    }

    for (
        const SnapshotEntry& current_entry :
        current.snapshot_entries
    ) {
        auto iterator =
            std::find_if(
                snapshot_entries.begin(),
                snapshot_entries.end(),
                [&](const SnapshotEntry& old_entry) {
                    return old_entry.path
                        == current_entry.path;
                }
            );

        if (
            iterator
            == snapshot_entries.end()
        ) {
            std::cout
                << "  Added: "
                << current_entry.path
                << "\n";

            differences = true;
        }
    }

    if (!differences) {
        std::cout
            << "  No differences.\n";
    }
}

void SnapshotManager::clear() {
    snapshot_entries.clear();
}

bool SnapshotManager::empty() const {
    return snapshot_entries.empty();
}