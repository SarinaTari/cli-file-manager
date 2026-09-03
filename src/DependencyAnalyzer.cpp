#include "DependencyAnalyzer.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct SourceFile {
    fs::path path;
    std::vector<std::string> includes;
};

bool is_cpp_file(
    const fs::path& path
) {
    const std::string extension =
        path.extension().string();

    return
        extension == ".cpp"
        || extension == ".cc"
        || extension == ".cxx"
        || extension == ".h"
        || extension == ".hh"
        || extension == ".hpp"
        || extension == ".hxx";
}

std::vector<fs::path> collect_cpp_files(
    const fs::path& root
) {
    std::vector<fs::path> files;

    std::error_code ec;

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

        const auto& entry = *iterator;

        if (
            entry.is_regular_file(ec)
            && is_cpp_file(entry.path())
        ) {
            files.push_back(entry.path());
        }

        iterator.increment(ec);
    }

    std::sort(
        files.begin(),
        files.end()
    );

    return files;
}

std::vector<std::string> extract_includes(
    const fs::path& path
) {
    std::vector<std::string> includes;

    std::ifstream file(path);

    if (!file) {
        return includes;
    }

    std::string line;

    std::regex include_pattern(
        R"(^\s*#\s*include\s*[<"]([^">]+)[">])"
    );

    while (std::getline(file, line)) {
        std::smatch match;

        if (
            std::regex_search(
                line,
                match,
                include_pattern
            )
        ) {
            if (match.size() >= 2) {
                includes.push_back(
                    match[1].str()
                );
            }
        }
    }

    return includes;
}

}

void DependencyAnalyzer::analyze(
    const fs::path& root
) {
    std::error_code ec;

    if (!fs::exists(root, ec)) {
        throw std::runtime_error(
            "Path does not exist: "
            + root.string()
        );
    }

    if (!fs::is_directory(root, ec)) {
        throw std::runtime_error(
            "Dependency analysis requires a directory: "
            + root.string()
        );
    }

    std::vector<fs::path> files =
        collect_cpp_files(root);

    std::cout
        << "C/C++ dependency analysis for: "
        << root
        << "\n\n";

    if (files.empty()) {
        std::cout
            << "No C/C++ source or header files found.\n";

        return;
    }

    std::size_t total_include_count = 0;

    std::map<std::string, std::size_t>
        include_frequency;

    std::vector<SourceFile> source_files;

    for (const fs::path& file : files) {
        std::vector<std::string> includes =
            extract_includes(file);

        total_include_count +=
            includes.size();

        for (
            const std::string& include :
            includes
        ) {
            ++include_frequency[include];
        }

        source_files.push_back({
            file,
            includes
        });
    }

    std::cout
        << "C/C++ files: "
        << files.size()
        << "\n";

    std::cout
        << "Total #include directives: "
        << total_include_count
        << "\n";

    std::cout
        << "\n#include relationships:\n";

    for (
        const SourceFile& source :
        source_files
    ) {
        std::cout
            << "\n"
            << source.path
            << "\n";

        if (source.includes.empty()) {
            std::cout
                << "  (no includes)\n";

            continue;
        }

        for (
            const std::string& include :
            source.includes
        ) {
            std::cout
                << "  -> "
                << include
                << "\n";
        }
    }

    std::vector<
        std::pair<std::string, std::size_t>
    > frequencies(
        include_frequency.begin(),
        include_frequency.end()
    );

    std::sort(
        frequencies.begin(),
        frequencies.end(),
        [](const auto& first, const auto& second) {
            if (first.second != second.second) {
                return first.second > second.second;
            }

            return first.first < second.first;
        }
    );

    std::cout
        << "\nMost frequently included headers:\n";

    std::size_t limit =
        std::min<std::size_t>(
            10,
            frequencies.size()
        );

    for (
        std::size_t i = 0;
        i < limit;
        ++i
    ) {
        std::cout
            << "  "
            << frequencies[i].first
            << " ("
            << frequencies[i].second
            << ")\n";
    }
}