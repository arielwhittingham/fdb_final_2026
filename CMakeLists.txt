cmake_minimum_required(VERSION 4.2)
project(fdb_final_2026)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable( bptree_exec
        src/index/bplustree.cpp)
