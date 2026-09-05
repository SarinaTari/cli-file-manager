#include "ProjectDetector.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct ProjectType {
    std::string name;
    std::vector<std::string> indicators;
};

const std::vector<ProjectType> project_types = {
    {
        "C++ / CMake",
        {
            "CMakeLists.txt"
        }
    },
    {
        "C++ / Make",
        {
            "Makefile"
        }
    },
    {
        "Python",
        {
            "pyproject.toml",
            "requirements.txt",
            "setup.py",
            "Pipfile"
        }
    },
    {
        "Rust",
        {
            "Cargo.toml"
        }
    },
    {
        "Java / Maven",
        {
            "pom.xml"
        }
    },
    {
        "Java / Gradle",
        {
            "build.gradle",
            "build.gradle.kts"
        }
    },
    {
        "Node.js",
        {
            "package.json"
        }
    }
};

bool contains_indicator(
    const fs::path& root,
    const std::vector<std::string>& indicators
) {
    for (const std::string& indicator : indicators) {
        if (fs::exists(root / indicator)) {
            return true;
        }
    }

    return false;
}

std::size_t count_files_with_extensions(
    const fs::path& root,
    const std::vector<std::string>& extensions
) {
    std::size_t count = 0;

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

        if (entry.is_regular_file(ec)) {
            std::string extension =
                entry.path().extension().string();

            if (
                std::find(
                    extensions.begin(),
                    extensions.end(),
                    extension
                ) != extensions.end()
            ) {
                ++count;
            }
        }

        iterator.increment(ec);
    }

    return count;
}

}

void ProjectDetector::analyze(
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
            "Project analysis requires a directory: "
            + root.string()
        );
    }

    std::cout
        << "Project analysis for: "
        << root
        << "\n\n";

    std::vector<std::string> detected_types;

    for (const ProjectType& type : project_types) {
        if (
            contains_indicator(
                root,
                type.indicators
            )
        ) {
            detected_types.push_back(type.name);
        }
    }

    if (detected_types.empty()) {
        std::cout
            << "Project type: Unknown\n";
    }
    else {
        std::cout
            << "Project type(s):\n";

        for (const std::string& type : detected_types) {
            std::cout
                << "  - "
                << type
                << "\n";
        }
    }

    std::size_t cpp_files =
        count_files_with_extensions(
            root,
            {
                ".cpp",
                ".cc",
                ".cxx"
            }
        );

    std::size_t header_files =
        count_files_with_extensions(
            root,
            {
                ".h",
                ".hh",
                ".hpp",
                ".hxx"
            }
        );

    std::size_t python_files =
        count_files_with_extensions(
            root,
            {
                ".py"
            }
        );

    std::size_t rust_files =
        count_files_with_extensions(
            root,
            {
                ".rs"
            }
        );

    std::size_t java_files =
        count_files_with_extensions(
            root,
            {
                ".java"
            }
        );

    std::cout
        << "\nSource statistics:\n";

    std::cout
        << "  C/C++ source files: "
        << cpp_files
        << "\n";

    std::cout
        << "  C/C++ header files: "
        << header_files
        << "\n";

    std::cout
        << "  Python files: "
        << python_files
        << "\n";

    std::cout
        << "  Rust files: "
        << rust_files
        << "\n";

    std::cout
        << "  Java files: "
        << java_files
        << "\n";

    std::cout
        << "\nDetected project indicators:\n";

    bool found_indicator = false;

    for (const ProjectType& type : project_types) {
        for (const std::string& indicator : type.indicators) {
            if (fs::exists(root / indicator)) {
                std::cout
                    << "  "
                    << indicator
                    << "\n";

                found_indicator = true;
            }
        }
    }

    if (!found_indicator) {
        std::cout
            << "  None\n";
    }
}