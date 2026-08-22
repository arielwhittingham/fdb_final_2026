#include <iostream>
#include <string>
#include <array>
#include <cstdint>

#pragma once

namespace schema {
    struct FileRecord {
        // uint64_t d_ino;
        // uint64_t d_offset;
        // short int d_reclen;
        // short int d_type;
        //std::string d_name;
        std::array<char, 1024> full_path;
        std::array<char, 255> file_name;
        std::array<char, 20> type;
        std::uint64_t size_bytes;
        std::uint64_t mtime;
        std::uint16_t mode;
        std::array<char, 20> permissions;
        std::uint16_t uid;
        std::uint16_t gid;
        std::uint64_t inode;
        std::uint64_t device_id;
        std::uint16_t hard_links;
    };

}