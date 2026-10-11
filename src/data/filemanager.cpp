#include "filemanager.h"
#include <iostream>
#include <array>
#include <vector>
#include "../schema/schema_objects.h"
#include "../index/structs_bplustree.h"
#include <string>
#include <string_view>
#include <fstream>
#include <variant>
#include <filesystem>


namespace FileManager
  {
  namespace fs = std::filesystem;


  /*

💡 Core ConceptThink of std::lower_bound as finding the earliest position where
you can insert a value without breaking the
sorted order of the container.If the value is found in the range,
it returns an iterator pointing to the first
occurrence of that value.If the value is not found, it returns an iterator pointing to the next
larger element.

If all elements in the range are smaller than the given value, it returns the end() iterator.

*/

  //------------------------Record Management------------------------------------------------


    BufferManager::BufferManager()
      {
        //create dir if not exist
        try
          {
            fs::create_directory(dir);
            fs::permissions(
              dir,
              fs::perms::owner_all
            );
          } catch (fs::filesystem_error &e)
            {
              std::cerr << "filesystem error, can't make directory" << e.what() <<
                  std::endl;
              throw;
            }
        filestream.open(DB_FILE_NAME,
                 std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);

          }



    uint32_t BufferManager::create_new_record_id()
      {
        this->record_count++;
        return this->record_count;
      }

    uint32_t BufferManager::write_full_record_to_file(const schema::FileRecord &full_record_no_record_id)
      {
        uint32_t new_record_id = this->create_new_record_id();
        this->records.push_back(new_record_id);
        Struc::StorageRecord full_record
        {
        .record_id = new_record_id,
        .row = full_record_no_record_id
        };
        filestream.seekp(static_cast<std::streamoff>(new_record_id - 1)
                 * sizeof(Struc::StorageRecord));
        filestream.write(
          reinterpret_cast<const char *>(&full_record),sizeof(Struc::StorageRecord));
        std::cout << "record_id: " << new_record_id << std::endl;
        return new_record_id;
      }

    Struc::StorageRecord BufferManager::read_from_file_to_memory(uint32_t record_id)
      {
        filestream.flush();
        Struc::StorageRecord storage_record{};
        uint32_t index_of_record_id= record_id -1;
        std::ifstream local_filestream(DB_FILE_NAME,std::ios::binary | std::ios::in);

        std::streamoff offset_len  = static_cast<std::streamoff>(std::streamoff(index_of_record_id * sizeof(Struc::StorageRecord)));
        local_filestream.seekg(offset_len, std::ios::beg);
        local_filestream.read(reinterpret_cast<char*>(&storage_record), sizeof(Struc::StorageRecord));

        //check if file is not currupted?
        // if (local_filestream.good())
        //   {
        //
        //   }

        return storage_record;
      }
  }