#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class StorageAnalyzer {
public:
    static void analyze(
        const fs::path& root
    );
};