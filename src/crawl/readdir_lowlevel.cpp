#include <sys/syscall.h>
#include <sys/attr.h>
#include <iostream>
#include <sys/errno.h>
#include <sys/vnode.h>
#include <sys/_types/_guid_t.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "../schema/schema_objects.h"

namespace crawl {
    using namespace schema;


    struct raw_bytes_returned {
        // header
        uint32_t length;                // 4 bytes
        attribute_set_t returned_attrs; // 4 bytes

        // atts
        attrreference_t name_ref;       // 8 bytes
        uint32_t permissions;           // +4 bytes internally
        // uint32_t document_id;    // remove for now
        struct timespec crtime;         // 16 bytes (Lands cleanly on byte 24)
        uint64_t file_id;               // 8 bytes //Lands cleanly on byte 48
        uint64_t parent_id;             // 8 bytes
        //file meta
        off_t total_size;               // 8 bytes
        uint32_t file_type;             // 4 bytes

    }; // no alignment rule

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

        attrlist attributes;
        //buffer
        char readinto[1024 * 64];
        memset(&attributes, 0, sizeof(attributes));

        // https://github.com/apple-oss-distributions/xnu/blob/main/bsd/man/man2/getattrlist.2
        // ATTR list from apple^^

        // attr.h structs: https://github.com/apple-oss-distributions/xnu/blob/main/bsd/sys/attr.h

        // file metadata categories that are filled
        attributes.bitmapcount = ATTR_BIT_MAP_COUNT;

        // common attributes for all files and dirs
        // needs to align with the struct above?
        attributes.commonattr =
              ATTR_CMN_RETURNED_ATTRS
            | ATTR_CMN_NAME
            | ATTR_CMN_OBJTYPE
            | ATTR_CMN_CRTIME
            | ATTR_CMN_ACCESSMASK
            // | ATTR_CMN_DOCUMENT_ID
            | ATTR_CMN_FILEID
            | ATTR_CMN_PARENTID;

        attributes.fileattr =
              ATTR_FILE_TOTALSIZE;
        // make sure starting path can open properly with the correct features
        int open_status = open(starting_path, O_CLOEXEC | O_RDONLY | O_DIRECTORY,0);


        std::cout << " status "<< open_status << std::endl;
        if (open_status == -1) {
            std::cout << "cant open: " << *starting_path << std::endl;
            return;
        }

        uint64_t options = 0;
        // need to mov this inside a later while-loop
        while (int numatts = getattrlistbulk(open_status, &attributes, &readinto[0], sizeof(readinto), options) > 0) {



        int entryindex{0};
        auto bufferstart = &readinto[0];
        char *value;
        uint32_t data_length;


        // loop through each returned bulkattrlist item
        for (entryindex = 0; entryindex < numatts; entryindex++) {

            // entry length
            char* cur = bufferstart;
            std::memcpy(&data_length, cur, sizeof(data_length));
            cur += sizeof(data_length);

            // returned attributes
            attribute_set_t returned_attrs{};
            std::memcpy( &returned_attrs, cur,sizeof(returned_attrs));
            cur += sizeof(returned_attrs);

            // ATTR_CMN_NAME

            std::string filename;

            if (returned_attrs.commonattr & ATTR_CMN_NAME) {
                char* ref_location = cur;
                attrreference_t name_ref{};
                std::memcpy( &name_ref, cur, sizeof(name_ref));

                cur += sizeof(name_ref);
                char* name_pointer = ref_location + name_ref.attr_dataoffset;
                filename.assign(name_pointer,name_ref.attr_length);

                // remove null terminator
                if (!filename.empty() && filename.back() == '\0') {
                    filename.pop_back();
                }
            }
            // ATTR_CMN_OBJTYPE


            fsobj_type_t object_type = 0;

            if (returned_attrs.commonattr & ATTR_CMN_OBJTYPE) {

                std::memcpy(&object_type, cur,sizeof(object_type));
                cur += sizeof(object_type);
            }

            // ATTR_CMN_CRTIME -- struct not just a single valu


            timespec crtime{};

            if (returned_attrs.commonattr & ATTR_CMN_CRTIME) {

                std::memcpy(&crtime,cur,sizeof(crtime)
                );

                cur += sizeof(crtime);
            }

            // ATTR_CMN_ACCESSMASK

            uint32_t permissions = 0;
            if (returned_attrs.commonattr & ATTR_CMN_ACCESSMASK) {
                std::memcpy(&permissions,cur,sizeof(permissions));
                cur += sizeof(permissions);
            }

            // remove document_id foor now
            // uint32_t document_id = 0;
            //
            // if (returned_attrs.commonattr & ATTR_CMN_DOCUMENT_ID) {
            //     std::memcpy(&document_id,cur,sizeof(document_id)
            //     );
            //
            //     cur += sizeof(document_id);
            // }


            // this is the inode
            // ATTR_CMN_FILEID

            uint64_t file_id = 0;
            if (returned_attrs.commonattr & ATTR_CMN_FILEID) {

                std::memcpy(
                    &file_id,
                    cur,
                    sizeof(file_id)
                );

                cur += sizeof(file_id);
            }


            // ---------------------------------
            // ATTR_CMN_PARENTID
            // ---------------------------------

            uint64_t parent_id = 0;

            if (returned_attrs.commonattr & ATTR_CMN_PARENTID) {
                std::memcpy( &parent_id, cur,sizeof(parent_id)
                );
                cur += sizeof(parent_id);
            }


            // ---------------------------------
            // ATTR_FILE_TOTALSIZE
            // ---------------------------------

            off_t total_size = 0;

            if (returned_attrs.fileattr & ATTR_FILE_TOTALSIZE) {

                std::memcpy(
                    &total_size,
                    cur,
                    sizeof(total_size)
                );

                cur += sizeof(total_size);
            }

            std::cout << "\n size of returned_atts variable: " << sizeof(returned_attrs) << "\n";
            std::cout << "num atts: " << numatts<< "\n";
            std::cout << "  file size: " << sizeof(filename) << std::endl;
            std::cout << "  file: " << filename << std::endl;
            std::cout << "  file: " << file_id << std::endl;
            std::cout << "  size bytes " <<total_size << std::endl;
            std::cout << " perms  " <<permissions << std::endl;
            // std::cout << " doc_id  " <<document_id << std::endl;

            std::cout << " parent_inode  " <<parent_id << std::endl;
            std::cout <<  "  seconds " <<crtime.tv_sec<< std::endl;
            std::cout << "  objtype " <<object_type << std::endl;



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

        /*
          parse file returned to get usefull metadata
        */



    }
}

int main() {


    // std::filesystem::directory_entry starting("/Users/arielwhittingham/Downloads/demo/"); //test di
    // // std::filesystem::directory_entry starting(std::getenv("HOME"));
    // std::cout << starting.path() << std::endl;

    crawl::crawl_filesystem("/Users/arielwhittingham/");

    // std::cout << "Home: " <<  std::getenv("HOME") << std::endl;
    //
    // std::cout << starting.exists() << "\n";

    schema::FileRecord* random_record;
    // random_record->file_name = "Ariel";
    //
    // std::cout << random_record->file_name << "\n";



}