cmake_minimum_required(VERSION 4.2)
project(fdb_final_2026)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. On Apple Silicon, Homebrew installs readline to /opt/homebrew
if(APPLE AND EXISTS "/opt/homebrew")
    include_directories("/opt/homebrew/include")
    link_directories("/opt/homebrew/lib")
endif()

add_executable(FileSystemDB
        src/FileSystemDB.cpp
        src/crawl/readdir_lowlevel.cpp
        src/data/filemanager.cpp
        src/index/bplustree.cpp)

target_link_libraries(FileSystemDB PRIVATE readline)