#include <iostream>
#include <cstring>
#include <functional>
#include <numbers>
#include <stdexcept>
#include "data/filemanager.h"
#include "index/bplustree.h"
#include "query/query_utils.h"
#include <readline/readline.h>
#include <readline/history.h>


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

std::vector<std::string> return_tabular_file_records(std::vector<schema::FileRecord> v)
  {
    std::vector<std::string> rows{};

    rows.push_back({"inode, file_name, parent_inode, created, size,mode, type "});
    for (auto x : v)

      {
        std::string row_str{};
        row_str += std::to_string(x.inode) + ", ";
        row_str += std::string(x.file_name.data()) + ", ";
        row_str += std::to_string(x.parent_inode) + ", ";
        row_str += std::to_string(x.created_timestamp) + ", ";
        row_str += std::to_string(x.size_bytes) + ", ";
        row_str += std::to_string(x.mode) + ", ";
        row_str += std::to_string(x.type) + '\n';
        rows.push_back(row_str);
      }
    return rows;
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
std::string& normalize_query(std::string  &input_string)
  {


    for (char &c : input_string) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return input_string;
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

    auto starting_dir = "/Users/arielwhittingham/Documents/fdb_parent";
    crawl::crawl_filesystem(starting_dir, sink);

    auto recore = database_file.read_from_file_to_memory(650);
    std::cout << " \n\n READ BACK RECORD: " << recore.to_string() << "\n";

    std::cout << " \n\n Crawl and B+Tree Generation Complete. " << std::endl;


    bool continue_querying = true;
    while (continue_querying)
      {
        while (true)
          {
            // std::string query_string;
            std::string new_query_input{};

            std::cout << "Input query string: ";
            char* buffer = readline("Query: ");

            if (buffer) {
                new_query_input = std::string(buffer);

                // 2. Optional: Add to history so UP/DOWN arrows work too!
                if (!new_query_input.empty()) {
                    add_history(buffer);
                }

                // 3. Free the memory allocated by readline
                free(buffer);
            }
            std::string where_clause = query::get_where_clause(new_query_input);
            std::vector<std::string> parsed_tokens;
/*
             1. Check query is valid and return tokens for primary where clause.
             */
            try
              {
                parsed_tokens = query::primary_where_clause_parser(where_clause);
              }
            catch (std::runtime_error &e)
              {
                std::cerr << "\n Enter A Different Query: " << e.what() << std::endl;
                continue;
              }

            char index_field = parsed_tokens[1][0];

            normalize_query(new_query_input);

            std::vector<uint32_t> output{};
            switch (index_field)
              {
                case 'f' : //file_name
                  std::cout <<  "" << std::endl;
                  output = file_name_index.return_matching_record_ids(std::string_view(new_query_input));
                  break;
                case 's': //size_bytes
                  output = size_index.return_matching_record_ids(std::string_view(new_query_input));
                  std::cout << ""  << std::endl;
                  break;
                default:
                  std::cout << " full scan " << std::endl;
                  break;
              }


            std::vector<schema::FileRecord> op{};
            for (auto& x : output)
              {

                auto recore = database_file.read_from_file_to_memory(x);
                op.push_back(recore.row);

              }
            std::vector<std::string> ret = return_tabular_file_records(op);
            for (auto& r : ret)
              {
                std::cout << r  << std::endl;

              }

            break;
          }

        continue_querying = false;

      }



  }


int main()
  {
    run_application();
    std::cout << "done" << std::endl;

    return 0;
  }
