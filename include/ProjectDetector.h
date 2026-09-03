#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class ProjectDetector {
public:
    static void analyze(const fs::path& root);
};