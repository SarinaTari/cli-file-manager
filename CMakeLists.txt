cmake_minimum_required(VERSION 3.16)

project(cli_file_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(filemanager
    src/main.cpp
    src/FileManager.cpp
    src/CommandParser.cpp
    src/TerminalUI.cpp
    src/FileHasher.cpp
    src/DuplicateDetector.cpp
    src/StorageAnalyzer.cpp
    src/ProjectDetector.cpp
    src/GitAnalyzer.cpp
    src/DependencyAnalyzer.cpp
    src/HistoryManager.cpp
    src/UndoManager.cpp
    src/SnapshotManager.cpp
    src/ProjectDashboard.cpp
)

target_include_directories(filemanager PRIVATE
    include
)

enable_testing()

add_test(
    NAME cli_file_manager_integration
    COMMAND bash
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_cli.sh
        $<TARGET_FILE:filemanager>
)