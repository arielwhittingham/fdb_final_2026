#pragma once
#include <iostream>
#include <array>
#include <string>

#pragma pack(push, 1)

namespace schema {
    struct FileRecord {
        std::uint64_t inode;
        std::uint64_t parent_inode;
        std::uint64_t created_timestamp; // crtime
        std::uint64_t size_bytes; // off_t
        std::uint32_t mode; // access_mask..?
        std::uint16_t type; //make this into an enum
        std::array<char, 255> file_name{}; // keeping last to align memory efficiently


        std::string to_string() const {
            return
                    "\n inode: " + std::to_string(inode) +
                    "\n parent_inode: " + std::to_string(parent_inode) +
                    "\n created: " + std::to_string(created_timestamp) +
                    "\n size: " + std::to_string(size_bytes) +
                    "\n mode: " + std::to_string(mode) +
                    "\n type: " + std::to_string(type) +
                    "\n file name: " + std::string(file_name.data());
        }
    };

}

#pragma pack(pop)
