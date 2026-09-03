#include "FileHasher.h"

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

std::string FileHasher::hash_file(
    const fs::path& path
) {
    std::ifstream file(
        path,
        std::ios::binary
    );

    if (!file) {
        throw std::runtime_error(
            "Unable to open file for hashing: "
            + path.string()
        );
    }

    constexpr std::uint64_t offset_basis =
        14695981039346656037ULL;

    constexpr std::uint64_t prime =
        1099511628211ULL;

    std::uint64_t hash = offset_basis;

    char buffer[8192];

    while (file.read(buffer, sizeof(buffer)) ||
           file.gcount() > 0) {

        std::streamsize bytes_read =
            file.gcount();

        for (
            std::streamsize i = 0;
            i < bytes_read;
            ++i
        ) {
            hash ^= static_cast<unsigned char>(
                buffer[i]
            );

            hash *= prime;
        }
    }

    std::ostringstream result;

    result
        << std::hex
        << std::setw(16)
        << std::setfill('0')
        << hash;

    return result.str();
}