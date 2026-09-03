#!/usr/bin/env bash

set -u

PROGRAM="$1"

if [ ! -x "$PROGRAM" ]; then
    echo "ERROR: Program does not exist or is not executable:"
    echo "$PROGRAM"
    exit 1
fi

TEST_DIR="$(mktemp -d "${TMPDIR:-/tmp}/cli-file-manager-test.XXXXXX")"
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

echo "========================================"
echo "CLI File Manager - Integration Tests"
echo "========================================"
echo

echo "Test directory:"
echo "$TEST_DIR"
echo

# --------------------------------------------------
# 1. Basic startup / pwd
# --------------------------------------------------

OUTPUT="$(run_program 'pwd\nq\n')"

assert_contains \
    "$OUTPUT" \
    "$TEST_DIR" \
    "pwd shows the current directory"


# --------------------------------------------------
# 2. File creation
# --------------------------------------------------

OUTPUT="$(run_program 'touch file.txt\nq\n')"

assert_file_exists \
    "$TEST_DIR/file.txt" \
    "touch creates a file"


# --------------------------------------------------
# 3. Directory creation
# --------------------------------------------------

OUTPUT="$(run_program 'mkdir folder\nq\n')"

assert_file_exists \
    "$TEST_DIR/folder" \
    "mkdir creates a directory"


# --------------------------------------------------
# 4. Directory navigation
# --------------------------------------------------

OUTPUT="$(run_program 'mkdir navigation\ncd navigation\npwd\nq\n')"

assert_contains \
    "$OUTPUT" \
    "$TEST_DIR/navigation" \
    "cd changes the current directory"


# --------------------------------------------------
# 5. Quoted paths
# --------------------------------------------------

OUTPUT="$(run_program 'mkdir "My Folder"\ncd "My Folder"\npwd\nq\n')"

assert_contains \
    "$OUTPUT" \
    "$TEST_DIR/My Folder" \
    "quoted paths work correctly"


# --------------------------------------------------
# 6. Rename
# --------------------------------------------------

OUTPUT="$(run_program 'touch old.txt\nrename old.txt new.txt\nq\n')"

assert_file_not_exists \
    "$TEST_DIR/old.txt" \
    "rename removes the old name"

assert_file_exists \
    "$TEST_DIR/new.txt" \
    "rename creates the new name"


# --------------------------------------------------
# 7. Copy
# --------------------------------------------------

OUTPUT="$(run_program 'touch source.txt\ncp source.txt copy.txt\nq\n')"

assert_file_exists \
    "$TEST_DIR/copy.txt" \
    "cp creates a copy"


# --------------------------------------------------
# 8. Move
# --------------------------------------------------

OUTPUT="$(run_program 'touch move-source.txt\nmv move-source.txt moved.txt\nq\n')"

assert_file_not_exists \
    "$TEST_DIR/move-source.txt" \
    "mv removes the original location"

assert_file_exists \
    "$TEST_DIR/moved.txt" \
    "mv creates the destination"


# --------------------------------------------------
# 9. File information
# --------------------------------------------------

OUTPUT="$(run_program 'touch info.txt\ninfo info.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "info.txt" \
    "info displays file information"


# --------------------------------------------------
# 10. Find by name
# --------------------------------------------------

OUTPUT="$(run_program 'touch searchable.txt\nfind searchable.txt .\nq\n')"

assert_contains \
    "$OUTPUT" \
    "searchable.txt" \
    "find locates a file by name"


# --------------------------------------------------
# 11. Find by extension
# --------------------------------------------------

OUTPUT="$(run_program 'touch extension.txt\nfindext .txt .\nq\n')"

assert_contains \
    "$OUTPUT" \
    "extension.txt" \
    "findext locates files by extension"


# --------------------------------------------------
# 12. Find by size
# --------------------------------------------------

OUTPUT="$(run_program 'touch sized.txt\nfindsize 0 .\nq\n')"

assert_contains \
    "$OUTPUT" \
    "sized.txt" \
    "findsize locates files by minimum size"


# --------------------------------------------------
# 13. Permissions
# --------------------------------------------------

OUTPUT="$(run_program 'touch permission.txt\nperm permission.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "permission.txt" \
    "perm displays file permissions"


# --------------------------------------------------
# 14. chmod
# --------------------------------------------------

OUTPUT="$(run_program 'touch chmod.txt\nchmod 755 chmod.txt\nperm chmod.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "rwx" \
    "chmod changes permissions"


# --------------------------------------------------
# 15. Symbolic link
# --------------------------------------------------

OUTPUT="$(run_program 'touch target.txt\nln -s target.txt symbolic.txt\nreadlink symbolic.txt\nq\n')"

assert_file_exists \
    "$TEST_DIR/symbolic.txt" \
    "ln -s creates a symbolic link"

assert_contains \
    "$OUTPUT" \
    "target.txt" \
    "readlink displays the symbolic link target"


# --------------------------------------------------
# 16. Hard link
# --------------------------------------------------

OUTPUT="$(run_program 'touch hard-target.txt\nln hard-target.txt hard-link.txt\nq\n')"

assert_file_exists \
    "$TEST_DIR/hard-link.txt" \
    "ln creates a hard link"


# --------------------------------------------------
# 17. Tree
# --------------------------------------------------

OUTPUT="$(run_program 'mkdir tree-test\ntouch tree-test/file.txt\ntree .\nq\n')"

assert_contains \
    "$OUTPUT" \
    "tree-test" \
    "tree displays directories"

assert_contains \
    "$OUTPUT" \
    "file.txt" \
    "tree displays nested files"


# --------------------------------------------------
# 18. Directory size
# --------------------------------------------------

OUTPUT="$(run_program 'mkdir size-test\ntouch size-test/file.txt\ndu size-test\nq\n')"

assert_contains \
    "$OUTPUT" \
    "size-test" \
    "du displays directory information"


# --------------------------------------------------
# 19. Invalid command arguments
# --------------------------------------------------

OUTPUT="$(run_program 'cd\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Usage" \
    "invalid argument count is rejected"


# --------------------------------------------------
# 20. Nonexistent file
# --------------------------------------------------

OUTPUT="$(run_program 'info does-not-exist.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "does-not-exist.txt" \
    "nonexistent file produces an error"


# --------------------------------------------------
# 21. Overwrite protection
# --------------------------------------------------

OUTPUT="$(run_program 'touch a.txt\ntouch b.txt\ncp a.txt b.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "already exists" \
    "copy refuses to overwrite an existing destination"


# --------------------------------------------------
# 22. Invalid chmod
# --------------------------------------------------

OUTPUT="$(run_program 'touch invalid-mode.txt\nchmod 999 invalid-mode.txt\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Permission mode must contain only octal digits" \
    "invalid chmod mode is rejected"


# --------------------------------------------------
# 23. Invalid findsize
# --------------------------------------------------

OUTPUT="$(run_program 'findsize abc .\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Minimum size must be a non-negative integer" \
    "invalid findsize value is rejected"


# --------------------------------------------------
# 24. Unmatched quotes
# --------------------------------------------------

OUTPUT="$(run_program 'cd "unfinished\nq\n')"

assert_contains \
    "$OUTPUT" \
    "Unmatched quote" \
    "unmatched quotes are detected"


# --------------------------------------------------
# 25. Dangerous deletion: current directory
# --------------------------------------------------

OUTPUT="$(run_program 'rm .\nq\n')"

if [ -d "$TEST_DIR" ]; then
    pass "rm . cannot delete the current directory"
else
    fail "rm . cannot delete the current directory"
fi


# --------------------------------------------------
# 26. Dangerous deletion: parent directory
# --------------------------------------------------

OUTPUT="$(run_program 'rm ..\nq\n')"

if [ -d "$TEST_DIR" ]; then
    pass "rm .. cannot delete the parent directory"
else
    fail "rm .. cannot delete the parent directory"
fi


# --------------------------------------------------
# 27. Recursive deletion cancellation
# --------------------------------------------------

mkdir "$TEST_DIR/delete-no"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'rm delete-no\nn\nq\n' | "$PROGRAM"
)"

if [ -d "$TEST_DIR/delete-no" ]; then
    pass "recursive deletion can be cancelled"
else
    fail "recursive deletion can be cancelled"
fi


# --------------------------------------------------
# 28. Recursive deletion confirmation
# --------------------------------------------------

mkdir "$TEST_DIR/delete-yes"
touch "$TEST_DIR/delete-yes/file.txt"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'rm delete-yes\ny\nq\n' | "$PROGRAM"
)"

assert_file_not_exists \
    "$TEST_DIR/delete-yes" \
    "recursive deletion works after confirmation"


# --------------------------------------------------
# 29. Copy directory into itself
# --------------------------------------------------

mkdir -p "$TEST_DIR/project/src"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'cp project project/src/copy\nq\n' | "$PROGRAM"
)"

assert_file_not_exists \
    "$TEST_DIR/project/src/copy" \
    "copying a directory into itself is rejected"


# --------------------------------------------------
# 30. Move directory into itself
# --------------------------------------------------

mkdir -p "$TEST_DIR/move-project/src"

OUTPUT="$(
    cd "$TEST_DIR" || exit 1
    printf 'mv move-project move-project/src/moved\nq\n' | "$PROGRAM"
)"

if [ -d "$TEST_DIR/move-project" ]; then
    pass "moving a directory into itself is rejected"
else
    fail "moving a directory into itself is rejected"
fi


# --------------------------------------------------
# Final result
# --------------------------------------------------

echo
echo "========================================"
echo "Test Summary"
echo "========================================"

echo "Passed: $PASS_COUNT"
echo "Failed: $FAIL_COUNT"
echo

if [ "$FAIL_COUNT" -eq 0 ]; then
    echo "ALL TESTS PASSED"
    exit 0
else
    echo "SOME TESTS FAILED"
    exit 1
fi