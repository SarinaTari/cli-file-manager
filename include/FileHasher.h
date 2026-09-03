#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

class FileHasher {
public:
    static std::string hash_file(
        const fs::path& path
    );
};