#pragma once

#include "FileHasher.h"

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class DuplicateDetector {
public:
    static void find_duplicates(
        const fs::path& root
    );

private:
    static bool files_equal(
        const fs::path& first,
        const fs::path& second
    );

    static void collect_files(
        const fs::path& root,
        std::vector<fs::path>& files
    );
};