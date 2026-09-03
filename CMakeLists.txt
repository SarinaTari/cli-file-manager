cmake_minimum_required(VERSION 3.16)

project(cli_file_manager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(filemanager
    src/main.cpp
    src/FileManager.cpp
    src/CommandParser.cpp
)

target_include_directories(filemanager PRIVATE
    include
)