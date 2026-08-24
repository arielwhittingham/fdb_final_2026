#include <sys/syscall.h>
#include <sys/attr.h>
#include <iostream>
#include <sys/errno.h>
#include <sys/vnode.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "../schema/schema_objects.h"

namespace crawl {


    using namespace schema;


    struct raw_bytes_returned {
        uint32_t length;                // size of this file's chunk
        attribute_set_t returned_attrs; // ATTR_CMN_RETURNED_ATTRS reference to all the attributes that get returned
        attrreference_t name_ref;       // pointer to length of string names
        mode_t permissions;             //  ATTR_CMN_ACCESSMASK
        timespec crtime;                // ATTR_CMN_CRTIME
        uint32_t document_id;           // From ATTR_CMN_DOCUMENT_ID
        uint32_t file_id;              // From ATTR_CMN_FILEID (u_int64_t)
        uint32_t parent_id;            // From ATTR_CMN_PARENTID(u_int64_t)
        uid_t owner_id;                 // From ATTR_CMN_OWNERID
        off_t total_size;               // off_t
        u_int32_t access_mask;          //

    } __attribute__((aligned(4)));     // alighnment rule

    // print out raw bytes in HEX
    void hexDump(const void* address, int len) {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(address);
        for (int i = 0; i < len; i++) {
            printf("%02x ", p[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
    }

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

        struct ::attrlist attributes;
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
        | ATTR_CMN_ACCESSMASK // mode maybe?
        ;

        //other
        attributes.fileattr = ATTR_FILE_TOTALSIZE | ATTR_FILE_FILETYPE ; // 2

        // make sure starting path can open properly with the correct features
        int open_status = open(starting_path, O_CLOEXEC | O_RDONLY | O_DIRECTORY,0);


        std::cout << " status "<< open_status << std::endl;
        if (open_status == -1) {
            std::cout << "cant open: " << *starting_path << std::endl;
            return;
        }

        uint64_t options = 0;
        // need to mov this inside a later while-loop
        int numatts = getattrlistbulk(open_status, &attributes, &readinto[0], sizeof(readinto), options);

        int entryindex{0};
        auto bufferstart = &readinto[0];
        char *value;
        uint32_t data_length;


        // loop through each returned bulkattrlist item
        for (entryindex = 0; entryindex < numatts; entryindex++) {
            std::cout << "entry    " << entryindex << std::endl;

            // Read entry length safely
            // need to use this to calculate ref pointer later
            std::memcpy(&data_length, bufferstart, sizeof(uint32_t));
            std::cout << "entry len = " << data_length << " byte\n" << std::endl;;


            // map struct to returned bytes
            auto* buff_entry = reinterpret_cast<raw_bytes_returned*>(bufferstart);
            char* name_pointer = reinterpret_cast<char*>(&buff_entry->name_ref) +
                buff_entry->name_ref.attr_dataoffset;
            std::string filename(name_pointer, buff_entry->name_ref.attr_length);

            std::cout << "#  " <<
                entryindex << "\n\n  entry_name (file name): " << filename << "\n";
            std::cout << " entry_len: " <<
                buff_entry->length << " bytes\n";
            std::cout << " inode (file_id): " <<
                buff_entry->file_id << std::endl;
            std::cout << " permissions: " << std::oct <<
                buff_entry->permissions << std::dec << "\n" << std::endl;
            std::cout << " owner_id: " <<
                buff_entry->owner_id << std::endl;
            std::cout << " document_id: "
            << buff_entry->document_id << "\n" << std::endl;
            std::cout << " tv_sec: " <<
                buff_entry->crtime.tv_sec << "\n" << std::endl;
            // std::cout << " access_mask: " <<
            //     buff_entry->access_mask  << "\n" << std::endl;
            // std::cout << " parent_id: " <<
            //     buff_entry->parent_id << "\n" << std::endl;
            // std::cout << " total_size: " <<
            //     buff_entry->total_size << "\n" << std::endl;

            char* ref_ptr = bufferstart + 24;

            attrreference_t name_ref;
            std::memcpy(&name_ref, ref_ptr, sizeof(attrreference_t));

            std::cout << "DEBUG Name Ref: Offset = " << name_ref.attr_dataoffset
                      << "Length = " << name_ref.attr_length << std::endl;

            // name string in relation to the ref variable
            char* name_string_ptr = ref_ptr + name_ref.attr_dataoffset;

            // 4. Safely extract and print the filename
            std::string safe_name(name_string_ptr, name_ref.attr_length);

            // remove null or empty bytes
            if (!safe_name.empty() && safe_name.back() == '\0') {
                safe_name.pop_back();
            }

            std::cout << "Parsed Filename: " << safe_name << std::endl;

            // -----DEBUG END-------
            // ---------------------
            // ----------------- Shift pointer tonext entry block -----------------

            bufferstart += data_length;


            // std::memcpy(&data_length, value, sizeof(data_length));
            //
            // returned_raw_attributes returned_values;
            //
            // returned_values.length = data_length;
            //
            // std::memcpy(&returned_values.attributes, value, sizeof(attribute_set_t));

            // std::cout << " test 1 " << returned_values.name << std::endl;
            // std::cout << " test 2 " << data_length << std::endl;

        }

        /*
         recursively crawl and write structs - OLD
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

    crawl::crawl_filesystem("/Users/arielwhittingham/Documents/fdb_test");

    // std::cout << "Home: " <<  std::getenv("HOME") << std::endl;
    //
    // std::cout << starting.exists() << "\n";

    schema::FileRecord* random_record;
    // random_record->file_name = "Ariel";
    //
    // std::cout << random_record->file_name << "\n";




}