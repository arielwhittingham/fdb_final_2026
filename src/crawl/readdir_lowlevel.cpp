#include <sys/syscall.h>
#include <sys/attr.h>
#include <iostream>
#include <sys/errno.h>
#include <sys/vnode.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "../schema/schema_objects.h"


namespace crawl {


    using namespace schema;
    // namespace filesystem = std::filesystem;


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






    // void crawl_filesystem(const std::string&  starting_path) {
    void crawl_filesystem(const char  *starting_path) {


        int counter{0};
        // std::cout << filesystem::canonical(starting_path).string() << std::endl;

        struct attrlist attributes;
        //buffer
        char readinto[1024 * 64];
        memset(&attributes, 0, sizeof(attributes));


        // file metadata categories
        attributes.bitmapcount = ATTR_BIT_MAP_COUNT;

        // common attributes for all files and dirs
        attributes.commonattr = ATTR_CMN_NAME | ATTR_CMN_RETURNED_ATTRS  //name
        | ATTR_CMN_ACCESSMASK //perm
        | ATTR_CMN_CRTIME //created
        | ATTR_CMN_DOCUMENT_ID //persistent doc id
        | ATTR_CMN_FILEID //File ID u_int64_t
        | ATTR_CMN_PARENTID // parent file id
        | ATTR_CMN_OWNERID //not sure
        | ATTR_CMN_FULLPATH // maybe full path?
        | ATTR_CMN_ACCESSMASK // mode maybe?
        ;

        //other
        attributes.fileattr = ATTR_FILE_TOTALSIZE | ATTR_FILE_FILETYPE ; // 2


        int open_status = open(starting_path, O_CLOEXEC | O_RDONLY | O_DIRECTORY,0);


        std::cout << " status "<< open_status << std::endl;
        if (open_status == -1) {
            std::cout << "cant open: " << *starting_path << std::endl;
            return;
        }

        uint64_t options = 0;
        int numatts = getattrlistbulk(open_status, &attributes, &readinto[0], sizeof(readinto), options);

        std::cout << open_status << std::endl;
        std::cout << &attributes << std::endl;
        std::cout << &readinto[0] << std::endl;
        std::cout << static_cast<void*>(&readinto[0])  << std::endl;
        std::cout << sizeof(readinto) << std::endl;

        std::cout << " atts returned " << numatts << std::endl;

        /*
         recursively crawl and write structs
         */

        // DIR* starting_directory_pointer = opendir(starting_path);
        // // std::cout <<  "what is this" << starting_path << std::endl;
        //
        // std::cout << " dir pointer " << starting_directory_pointer << std::endl;
        //
        // struct dirent* directory_list;

        // while (readdir(starting_directory_pointer) != nullptr) {
        //     auto lst  = readdir(starting_directory_pointer);
        //     std::cout << lst->d_name  << std::endl;
        //     std::cout << lst->d_namlen  << std::endl;
        //     // std::cout << lst->d_type  << std::endl;
        //     std::cout << lst->d_ino  << std::endl;
        //
        // }








        /*
        // void write_string_view_char_array(char* target_char_array,std::size_t array_size, std::string_view input_string)

            schema::FileRecord raw_record;

            // string parameters
            write_string_view_char_array(raw_record.full_path.data(),raw_record.full_path.size(), entry.path().string());
            write_string_view_char_array(raw_record.file_name.data(),raw_record.file_name.size(), entry.path().string());
            write_string_view_char_array(raw_record.type.data(),raw_record.type.size(), entry.path().string());
            write_string_view_char_array(raw_record.permissions.data(),raw_record.permissions.size(), entry.path().string());

            // numeric parameters

                std::uint64_t size_bytes = ,
                std::uint16_t mode = ,
                std::uint64_t inode = ,
            };

            std::cout << filesystem::canonical(entry.path()).string() << "\n";
            std::cout << filesystem::file_size(entry) / (1024.f * 1024.f) << " MB"  << "\n";









            // compiler trick, simpler
            // static_assert(sizeof(decltype(filesystem::canonical(entry.path()))) == 0, "Check type here");

            ++counter;
        */
        }
        // auto endtime = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double, std::milli> ft =  endtime - starttime;

    //     std::cout << counter
    //               << "<- items  Seconds -> "
    //               << ft.count() / 1000.f
    //               << " for dirctory -> "
    //               << filesystem::canonical(starting_path).string()
    //               << "\n";
    //
    //
    // }

    /*
      parse file returned to get usefull metadata
    */



}

int main() {


    // std::filesystem::directory_entry starting("/Users/arielwhittingham/Downloads/demo/"); //test di
    // // std::filesystem::directory_entry starting(std::getenv("HOME"));
    // std::cout << starting.path() << std::endl;

    crawl::crawl_filesystem("/Users/arielwhittingham/Downloads/demo/");

    // std::cout << "Home: " <<  std::getenv("HOME") << std::endl;
    //
    // std::cout << starting.exists() << "\n";

    schema::FileRecord* random_record;
    // random_record->file_name = "Ariel";
    //
    // std::cout << random_record->file_name << "\n";




}