#include <iostream>
#include <filesystem>
#include <typeinfo>
#include <cstdlib>
#include <chrono>
#include <string_view>
#include "../schema/schema_objects.h"
#include <typeinfo>



namespace crawl {


    using namespace schema;
    namespace filesystem = std::filesystem;


    // turn string view into char array of fixed length
    // this is for writing to the char members of the file record struct


    void write_string_view_char_array(char* target_char_array,std::size_t array_size, std::string_view input_string ) {
        std::size_t copy_length = std::min(input_string.length(), array_size - 1);
        std::cout << copy_length << "\n" ;
        std::copy_n(input_string.data(), copy_length, target_char_array);
        std::fill( target_char_array +copy_length, target_char_array + array_size, '\0' );
    }

    void write_raw_records_to_log_file() {
        return;
    }






    void crawl_filesystem(const std::string&  starting_path) {


        int counter{0};
        std::cout << filesystem::canonical(starting_path).string() << std::endl;

        /*
         recursively crawl and write structs
         */


        // void write_string_view_char_array(char* target_char_array,std::size_t array_size, std::string_view input_string)

            schema::FileRecord raw_record;

            // string parameters
            write_string_view_char_array(raw_record.full_path.data(),raw_record.full_path.size(), entry.path().string());
            write_string_view_char_array(raw_record.file_name.data(),raw_record.file_name.size(), entry.path().string());
            write_string_view_char_array(raw_record.type.data(),raw_record.type.size(), entry.path().string());
            write_string_view_char_array(raw_record.permissions.data(),raw_record.permissions.size(), entry.path().string());

            // numeric parameters

                std::uint64_t size_bytes = ,
                std::uint64_t mtime = ,
                std::uint16_t mode = ,
                std::uint16_t uid = ,
                std::uint16_t gid = ,
                std::uint64_t inode = ,
                std::uint64_t device_id = ,
                std::uint16_t hard_links  =
            };

            std::cout << filesystem::canonical(entry.path()).string() << "\n";
            std::cout << filesystem::file_size(entry) / (1024.f * 1024.f) << " MB"  << "\n";









            // compiler trick, simpler
            // static_assert(sizeof(decltype(filesystem::canonical(entry.path()))) == 0, "Check type here");

            ++counter;

        }

        // auto endtime = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double, std::milli> ft =  endtime - starttime;

        std::cout << counter
                  << "<- items  Seconds -> "
                  << ft.count() / 1000.f
                  << " for dirctory -> "
                  << filesystem::canonical(starting_path).string()
                  << "\n";


    }

    /*
      parse file returned to get usefull metadata
    */



}

int main() {


    std::filesystem::directory_entry starting("/Users/arielwhittingham/Downloads");
    // std::filesystem::directory_entry starting(std::getenv("HOME"));

    crawl::crawl_filesystem(starting);

    std::cout << "Home: " <<  std::getenv("HOME") << std::endl;

    std::cout << starting.exists() << "\n";

    schema::FileRecord* random_record;
    random_record->file_name = "Ariel";

    std::cout << random_record->file_name << "\n";




}