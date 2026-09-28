#include <iostream>
#include <cstring>
#include <functional>
#include "data/filemanager.h"
#include "index/bplustree.h"


namespace crawl
  {
  void write_string_view_char_array(char *target_char_array, std::size_t array_size,
                                    std::string_view input_string);


  void write_raw_records_to_log_file();


  void append_to_create_new_file_path(const char *new_folder_name, const char *starting, char *op,
                                      size_t op_size = PATH_MAX);


  void crawl_filesystem(const char *starting_path,
                        const std::function<uint32_t(const schema::FileRecord &)> &emit);
  }

constexpr char lowercase(char input_letter)
  {
    if (input_letter <= 'Z' && input_letter >= 'A')
      {
        return char(input_letter + 32);
      }
    else
      {
        return input_letter;
      }
  }

Struc::FileNameDataType normalize_name_key(const std::array<char, 255> &input_string)
  {
    Struc::FileNameDataType op{};

    for (std::size_t i = 0; i < input_string.size() && input_string[i] != '\0'; ++i)
      {
        op[i] = lowercase(input_string[i]);
      }
    return op;
  }

void run_application()
  {
    /*----------------------------------------------------------------
         STEPS
         1) Crawl Filesystem starting at some directory
         2) Create Database File Buffer Object
         3) Create B+Tree Objects for each index field chosen
         4) Create emit function to pass data to the database file object
         and B+Tree objects
         5)

    ----------------------------------------------------------------*/
    std::cout << " \n Create File Buffer Object " << "\n";

    FileManager::BufferManager database_file;

    std::cout << " \n Create B+Trees " << "\n";

    Index::BPlusTreeManager<Struc::FileNameDataType> file_name_index("file_name");
    Index::BPlusTreeManager<std::uint64_t> size_index("size_bytes");


    auto sink = [&database_file, &file_name_index, &size_index](
      const schema::FileRecord &rec) -> uint32_t
      {
      uint32_t recid = database_file.write_full_record_to_file(rec);

      Struc::IndexRecord<Struc::FileNameDataType> file_rec{
      recid, normalize_name_key(rec.file_name)
      };

      Struc::IndexRecord<uint64_t> size_rec{
      .record_id = recid,
      .field_value = rec.size_bytes
      };


      file_name_index.add_record(file_rec);
      size_index.add_record(size_rec);


      return recid;
      };

    auto starting_dir = "/Users/arielwhittingham/Documents/CodingNomads";
    crawl::crawl_filesystem(starting_dir, sink);

    auto recore = database_file.read_from_file_to_memory(650);
    std::cout << " \n\n READ BACK RECORD: " << recore.to_string() << "\n";
  }


int main()
  {
    run_application();
    std::cout << "done" << std::endl;

    return 0;
  }
