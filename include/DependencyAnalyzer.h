#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class DependencyAnalyzer {
public:
    static void analyze(const fs::path& root);
};