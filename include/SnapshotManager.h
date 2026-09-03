#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct SnapshotEntry {
    std::string path;
    bool is_directory;
    std::uintmax_t size;
};

class SnapshotManager {
private:
    std::vector<SnapshotEntry> snapshot_entries;

public:
    void create(
        const fs::path& root
    );

    void show() const;

    void diff(
        const fs::path& root
    ) const;

    void clear();

    bool empty() const;

private:
    void collect(
        const fs::path& root
    );
};