#include "GitAnalyzer.h"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

std::string trim(
    const std::string& value
) {
    const std::string whitespace =
        " \t\n\r";

    std::size_t start =
        value.find_first_not_of(whitespace);

    if (start == std::string::npos) {
        return "";
    }

    std::size_t end =
        value.find_last_not_of(whitespace);

    return value.substr(
        start,
        end - start + 1
    );
}

std::string run_command(
    const std::string& command
) {
    std::array<char, 256> buffer{};

    std::string result;

    FILE* pipe =
        popen(
            command.c_str(),
            "r"
        );

    if (pipe == nullptr) {
        throw std::runtime_error(
            "Unable to execute Git command."
        );
    }

    while (
        fgets(
            buffer.data(),
            static_cast<int>(buffer.size()),
            pipe
        ) != nullptr
    ) {
        result += buffer.data();
    }

    int status =
        pclose(pipe);

    if (status != 0) {
        return "";
    }

    return trim(result);
}

bool is_git_repository(
    const fs::path& path
) {
    std::string command =
        "git -C \"" +
        path.string() +
        "\" rev-parse --is-inside-work-tree 2>/dev/null";

    return run_command(command) == "true";
}

}

void GitAnalyzer::analyze(
    const fs::path& path
) {
    std::error_code ec;

    if (!fs::exists(path, ec)) {
        throw std::runtime_error(
            "Path does not exist: "
            + path.string()
        );
    }

    fs::path working_path = path;

    if (
        fs::is_regular_file(
            working_path,
            ec
        )
    ) {
        working_path =
            working_path.parent_path();
    }

    if (!is_git_repository(working_path)) {
        std::cout
            << "Git repository: No\n";

        return;
    }

    std::cout
        << "Git repository: Yes\n";

    std::string root =
        run_command(
            "git -C \"" +
            working_path.string() +
            "\" rev-parse --show-toplevel 2>/dev/null"
        );

    std::string branch =
        run_command(
            "git -C \"" +
            working_path.string() +
            "\" branch --show-current 2>/dev/null"
        );

    std::string status =
        run_command(
            "git -C \"" +
            working_path.string() +
            "\" status --short 2>/dev/null"
        );

    std::string commit =
        run_command(
            "git -C \"" +
            working_path.string() +
            "\" log -1 --pretty=format:%h\\ %s 2>/dev/null"
        );

    std::cout
        << "Repository root: "
        << (root.empty() ? "Unknown" : root)
        << "\n";

    std::cout
        << "Branch: "
        << (branch.empty() ? "(detached HEAD or no commits)" : branch)
        << "\n";

    std::cout
        << "Latest commit: "
        << (commit.empty() ? "None" : commit)
        << "\n";

    std::size_t modified = 0;
    std::size_t staged = 0;
    std::size_t untracked = 0;

    std::size_t position = 0;

    while (position < status.size()) {
        std::size_t newline =
            status.find(
                '\n',
                position
            );

        if (newline == std::string::npos) {
            newline = status.size();
        }

        std::string line =
            status.substr(
                position,
                newline - position
            );

        if (line.size() >= 2) {
            char index_status = line[0];
            char worktree_status = line[1];

            if (index_status == '?') {
                ++untracked;
            }
            else {
                if (
                    index_status != ' '
                    && index_status != '?'
                ) {
                    ++staged;
                }

                if (
                    worktree_status != ' '
                    && worktree_status != '?'
                ) {
                    ++modified;
                }
            }
        }

        position = newline + 1;
    }

    std::cout
        << "\nWorking tree:\n";

    std::cout
        << "  Staged changes: "
        << staged
        << "\n";

    std::cout
        << "  Modified files: "
        << modified
        << "\n";

    std::cout
        << "  Untracked files: "
        << untracked
        << "\n";

    if (status.empty()) {
        std::cout
            << "\nWorking tree is clean.\n";
    }
    else {
        std::cout
            << "\nGit status:\n"
            << status
            << "\n";
    }
}