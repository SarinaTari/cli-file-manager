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


# ============================================================
# PHASE 15.1 — PROJECT DASHBOARD
# ============================================================

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


# ============================================================
# PHASE 13 — DEPENDENCY ANALYZER
# ============================================================

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


# ============================================================
# PHASE 13 — GIT DETECTOR
# ============================================================

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
# PHASE 15.2 — SAFE DELETE
# ============================================================

SAFE_DELETE_TEST="$TEST_DIR/safe-delete-test"

mkdir -p "$SAFE_DELETE_TEST/subdirectory"

printf 'hello' \
    > "$SAFE_DELETE_TEST/file1.txt"

printf 'world' \
    > "$SAFE_DELETE_TEST/subdirectory/file2.txt"


# ============================================================
# SAFE DELETE — PREVIEW
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm safe-delete-test\nn\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "SAFE DELETE" \
    "safe rm displays safe delete preview"

assert_contains \
    "$OUTPUT" \
    "Files:" \
    "safe rm displays file count"

assert_contains \
    "$OUTPUT" \
    "Directories:" \
    "safe rm displays directory count"

assert_contains \
    "$OUTPUT" \
    "Total size:" \
    "safe rm displays total size"

assert_contains \
    "$OUTPUT" \
    "WARNING: This operation cannot be undone." \
    "safe rm displays deletion warning"

assert_contains \
    "$OUTPUT" \
    "Continue? [y/N]:" \
    "safe rm asks for confirmation"


# ============================================================
# SAFE DELETE — CANCELLATION
# ============================================================

assert_file_exists \
    "$SAFE_DELETE_TEST" \
    "safe rm cancellation preserves target"


assert_contains \
    "$OUTPUT" \
    "Deletion cancelled." \
    "safe rm reports cancellation"


# ============================================================
# SAFE DELETE — CONFIRMED FILE DELETION
# ============================================================

SAFE_DELETE_FILE="$TEST_DIR/safe-delete-file.txt"

printf 'safe delete file' \
    > "$SAFE_DELETE_FILE"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm safe-delete-file.txt\ny\nq\n' | "$PROGRAM"
)"

assert_file_not_exists \
    "$SAFE_DELETE_FILE" \
    "safe rm deletes confirmed file"


assert_contains \
    "$OUTPUT" \
    "Deleted successfully." \
    "safe rm reports successful file deletion"


# ============================================================
# SAFE DELETE — CONFIRMED DIRECTORY DELETION
# ============================================================

SAFE_DELETE_CONFIRM="$TEST_DIR/safe-delete-confirm"

mkdir -p "$SAFE_DELETE_CONFIRM/subdir"

printf 'file one' \
    > "$SAFE_DELETE_CONFIRM/file1.txt"

printf 'file two' \
    > "$SAFE_DELETE_CONFIRM/subdir/file2.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm safe-delete-confirm\ny\nq\n' | "$PROGRAM"
)"

assert_file_not_exists \
    "$SAFE_DELETE_CONFIRM" \
    "safe rm deletes confirmed directory recursively"


assert_contains \
    "$OUTPUT" \
    "Deleted successfully." \
    "safe rm reports successful directory deletion"


# ============================================================
# SAFE DELETE — EMPTY INPUT CANCELS
# ============================================================

SAFE_DELETE_EMPTY="$TEST_DIR/safe-delete-empty"

mkdir "$SAFE_DELETE_EMPTY"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm safe-delete-empty\n\nq\n' | "$PROGRAM"
)"

assert_file_exists \
    "$SAFE_DELETE_EMPTY" \
    "safe rm empty confirmation preserves target"


assert_contains \
    "$OUTPUT" \
    "Deletion cancelled." \
    "safe rm empty confirmation cancels deletion"


# ============================================================
# SAFE DELETE — OTHER INPUT CANCELS
# ============================================================

SAFE_DELETE_OTHER="$TEST_DIR/safe-delete-other"

mkdir "$SAFE_DELETE_OTHER"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm safe-delete-other\nhello\nq\n' | "$PROGRAM"
)"

assert_file_exists \
    "$SAFE_DELETE_OTHER" \
    "safe rm invalid confirmation preserves target"


assert_contains \
    "$OUTPUT" \
    "Deletion cancelled." \
    "safe rm invalid confirmation cancels deletion"


# ============================================================
# SAFE DELETE — DANGEROUS CURRENT DIRECTORY
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm .\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "dangerous path" \
    "safe rm rejects current-directory path"


# ============================================================
# SAFE DELETE — DANGEROUS PARENT DIRECTORY
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm ..\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "dangerous path" \
    "safe rm rejects parent-directory path"


# ============================================================
# SAFE DELETE — NONEXISTENT PATH
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm does-not-exist\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Path does not exist" \
    "safe rm rejects nonexistent path"


# ============================================================
# SAFE DELETE — SINGLE FILE STATISTICS
# ============================================================

SAFE_DELETE_STAT_FILE="$TEST_DIR/safe-delete-stat.txt"

printf 'statistics' \
    > "$SAFE_DELETE_STAT_FILE"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm safe-delete-stat.txt\nn\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Type: File" \
    "safe rm identifies a single file"

assert_contains \
    "$OUTPUT" \
    "Files: 1" \
    "safe rm counts a single file correctly"

assert_contains \
    "$OUTPUT" \
    "Directories: 0" \
    "safe rm reports zero directories for a file"


# ============================================================
# SAFE DELETE — DIRECTORY STATISTICS
# ============================================================

SAFE_DELETE_STATS_DIR="$TEST_DIR/safe-delete-stats-dir"

mkdir -p "$SAFE_DELETE_STATS_DIR/a/b"

printf 'one' \
    > "$SAFE_DELETE_STATS_DIR/file1.txt"

printf 'two' \
    > "$SAFE_DELETE_STATS_DIR/a/file2.txt"

printf 'three' \
    > "$SAFE_DELETE_STATS_DIR/a/b/file3.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm safe-delete-stats-dir\nn\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Type: Directory" \
    "safe rm identifies a directory"

assert_contains \
    "$OUTPUT" \
    "Files: 3" \
    "safe rm counts recursive files correctly"

assert_contains \
    "$OUTPUT" \
    "Directories: 2" \
    "safe rm counts recursive directories correctly"


# ============================================================
# SAFE DELETE — DIRECTORY SYMLINK
# ============================================================

SAFE_DELETE_SYMLINK_TARGET="$TEST_DIR/safe-delete-symlink-target"
SAFE_DELETE_SYMLINK_DIR="$TEST_DIR/safe-delete-symlink-dir"

mkdir -p "$SAFE_DELETE_SYMLINK_TARGET"
mkdir -p "$SAFE_DELETE_SYMLINK_DIR"

printf 'outside content' \
    > "$SAFE_DELETE_SYMLINK_TARGET/outside.txt"

ln -s \
    "$SAFE_DELETE_SYMLINK_TARGET" \
    "$SAFE_DELETE_SYMLINK_DIR/link"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'safe rm safe-delete-symlink-dir\nn\nq\n' | "$PROGRAM"
)"

assert_file_exists \
    "$SAFE_DELETE_SYMLINK_TARGET/outside.txt" \
    "safe rm does not traverse directory symlink target"

assert_file_exists \
    "$SAFE_DELETE_SYMLINK_DIR" \
    "safe rm symlink preview cancellation preserves directory"


# ============================================================
# PHASE 15.3 — PROJECT HEALTH CHECK
# ============================================================

# Create a clean project structure for doctor tests.

DOCTOR_TEST_DIR="$TEST_DIR/doctor-test"

mkdir -p "$DOCTOR_TEST_DIR/src"
mkdir -p "$DOCTOR_TEST_DIR/include"
mkdir -p "$DOCTOR_TEST_DIR/tests"

cat > "$DOCTOR_TEST_DIR/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.16)
project(doctor_test)
EOF

cat > "$DOCTOR_TEST_DIR/README.md" <<'EOF'
# Doctor Test Project
EOF


# ============================================================
# DOCTOR — BASIC HEALTH CHECK
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'doctor doctor-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "PROJECT HEALTH CHECK" \
    "doctor displays health check"

assert_contains \
    "$OUTPUT" \
    "CMakeLists.txt found" \
    "doctor checks CMakeLists.txt"

assert_contains \
    "$OUTPUT" \
    "src directory found" \
    "doctor checks src directory"

assert_contains \
    "$OUTPUT" \
    "include directory found" \
    "doctor checks include directory"

assert_contains \
    "$OUTPUT" \
    "tests directory found" \
    "doctor checks tests directory"

assert_contains \
    "$OUTPUT" \
    "README.md found" \
    "doctor checks README"

assert_contains \
    "$OUTPUT" \
    "Git repository detected" \
    "doctor checks Git repository"

assert_contains \
    "$OUTPUT" \
    "Git working tree" \
    "doctor checks Git status"

assert_contains \
    "$OUTPUT" \
    "Summary:" \
    "doctor displays summary"

assert_contains \
    "$OUTPUT" \
    "Checks:" \
    "doctor displays check count"

assert_contains \
    "$OUTPUT" \
    "Passed:" \
    "doctor displays passed count"

assert_contains \
    "$OUTPUT" \
    "Warnings:" \
    "doctor displays warning count"

assert_contains \
    "$OUTPUT" \
    "Errors:" \
    "doctor displays error count"


# ============================================================
# DOCTOR — CLEAN PROJECT STRUCTURE
# ============================================================

assert_contains \
    "$OUTPUT" \
    "[OK] CMakeLists.txt found" \
    "doctor marks CMakeLists.txt as healthy"

assert_contains \
    "$OUTPUT" \
    "[OK] src directory found" \
    "doctor marks src as healthy"

assert_contains \
    "$OUTPUT" \
    "[OK] include directory found" \
    "doctor marks include as healthy"

assert_contains \
    "$OUTPUT" \
    "[OK] tests directory found" \
    "doctor marks tests as healthy"

assert_contains \
    "$OUTPUT" \
    "[OK] README.md found" \
    "doctor marks README as healthy"


# ============================================================
# DOCTOR — MISSING README
# ============================================================

rm "$DOCTOR_TEST_DIR/README.md"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'doctor doctor-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "[WARN] README.md found" \
    "doctor warns about missing README"


# ============================================================
# DOCTOR — MISSING REQUIRED DIRECTORY
# ============================================================

rm -rf "$DOCTOR_TEST_DIR/src"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'doctor doctor-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "[ERROR] src directory found" \
    "doctor detects missing src directory"


# ============================================================
# DOCTOR — MISSING INCLUDE DIRECTORY
# ============================================================

rm -rf "$DOCTOR_TEST_DIR/include"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'doctor doctor-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "[ERROR] include directory found" \
    "doctor detects missing include directory"


# ============================================================
# DOCTOR — MISSING CMAKE
# ============================================================

rm -f "$DOCTOR_TEST_DIR/CMakeLists.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'doctor doctor-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "[ERROR] CMakeLists.txt found" \
    "doctor detects missing CMakeLists.txt"


# ============================================================
# DOCTOR — NONEXISTENT PATH
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'doctor does-not-exist\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Path does not exist" \
    "doctor rejects nonexistent path"


# ============================================================
# DOCTOR — TOO MANY ARGUMENTS
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'doctor one two\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Usage: doctor [path]" \
    "doctor rejects too many arguments"


# ============================================================
# DOCTOR — DEFAULT PATH
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'doctor\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "PROJECT HEALTH CHECK" \
    "doctor uses current directory by default"

# ============================================================
# PHASE 15.4 — FILE EXPLAINER
# ============================================================

WHY_TEST="$TEST_DIR/why-test"

mkdir -p "$WHY_TEST/src"
mkdir -p "$WHY_TEST/include"

cat > "$WHY_TEST/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.16)

project(why_test)

add_executable(why_test
    src/main.cpp
)
EOF

cat > "$WHY_TEST/src/main.cpp" <<'EOF'
#include "../include/test.h"

int main() {
    return 0;
}
EOF

cat > "$WHY_TEST/include/test.h" <<'EOF'
#pragma once

int test_function();
EOF

cat > "$WHY_TEST/README.md" <<'EOF'
# Why Test

Test project for the file explainer.
EOF

cat > "$WHY_TEST/config.json" <<'EOF'
{
    "name": "why-test"
}
EOF

git -C "$WHY_TEST" init -q
git -C "$WHY_TEST" add .
git -C "$WHY_TEST" \
    -c user.name="Test User" \
    -c user.email="test@example.com" \
    commit -q -m "initial test commit"

# SOURCE FILE
OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why why-test/src/main.cpp\nq\n' | "$PROGRAM"
)"

assert_contains "$OUTPUT" \
    "FILE EXPLAINER" \
    "why displays file explainer"

assert_contains "$OUTPUT" \
    "Name: main.cpp" \
    "why displays file name"

assert_contains "$OUTPUT" \
    "Type: Source code" \
    "why identifies C++ source file"

assert_contains "$OUTPUT" \
    "Language: C++" \
    "why identifies C++ language"

assert_contains "$OUTPUT" \
    "Project Role:" \
    "why displays project role"

assert_contains "$OUTPUT" \
    "Related Files:" \
    "why displays related files"

assert_contains "$OUTPUT" \
    "Header:" \
    "why identifies related header"

assert_contains "$OUTPUT" \
    "Git:" \
    "why displays Git information"

assert_contains "$OUTPUT" \
    "Tracked: Yes" \
    "why detects tracked file"

assert_contains "$OUTPUT" \
    "Status: Clean" \
    "why detects clean Git status"

assert_contains "$OUTPUT" \
    "Purpose:" \
    "why displays file purpose"

# HEADER FILE
OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why why-test/include/test.h\nq\n' | "$PROGRAM"
)"

assert_contains "$OUTPUT" \
    "Type: Header / interface" \
    "why identifies header file"

assert_contains "$OUTPUT" \
    "Language: C/C++ header" \
    "why identifies header language"

# CMAKE FILE
OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why why-test/CMakeLists.txt\nq\n' | "$PROGRAM"
)"

assert_contains "$OUTPUT" \
    "Type: Build configuration" \
    "why identifies CMake configuration"

assert_contains "$OUTPUT" \
    "Defines how the project is built." \
    "why explains build configuration"

# README
OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why why-test/README.md\nq\n' | "$PROGRAM"
)"

assert_contains "$OUTPUT" \
    "Type: Project documentation" \
    "why identifies documentation"

# CONFIGURATION FILE
OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why why-test/config.json\nq\n' | "$PROGRAM"
)"

assert_contains "$OUTPUT" \
    "Type: Configuration / data" \
    "why identifies JSON configuration"

# NONEXISTENT FILE
OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why why-test/does-not-exist.txt\nq\n' | "$PROGRAM"
)"

assert_contains "$OUTPUT" \
    "Path does not exist" \
    "why rejects nonexistent file"

# DIRECTORY
OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why why-test/src\nq\n' | "$PROGRAM"
)"

assert_contains "$OUTPUT" \
    "why currently supports files only" \
    "why rejects directories"

# INVALID ARGUMENTS
OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why\nq\n' | "$PROGRAM"
)"

assert_contains "$OUTPUT" \
    "Usage: why <path>" \
    "why validates missing argument"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why one two\nq\n' | "$PROGRAM"
)"

assert_contains "$OUTPUT" \
    "Usage: why <path>" \
    "why validates extra arguments"


# ============================================================
# PHASE 15.5 — SMART PROJECT TREE
# ============================================================

SMART_TREE_TEST="$TEST_DIR/smart-tree-test"

mkdir -p "$SMART_TREE_TEST/src"
mkdir -p "$SMART_TREE_TEST/include"
mkdir -p "$SMART_TREE_TEST/tests"
mkdir -p "$SMART_TREE_TEST/docs"
mkdir -p "$SMART_TREE_TEST/build"

cat > "$SMART_TREE_TEST/src/main.cpp" <<'EOF'
int main() {
    return 0;
}
EOF

cat > "$SMART_TREE_TEST/src/helper.cpp" <<'EOF'
void helper() {
}
EOF

cat > "$SMART_TREE_TEST/include/helper.h" <<'EOF'
#pragma once

void helper();
EOF

cat > "$SMART_TREE_TEST/tests/test_cli.sh" <<'EOF'
#!/usr/bin/env bash
echo "test"
EOF

cat > "$SMART_TREE_TEST/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.16)

project(smart_tree_test)
EOF

cat > "$SMART_TREE_TEST/README.md" <<'EOF'
# Smart Tree Test
EOF

cat > "$SMART_TREE_TEST/.gitignore" <<'EOF'
build/
EOF


# ============================================================
# SMART TREE — BASIC OUTPUT
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'tree --smart smart-tree-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "SMART PROJECT TREE" \
    "tree --smart displays smart tree"


# ============================================================
# SMART TREE — DIRECTORY CLASSIFICATION
# ============================================================

assert_contains \
    "$OUTPUT" \
    "src  [SOURCE]" \
    "tree --smart identifies source directory"

assert_contains \
    "$OUTPUT" \
    "include  [HEADERS]" \
    "tree --smart identifies header directory"

assert_contains \
    "$OUTPUT" \
    "tests  [TESTS]" \
    "tree --smart identifies test directory"

assert_contains \
    "$OUTPUT" \
    "docs  [DOCUMENTATION]" \
    "tree --smart identifies documentation directory"

assert_contains \
    "$OUTPUT" \
    "build  [BUILD OUTPUT]" \
    "tree --smart identifies build directory"


# ============================================================
# SMART TREE — FILE CLASSIFICATION
# ============================================================

assert_contains \
    "$OUTPUT" \
    "main.cpp  [ENTRY POINT]" \
    "tree --smart identifies entry point"

assert_contains \
    "$OUTPUT" \
    "helper.cpp  [SOURCE]" \
    "tree --smart identifies source files"

assert_contains \
    "$OUTPUT" \
    "helper.h  [INTERFACE]" \
    "tree --smart identifies header files"

assert_contains \
    "$OUTPUT" \
    "CMakeLists.txt  [BUILD SYSTEM]" \
    "tree --smart identifies build system"

assert_contains \
    "$OUTPUT" \
    "README.md  [DOCUMENTATION]" \
    "tree --smart identifies documentation"

assert_contains \
    "$OUTPUT" \
    ".gitignore  [GIT CONFIG]" \
    "tree --smart identifies Git configuration"


# ============================================================
# SMART TREE — DEFAULT PATH
# ============================================================

OUTPUT="$(
    cd "$SMART_TREE_TEST" || exit 1
    printf 'tree --smart\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "SMART PROJECT TREE" \
    "tree --smart uses current directory by default"


# ============================================================
# SMART TREE — SUBDIRECTORY
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'tree --smart smart-tree-test/src\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "main.cpp  [ENTRY POINT]" \
    "tree --smart works on subdirectories"


# ============================================================
# SMART TREE — NONEXISTENT PATH
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'tree --smart does-not-exist\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Path does not exist" \
    "tree --smart rejects nonexistent path"


# ============================================================
# SMART TREE — FILE PATH
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'tree --smart smart-tree-test/README.md\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "requires a directory" \
    "tree --smart rejects file paths"


# ============================================================
# SMART TREE — TOO MANY ARGUMENTS
# ============================================================

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'tree --smart one two\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Usage: tree --smart [path]" \
    "tree --smart validates argument count"


# ============================================================
# PHASE 15.6 — INTEGRATION
# ============================================================

INTEGRATION_TEST="$TEST_DIR/integration-test"

mkdir -p "$INTEGRATION_TEST/src"
mkdir -p "$INTEGRATION_TEST/include"
mkdir -p "$INTEGRATION_TEST/tests"
mkdir -p "$INTEGRATION_TEST/docs"

cat > "$INTEGRATION_TEST/src/main.cpp" <<'EOF'
#include "../include/app.h"

int main() {
    return app();
}
EOF

cat > "$INTEGRATION_TEST/include/app.h" <<'EOF'
#pragma once

int app();
EOF

cat > "$INTEGRATION_TEST/src/app.cpp" <<'EOF'
#include "../include/app.h"

int app() {
    return 0;
}
EOF

cat > "$INTEGRATION_TEST/tests/test_app.sh" <<'EOF'
#!/usr/bin/env bash
echo "integration test"
EOF

cat > "$INTEGRATION_TEST/CMakeLists.txt" <<'EOF'
cmake_minimum_required(VERSION 3.16)

project(integration_test)
EOF

cat > "$INTEGRATION_TEST/README.md" <<'EOF'
# Integration Test
EOF

cat > "$INTEGRATION_TEST/.gitignore" <<'EOF'
build/
EOF

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'tree --smart integration-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "SMART PROJECT TREE" \
    "integration tree works"

assert_contains \
    "$OUTPUT" \
    "src  [SOURCE]" \
    "integration tree identifies src"

assert_contains \
    "$OUTPUT" \
    "include  [HEADERS]" \
    "integration tree identifies include"

assert_contains \
    "$OUTPUT" \
    "tests  [TESTS]" \
    "integration tree identifies tests"

assert_contains \
    "$OUTPUT" \
    "CMakeLists.txt  [BUILD SYSTEM]" \
    "integration tree identifies CMake"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'doctor integration-test\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "PROJECT HEALTH CHECK" \
    "integration doctor works"

assert_contains \
    "$OUTPUT" \
    "CMakeLists.txt" \
    "integration doctor checks CMake"

assert_contains \
    "$OUTPUT" \
    "src" \
    "integration doctor checks src"

assert_contains \
    "$OUTPUT" \
    "include" \
    "integration doctor checks include"

assert_contains \
    "$OUTPUT" \
    "tests" \
    "integration doctor checks tests"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'why integration-test/src/main.cpp\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "FILE EXPLAINER" \
    "integration why works"

assert_contains \
    "$OUTPUT" \
    "main.cpp" \
    "integration why identifies file"

assert_contains \
    "$OUTPUT" \
    "C++" \
    "integration why identifies language"

assert_contains \
    "$OUTPUT" \
    "Source code" \
    "integration why identifies project role"

OUTPUT="$(
    cd "$INTEGRATION_TEST" || exit 1
    printf 'snapshot\nshowsnapshot\ndiff\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "Snapshot created." \
    "integration snapshot works"

assert_contains \
    "$OUTPUT" \
    "Snapshot differences:" \
    "integration diff works"

OUTPUT="$(
    cd "$INTEGRATION_TEST" || exit 1
    printf 'touch integration-file.txt\nhistory\nq\n' | "$PROGRAM"
)"

assert_contains \
    "$OUTPUT" \
    "integration-file.txt" \
    "integration history records operation"
    
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