cmake_minimum_required(VERSION 3.16)

project(
    CLIFileManager
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
    filemanager
    src/main.cpp
)

target_include_directories(
    filemanager
    PRIVATE
    include
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        filemanager
        PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()