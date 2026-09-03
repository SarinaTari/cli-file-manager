#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class GitAnalyzer {
public:
    static void analyze(const fs::path& path);
};