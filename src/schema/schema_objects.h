#include <iostream>
#include <string>
#include <array>
#include <cstdint>

#pragma once

namespace schema {
    struct FileRecord {
        std::array<char, 1024> full_path;// need to get this reursively
        std::array<char, 255> file_name;
        std::array<char, 20> type;
        std::uint64_t size_bytes;
        std::uint16_t mode;
        std::array<char, 20> permissions;
        std::uint64_t inode;
    };

}