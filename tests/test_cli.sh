#!/usr/bin/env bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

set -u

PROGRAM="$1"

if [ ! -x "$PROGRAM" ]; then
    echo "ERROR: Program does not exist or is not executable:"
    echo "$PROGRAM"
    exit 1
fi

# Convert PROGRAM to an absolute path.
PROGRAM="$(cd "$(dirname "$PROGRAM")" && pwd)/$(basename "$PROGRAM")"

# Create temporary test directory.
TEST_DIR="$(mktemp -d "${TMPDIR:-/tmp}/cli-file-manager-test.XXXXXX")"

# macOS can represent /var as /private/var.
# pwd -P gives us the physical path.
TEST_DIR="$(cd "$TEST_DIR" && pwd -P)"

cleanup() {
    rm -rf "$TEST_DIR"
}

trap cleanup EXIT

PASS_COUNT=0
FAIL_COUNT=0

pass() {
    echo "PASS: $1"
    PASS_COUNT=$((PASS_COUNT + 1))
}

fail() {
    echo "FAIL: $1"
    FAIL_COUNT=$((FAIL_COUNT + 1))
}

assert_file_exists() {
    local path="$1"
    local description="$2"

    if [ -e "$path" ]; then
        pass "$description"
    else
        fail "$description"
    fi
}

assert_file_not_exists() {
    local path="$1"
    local description="$2"

    if [ ! -e "$path" ]; then
        pass "$description"
    else
        fail "$description"
    fi
}

assert_contains() {
    local output="$1"
    local expected="$2"
    local description="$3"

    if printf '%s\n' "$output" | grep -Fq "$expected"; then
        pass "$description"
    else
        fail "$description"

        echo "  Expected output to contain:"
        echo "  $expected"

        echo "  Actual output:"
        echo "$output"
    fi
}

run_program() {
    local commands="$1"

    (
        cd "$TEST_DIR" || exit 1
        printf '%b' "$commands" | "$PROGRAM"
    )
}


# ============================================================
# PHASE 1 — NAVIGATION
# ============================================================

OUTPUT="$(run_program 'pwd\nq\n')"

assert_contains \
    "$OUTPUT" \
    "$TEST_DIR" \
    "pwd shows the current directory"


mkdir "$TEST_DIR/navigation"

OUTPUT="$(run_program 'cd navigation\npwd\nq\n')"

assert_contains \
    "$OUTPUT" \
    "$TEST_DIR/navigation" \
    "cd changes the current directory"


# ============================================================
# PHASE 2 — FILE OPERATIONS
# ============================================================

OUTPUT="$(run_program 'touch file.txt\nq\n')"

assert_file_exists \
    "$TEST_DIR/file.txt" \
    "touch creates a file"


OUTPUT="$(run_program 'mkdir folder\nq\n')"

assert_file_exists \
    "$TEST_DIR/folder" \
    "mkdir creates a directory"


OUTPUT="$(run_program 'mkdir "folder with spaces"\nq\n')"

assert_file_exists \
    "$TEST_DIR/folder with spaces" \
    "quoted paths work correctly"


OUTPUT="$(run_program 'touch original.txt\nrename original.txt renamed.txt\nq\n')"

assert_file_not_exists \
    "$TEST_DIR/original.txt" \
    "rename removes the old name"

assert_file_exists \
    "$TEST_DIR/renamed.txt" \
    "rename creates the new name"


OUTPUT="$(run_program 'touch copy-source.txt\ncp copy-source.txt copy.txt\nq\n')"

assert_file_exists \
    "$TEST_DIR/copy.txt" \
    "copy creates a copied file"


OUTPUT="$(run_program 'touch move-source.txt\nmv move-source.txt moved.txt\nq\n')"

assert_file_not_exists \
    "$TEST_DIR/move-source.txt" \
    "move removes the original file"

assert_file_exists \
    "$TEST_DIR/moved.txt" \
    "move creates the destination file"


# ============================================================
# PHASE 3 — FILE INFORMATION
# ============================================================

OUTPUT="$(run_program 'touch information.txt\nsize information.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Size:" \
    "size displays file size"


OUTPUT="$(run_program 'touch information.txt\ntype information.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Type:" \
    "type displays file type"


OUTPUT="$(run_program 'touch information.txt\nmodified information.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Modified:" \
    "modified displays modification time"


OUTPUT="$(run_program 'touch detailed.txt\ninfo detailed.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Path:" \
    "info displays detailed information"


# ============================================================
# PHASE 4 — DIRECTORY TREE / SIZE
# ============================================================

mkdir -p "$TEST_DIR/tree-test/subdirectory"

touch "$TEST_DIR/tree-test/file1.txt"
touch "$TEST_DIR/tree-test/subdirectory/file2.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'tree tree-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "file1.txt" \
    "tree displays files"

assert_contains \
    "$OUTPUT" \
    "subdirectory" \
    "tree displays directories"


OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'du tree-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "bytes" \
    "du displays directory size"


# ============================================================
# PHASE 5 — RECURSIVE SEARCH
# ============================================================

mkdir -p "$TEST_DIR/search-test/sub"

touch "$TEST_DIR/search-test/hello.txt"
touch "$TEST_DIR/search-test/sub/hello.txt"
touch "$TEST_DIR/search-test/sub/test.cpp"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'find hello.txt search-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "hello.txt" \
    "find locates files recursively"


OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'findext .cpp search-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "test.cpp" \
    "findext locates files by extension"


# ============================================================
# PHASE 6 — FIND BY SIZE
# ============================================================

printf 'this is a relatively large test file\n' \
    > "$TEST_DIR/large.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'findsize 1 .\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "large.txt" \
    "findsize locates files above minimum size"


# ============================================================
# PHASE 8 — PERMISSIONS
# ============================================================

touch "$TEST_DIR/permission.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'perm permission.txt\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Permissions:" \
    "perm displays permissions"


OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'chmod 644 permission.txt\nperm permission.txt\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "644" \
    "chmod changes permissions"


# ============================================================
# PHASE 8 — SYMBOLIC LINKS
# ============================================================

touch "$TEST_DIR/target.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'symlink target.txt symbolic.txt\nq\n' | "$PROGRAM"
)"

assert_file_exists \
    "$TEST_DIR/symbolic.txt" \
    "symbolic link is created"


OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'linktarget symbolic.txt\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "target.txt" \
    "linktarget displays symbolic link target"


# ============================================================
# PHASE 8 — HARD LINKS
# ============================================================

touch "$TEST_DIR/hard-target.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'ln hard-target.txt hard-link.txt\nq\n' | "$PROGRAM"
)"

assert_file_exists \
    "$TEST_DIR/hard-link.txt" \
    "hard link is created"


# ============================================================
# ERROR HANDLING
# ============================================================

OUTPUT="$(run_program 'cd\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Usage: cd <directory>" \
    "invalid argument count is handled"


OUTPUT="$(run_program 'size does-not-exist.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Path does not exist" \
    "nonexistent file is handled"


# ============================================================
# OVERWRITE PROTECTION
# ============================================================

touch "$TEST_DIR/source.txt"
touch "$TEST_DIR/destination.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'cp source.txt destination.txt\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "already exists" \
    "copy refuses to overwrite existing destination"


# ============================================================
# INVALID CHMOD
# ============================================================

OUTPUT="$(run_program 'chmod 999 file.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Permission mode must contain only octal digits" \
    "invalid chmod mode is rejected"


# ============================================================
# INVALID FINDSIZE
# ============================================================

OUTPUT="$(run_program 'findsize abc .\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Minimum size must be a non-negative integer" \
    "invalid findsize argument is rejected"


# ============================================================
# UNMATCHED QUOTES
# ============================================================

OUTPUT="$(run_program 'touch "unmatched\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Unmatched quote" \
    "unmatched quotes are rejected"


# ============================================================
# DANGEROUS PATH PROTECTION
# ============================================================

OUTPUT="$(run_program 'rm .\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Refusing to remove the current directory." \
    "rm refuses current-directory path"


OUTPUT="$(run_program 'rm ..\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Refusing to remove the parent directory." \
    "rm refuses parent-directory path"


# ============================================================
# RECURSIVE DELETE — CANCELLATION
# ============================================================

mkdir "$TEST_DIR/delete-no"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'rm delete-no\nn\nq\n' | "$PROGRAM"
)"

assert_file_exists \
    "$TEST_DIR/delete-no" \
    "recursive deletion can be cancelled"


assert_contains \
    "$OUTPUT" \
    "Deletion cancelled" \
    "recursive deletion cancellation is reported"


# ============================================================
# RECURSIVE DELETE — CONFIRMATION
# ============================================================

mkdir "$TEST_DIR/delete-yes"

touch "$TEST_DIR/delete-yes/file.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'rm delete-yes\ny\nq\n' | "$PROGRAM"
)"

assert_file_not_exists \
    "$TEST_DIR/delete-yes" \
    "recursive deletion works after confirmation"


assert_contains \
    "$OUTPUT" \
    "Removed:" \
    "successful recursive deletion is reported"


# ============================================================
# COPY DIRECTORY INTO ITSELF
# ============================================================

mkdir -p "$TEST_DIR/copy-source/sub"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'cp copy-source copy-source/sub\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "inside" \
    "copy refuses copying directory into itself"


# ============================================================
# MOVE DIRECTORY INTO ITSELF
# ============================================================

mkdir -p "$TEST_DIR/move-source/sub"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'mv move-source move-source/sub\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "inside" \
    "move refuses moving directory into itself"


# ============================================================
# PHASE 12 — STORAGE ANALYZER
# ============================================================

mkdir -p "$TEST_DIR/storage-test"

printf 'hello' \
    > "$TEST_DIR/storage-test/a.txt"

printf 'world' \
    > "$TEST_DIR/storage-test/b.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'analyze storage-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Storage analysis" \
    "storage analyzer works"

assert_contains \
    "$OUTPUT" \
    "Directories:" \
    "storage analyzer reports directory count"

assert_contains \
    "$OUTPUT" \
    "Files:" \
    "storage analyzer reports file count"

assert_contains \
    "$OUTPUT" \
    "Total size:" \
    "storage analyzer reports total size"

assert_contains \
    "$OUTPUT" \
    "Largest files:" \
    "storage analyzer reports largest files"


# ============================================================
# PHASE 12 — DUPLICATE DETECTOR
# ============================================================

mkdir -p "$TEST_DIR/duplicate-test"

printf 'same content' \
    > "$TEST_DIR/duplicate-test/a.txt"

printf 'same content' \
    > "$TEST_DIR/duplicate-test/b.txt"

printf 'different content' \
    > "$TEST_DIR/duplicate-test/c.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'duplicates duplicate-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "a.txt" \
    "duplicate detector finds first duplicate"

assert_contains \
    "$OUTPUT" \
    "b.txt" \
    "duplicate detector finds second duplicate"


# ============================================================
# PHASE 12 — DUPLICATE DETECTOR SHOULD NOT GROUP
# DIFFERENT FILES
# ============================================================

mkdir -p "$TEST_DIR/no-duplicate-test"

printf 'content one' \
    > "$TEST_DIR/no-duplicate-test/a.txt"

printf 'content two' \
    > "$TEST_DIR/no-duplicate-test/b.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'duplicates no-duplicate-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "No duplicate files found." \
    "duplicate detector reports no duplicates when appropriate"


# ============================================================
# PHASE 12 — DUPLICATE DETECTOR WITH EMPTY DIRECTORY
# ============================================================

mkdir -p "$TEST_DIR/empty-duplicate-test"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'duplicates empty-duplicate-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "No duplicate files found." \
    "duplicate detector handles empty directories"


# ============================================================
# PHASE 12 — ANALYZE EMPTY DIRECTORY
# ============================================================

mkdir -p "$TEST_DIR/empty-storage-test"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'analyze empty-storage-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Files: 0" \
    "storage analyzer handles empty directories"


# ============================================================
# PHASE 12 — INVALID ANALYZE PATH
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'analyze does-not-exist\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Path does not exist" \
    "analyze handles nonexistent paths"


# ============================================================
# PHASE 12 — INVALID DUPLICATES PATH
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'duplicates does-not-exist\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "No duplicate files found" \
    "duplicates handles nonexistent paths"


# ============================================================
# PHASE 13 — DEVELOPER INTELLIGENCE
# ============================================================

PROJECT_DIR="$TEST_DIR/project-test"

mkdir -p "$PROJECT_DIR/src"
mkdir -p "$PROJECT_DIR/include"

cat > "$PROJECT_DIR/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.16)
project(test_project)
EOF

cat > "$PROJECT_DIR/src/main.cpp" <<'EOF'
#include <iostream>
#include "test.h"

int main() {
    return 0;
}
EOF

cat > "$PROJECT_DIR/include/test.h" <<'EOF'
#pragma once

void test_function();
EOF

OUTPUT="$(run_program "project \"$PROJECT_DIR\"
q
")"

assert_contains \
    "$OUTPUT" \
    "C++ / CMake" \
    "project detects CMake C++ project"

assert_contains \
    "$OUTPUT" \
    "C/C++ source files: 1" \
    "project counts C++ source files"

assert_contains \
    "$OUTPUT" \
    "C/C++ header files: 1" \
    "project counts C++ header files"

assert_contains \
    "$OUTPUT" \
    "PROJECT DASHBOARD" \
    "project displays dashboard"

assert_contains \
    "$OUTPUT" \
    "Total files:" \
    "project displays total file count"

assert_contains \
    "$OUTPUT" \
    "Storage:" \
    "project displays storage information"

assert_contains \
    "$OUTPUT" \
    "Git:" \
    "project displays Git information"

assert_contains \
    "$OUTPUT" \
    "Languages:" \
    "project displays language breakdown"


OUTPUT="$(run_program "deps \"$PROJECT_DIR\"
q
")"

assert_contains \
    "$OUTPUT" \
    "C/C++ files: 2" \
    "deps counts C++ files"

assert_contains \
    "$OUTPUT" \
    "#include relationships:" \
    "deps displays include relationships"

assert_contains \
    "$OUTPUT" \
    "iostream" \
    "deps detects standard library include"

assert_contains \
    "$OUTPUT" \
    "test.h" \
    "deps detects local header include"


git -C "$TEST_DIR" init -q

OUTPUT="$(run_program "git .
q
")"

assert_contains \
    "$OUTPUT" \
    "Git repository: Yes" \
    "git detects repository"

assert_contains \
    "$OUTPUT" \
    "Repository root:" \
    "git displays repository root"

assert_contains \
    "$OUTPUT" \
    "Branch:" \
    "git displays branch"

rm -rf "$PROJECT_DIR"


# ============================================================
# PHASE 14 — HISTORY
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'touch history-file.txt\nhistory\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "create_file" \
    "history records file creation"


OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'mkdir history-dir\nhistory\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "create_directory" \
    "history records directory creation"


OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'touch history-original.txt\nrename history-original.txt history-renamed.txt\nhistory\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "rename" \
    "history records rename"


OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'touch history-move-source.txt\nmv history-move-source.txt history-move-destination.txt\nhistory\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "move" \
    "history records move"


# ============================================================
# PHASE 14 — UNDO FILE CREATION
# ============================================================

rm -f "$TEST_DIR/undo-file.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'touch undo-file.txt\nundo\nq\n' | "$PROGRAM"
)"

assert_file_not_exists \
    "$TEST_DIR/undo-file.txt" \
    "undo removes a newly created file"

assert_contains \
    "$OUTPUT" \
    "Undone: file creation" \
    "undo reports successful file creation undo"


# ============================================================
# PHASE 14 — UNDO DIRECTORY CREATION
# ============================================================

rm -rf "$TEST_DIR/undo-directory"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'mkdir undo-directory\nundo\nq\n' | "$PROGRAM"
)"

assert_file_not_exists \
    "$TEST_DIR/undo-directory" \
    "undo removes a newly created empty directory"

assert_contains \
    "$OUTPUT" \
    "Undone: directory creation" \
    "undo reports successful directory creation undo"


# ============================================================
# PHASE 14 — UNDO RENAME
# ============================================================

rm -f "$TEST_DIR/undo-rename-old.txt"
rm -f "$TEST_DIR/undo-rename-new.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'touch undo-rename-old.txt\nrename undo-rename-old.txt undo-rename-new.txt\nundo\nq\n' | "$PROGRAM"
)"

assert_file_exists \
    "$TEST_DIR/undo-rename-old.txt" \
    "undo restores original name after rename"

assert_file_not_exists \
    "$TEST_DIR/undo-rename-new.txt" \
    "undo removes renamed path after rename"


# ============================================================
# PHASE 14 — UNDO MOVE
# ============================================================

rm -f "$TEST_DIR/undo-move-source.txt"
rm -f "$TEST_DIR/undo-move-destination.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'touch undo-move-source.txt\nmv undo-move-source.txt undo-move-destination.txt\nundo\nq\n' | "$PROGRAM"
)"

assert_file_exists \
    "$TEST_DIR/undo-move-source.txt" \
    "undo restores original path after move"

assert_file_not_exists \
    "$TEST_DIR/undo-move-destination.txt" \
    "undo removes moved path after move"


# ============================================================
# PHASE 14 — UNDO AFTER CHANGING DIRECTORY
# ============================================================

rm -f "$TEST_DIR/navigation-undo.txt"

rm -rf "$TEST_DIR/undo-navigation"
mkdir "$TEST_DIR/undo-navigation"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'touch navigation-undo.txt\ncd undo-navigation\nundo\nq\n' | "$PROGRAM"
)"

assert_file_not_exists \
    "$TEST_DIR/navigation-undo.txt" \
    "undo works after changing directory"


# ============================================================
# PHASE 14 — EMPTY HISTORY
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'undo\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Nothing to undo." \
    "undo reports when there is nothing to undo"


# ============================================================
# PHASE 14 — SNAPSHOT CREATION
# ============================================================

rm -rf "$TEST_DIR/snapshot-test"

mkdir -p "$TEST_DIR/snapshot-test/sub"

printf 'snapshot content' \
    > "$TEST_DIR/snapshot-test/file.txt"

OUTPUT="$(
    cd "$TEST_DIR/snapshot-test" || exit 1
    printf 'snapshot\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Snapshot created." \
    "snapshot creates a snapshot"

assert_contains \
    "$OUTPUT" \
    "Entries:" \
    "snapshot reports number of entries"


# ============================================================
# PHASE 14 — SHOW SNAPSHOT
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR/snapshot-test" || exit 1
    printf 'snapshot\nshowsnapshot\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Snapshot:" \
    "showsnapshot displays snapshot"

assert_contains \
    "$OUTPUT" \
    "file.txt" \
    "showsnapshot displays captured files"


# ============================================================
# PHASE 14 — SNAPSHOT DIFF: ADDED FILE
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR/snapshot-test" || exit 1
    printf 'snapshot\ntouch added.txt\ndiff\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Added:" \
    "snapshot diff detects added files"

assert_contains \
    "$OUTPUT" \
    "added.txt" \
    "snapshot diff identifies added file"


# ============================================================
# PHASE 14 — SNAPSHOT DIFF: REMOVED FILE
# ============================================================

rm -f "$TEST_DIR/snapshot-test/removed.txt"

printf 'temporary snapshot file' \
    > "$TEST_DIR/snapshot-test/removed.txt"

OUTPUT="$(
    cd "$TEST_DIR/snapshot-test" || exit 1
    printf 'snapshot\nrm removed.txt\ny\ndiff\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Removed:" \
    "snapshot diff detects removed files"

assert_contains \
    "$OUTPUT" \
    "removed.txt" \
    "snapshot diff identifies removed file"


# ============================================================
# PHASE 14 — NO SNAPSHOT
# ============================================================

rm -rf "$TEST_DIR/no-snapshot-test"

mkdir "$TEST_DIR/no-snapshot-test"

OUTPUT="$(
    cd "$TEST_DIR/no-snapshot-test" || exit 1
    printf 'showsnapshot\ndiff\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "No snapshot exists." \
    "snapshot commands handle missing snapshot"


# ============================================================
# PHASE 14 — CLEAR SNAPSHOT
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR/snapshot-test" || exit 1
    printf 'snapshot\nclearsnapshot\nshowsnapshot\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Snapshot cleared." \
    "clearsnapshot reports successful clearing"

assert_contains \
    "$OUTPUT" \
    "No snapshot exists." \
    "clearsnapshot removes stored snapshot"


# ============================================================
# UI COMMAND
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'ui\nq\n' | "$PROGRAM"
)"

# We don't assert detailed UI output here because
# terminal UI behavior depends on terminal capabilities.
# The important thing is that the command does not crash.


# ============================================================
# FINAL RESULT
# ============================================================

echo
echo "========================================"
echo "Test Summary"
echo "========================================"

echo "Passed: $PASS_COUNT"
echo "Failed: $FAIL_COUNT"

echo "========================================"

if [ "$FAIL_COUNT" -eq 0 ]; then
    echo "ALL TESTS PASSED"
    exit 0
else
    echo "SOME TESTS FAILED"
    exit 1
fi
