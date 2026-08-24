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
        std::uint64_t size_bytes; // off_t
        std::uint16_t mode; // access_mask..?
        std::array<char, 20> permissions; //permissions
        std::uint64_t inode; // file_id
        std::uint64_t parent_id; // parent_id
        std::uint32_t uid; //document_id
        std::uint64_t created_timestamp; // crtime
        std::uint32_t size=0; //record size total - need to calculate; //,aybe length?



    };

}