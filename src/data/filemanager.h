
#pragma once
// #include "include/fdb_final_2026/bplustree.h"

#include <iostream>
#include <array>
#include <vector>
#include "../schema/schema_objects.h"
#include "../index/structs_bplustree.h"
#include <string>
#include <fstream>
#include <variant>
#include <filesystem>


namespace FileManager
  {
  namespace fs = std::filesystem;
  inline const std::string DB_FILE_LOCATION = "../test/";
  inline const std::string DB_FILE_NAME = "../test/databasefile.db";

  //-----------------HELPER Non-Member Methods ----------------

  template<std::size_t N>
  bool element_exists(uint32_t search_value, std::array<uint32_t, N> &search_array)
    {
      uint32_t return_index = 0;
      for (const auto &item: search_array)
        {
          if (item == search_value)
            {
              return true;
            }
          return_index++;
        }
      return false;
    }

  //------------------------Record Management------------------------------------------------

  class BufferManager
    {
  public:
    fs::path dir = DB_FILE_LOCATION;
    std::fstream filestream;
    uint32_t record_count = 0;
    std::vector<uint32_t> records;

    BufferManager();

    uint32_t create_new_record_id();

    uint32_t write_full_record_to_file(const schema::FileRecord &full_record_no_record_id);

    Struc::StorageRecord read_from_file_to_memory(uint32_t record_id);
    };
  }