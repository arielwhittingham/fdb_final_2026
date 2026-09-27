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


  void crawl_filesystem(const char *starting_path,const std::function<uint32_t(const schema::FileRecord &)> &emit);
  }


void run_application()
  {
    /*----------------------------------------------------------------
         STEP 1)

         Crawl Filesystem starting at some directory
    ----------------------------------------------------------------*/
    std::cout << " \n STEP 1) " << "Crawl Filesystem starting at some directory" << std::endl;

    FileManager::BufferManager database_file;
    auto sink = [&database_file](const schema::FileRecord &rec) -> uint32_t
      {
      return database_file.write_full_record_to_file(rec);
      };


    auto starting_dir = "/Users/arielwhittingham/Documents/fdb_parent";
    crawl::crawl_filesystem(starting_dir, sink);

    auto recore = database_file.read_from_file_to_memory(27);
    std::cout << recore.to_string() << "\n";
  }


int main()
  {
    run_application();
    std::cout << "done" << std::endl;

    return 0;
  }
