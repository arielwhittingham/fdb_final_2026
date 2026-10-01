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
#include <stdexcept>
#include <string.h>
#include <string_view>
#include <vector>
#include <climits>
#include <functional>
#include <cstdio>
#include <cstring>
#include "../schema/schema_objects.h"
#include "../index/structs_bplustree.h"

namespace crawl
  {
  using namespace schema;
  uint32_t global_file_counter{0};
  std::vector<char *> global_file_list{};


  struct raw_bytes_returned
    {
    // header
    uint32_t length; // 4 bytes
    attribute_set_t returned_attrs; // 4 bytes

    // atts
    attrreference_t name_ref; // 8 bytes
    uint32_t permissions; // +4 bytes internally
    // uint32_t document_id;    // remove for now
    struct timespec crtime; // 16 bytes (Lands cleanly on byte 24)
    uint64_t file_id; // 8 bytes //Lands cleanly on byte 48
    uint64_t parent_id; // 8 bytes
    //file meta
    off_t total_size; // 8 bytes
    uint32_t file_type; // 4 bytes
    }; // no alignment rule

  // print out raw bytes in HEX
  void hexDump(const void *address, int len)
    {
      const unsigned char *p = reinterpret_cast<const unsigned char *>(address);
      for (int i = 0; i < len; i++)
        {
          printf("%02x ", p[i]);
          if ((i + 1) % 16 == 0) printf("\n");
        }
      printf("\n");
    }

  // turn string view into char array of fixed length
  // this is for writing to the char members of the file record struct


  void write_string_view_char_array(char *target_char_array, std::size_t array_size,
                                    std::string_view input_string)
    {
      std::size_t copy_length = std::min(input_string.length(), array_size - 1);
      std::cout << copy_length << "\n";
      std::copy_n(input_string.data(), copy_length, target_char_array);
      std::fill(target_char_array + copy_length, target_char_array + array_size, '\0');
    }

  void write_raw_records_to_log_file()
    {
      return;
    }


  int buffer_reads_count{0};
  // void crawl_filesystem(const std::string&  starting_path) {

  void append_to_create_new_file_path(const char *new_folder_name, const char *starting, char *op,
                                      size_t op_size = PATH_MAX)
    {
      int check = snprintf(op, op_size, "%s/%s", starting, new_folder_name);
      //invlaid or too long
      if (check < 0 || static_cast<std::size_t>(check) >= PATH_MAX)
        {
          std::cout << "ERROR: path_max is too big" << std::endl;
          std::cerr << "ERROR: path_max is too big" << std::endl;
          throw std::runtime_error("path_max is too big");
        }
      // std::cout << " new folder name: " << op << std::endl;
      // return op;
    }


  void crawl_filesystem(const char *starting_path,
                        const std::function<uint32_t(const FileRecord &)> &emit)
    {
      struct file_crawl_object
        {
        int descriptor;
        char new_path[4096];
        };

      std::vector<file_crawl_object> directory_vec;
      int open_status = open(starting_path, O_CLOEXEC | O_RDONLY | O_DIRECTORY, 0);

      // int open_status = open(starting_path, O_CLOEXEC | O_RDONLY | O_DIRECTORY, 0);

      file_crawl_object crawl_obj{};


      // make sure starting path can open properly before adding it to crawl
      if (!(open_status == -1))
        {
          crawl_obj.descriptor = open_status;
          strncpy(crawl_obj.new_path, starting_path, sizeof(crawl_obj.new_path) - 1);
          crawl_obj.new_path[sizeof(crawl_obj.new_path) - 1] = '\0';
          directory_vec.push_back(crawl_obj);
        }

      while (!directory_vec.empty())
        {
          attrlist attributes;
          //buffer
          char readinto[1024 * 64];
          memset(&attributes, 0, sizeof(attributes));
          file_crawl_object file_current = directory_vec.back();
          directory_vec.pop_back();

          // https://github.com/apple-oss-distributions/xnu/blob/main/bsd/man/man2/getattrlist.2
          // ATTR list from apple^^

          // attr.h structs: https://github.com/apple-oss-distributions/xnu/blob/main/bsd/sys/attr.h

          // file metadata categories that are filled
          attributes.bitmapcount = ATTR_BIT_MAP_COUNT;

          // common attributes for all files and dirs
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

          // if (open_status == -1)
          //   {
          //     std::cout << "cant open: " << *starting_path << std::endl;
          //     return;
          //   }

          uint64_t options = 0;
          int numatts;
          while ((numatts = getattrlistbulk(
                    file_current.descriptor, &attributes, &readinto[0], sizeof(readinto),
                    options)) > 0)
            {
              auto bufferstart = &readinto[0];
              char *value;
              uint32_t data_length;
              size_t ct;


              // loop through each returned getattrlistbulk item
              for (ct = 0; ct < numatts; ct++)
                {
                  crawl::global_file_counter++;

                  // entry length
                  char *cursor = bufferstart;
                  // std::cout << " NUMATTS: " << numatts << std::endl;

                  std::memcpy(&data_length, cursor, sizeof(data_length));
                  cursor += sizeof(data_length);

                  // returned attributes
                  attribute_set_t returned_attrs{};
                  std::memcpy(&returned_attrs, cursor, sizeof(returned_attrs));
                  cursor += sizeof(returned_attrs);

                  // ATTR_CMN_NAME -- this is the file name and its variable

                  std::string_view filename;
                  char *name_pointer = nullptr;
                  std::size_t filename_length{};
                  if (returned_attrs.commonattr & ATTR_CMN_NAME)
                    {
                      char *ref_location = cursor;
                      attrreference_t name_ref{};
                      std::memcpy(&name_ref, cursor, sizeof(name_ref));

                      cursor += sizeof(name_ref);
                      name_pointer = ref_location + name_ref.attr_dataoffset;

                      if (name_ref.attr_length > 0)
                        {
                          name_pointer[name_ref.attr_length - 1] = '\0';
                        }
                      filename = std::string_view(name_pointer, name_ref.attr_length - 1);
                      filename_length = name_ref.attr_length - 1;
                    }
                  crawl::global_file_list.push_back(name_pointer);

                  fsobj_type_t object_type = 0;

                  if (returned_attrs.commonattr & ATTR_CMN_OBJTYPE)
                    {
                      std::memcpy(&object_type, cursor, sizeof(object_type));
                      cursor += sizeof(object_type);
                    }


                  timespec crtime{}; //this is a struct not a single value

                  if (returned_attrs.commonattr & ATTR_CMN_CRTIME)
                    {
                      std::memcpy(
                        &crtime, cursor, sizeof(crtime)
                      );

                      cursor += sizeof(crtime);
                    }

                  uint32_t permissions = 0;
                  if (returned_attrs.commonattr & ATTR_CMN_ACCESSMASK)
                    {
                      std::memcpy(&permissions, cursor, sizeof(permissions));
                      cursor += sizeof(permissions);
                    }

                  // ATTR_CMN_FILEID

                  uint64_t file_id = 0;
                  if (returned_attrs.commonattr & ATTR_CMN_FILEID)
                    {
                      std::memcpy(
                        &file_id,
                        cursor,
                        sizeof(file_id)
                      );

                      cursor += sizeof(file_id);
                    }

                  uint64_t parent_id = 0;

                  if (returned_attrs.commonattr & ATTR_CMN_PARENTID)
                    {
                      std::memcpy(
                        &parent_id, cursor, sizeof(parent_id)
                      );
                      cursor += sizeof(parent_id);
                    }


                  off_t total_size = 0;

                  if (returned_attrs.fileattr & ATTR_FILE_TOTALSIZE)
                    {
                      std::memcpy(
                        &total_size,
                        cursor,
                        sizeof(total_size)
                      );

                      cursor += sizeof(total_size);
                    }


                  // std::cout << "\n  fcount: " << global_file_counter << std::endl;
                  // std::cout << " parent: " << file_current.new_path << "  file: " << filename << std::endl;

                  /*This is a new directory to add to directory vector */
                  if (object_type == VDIR)
                    {
                      int fd = openat(
                        file_current.descriptor, name_pointer, O_CLOEXEC | O_RDONLY | O_DIRECTORY);
                      if (fd != -1)
                        {
                          file_crawl_object new_obj{};
                          new_obj.descriptor = fd;

                          // std::cout << new_path  << std::endl;
                          append_to_create_new_file_path(
                            name_pointer, file_current.new_path, new_obj.new_path);
                          directory_vec.push_back(new_obj);
                        }
                    }

                  /* create file row here:


                  */

                  //makerec

                  FileRecord file_record{};
                  std::copy_n(
                    name_pointer,
                    std::min(std::strlen(name_pointer), file_record.file_name.size() - 1),
                    file_record.file_name.begin());

                    file_record.type = object_type;
                    file_record.created_timestamp =  crtime.tv_sec;
                    file_record.mode = permissions;
                    file_record.inode = file_id;
                    file_record.parent_inode = parent_id;
                    file_record.size_bytes = total_size;


                  //fill file record
                  // std::cout << "--here:::: " << file_record.to_string() << std::endl;
                  // std::cout << "sizeof(FileRecord) = " << sizeof(FileRecord) << "\n";
                  (void) emit(file_record);

                  // ----------------- Shift pointer tonext entry block -----------------
                  bufferstart += data_length;


                  /*
                   recursively crawl and write structs - OLD
                   */

                  // DIR* starting_directory_pointer = opendir(starting_path);
                  // struct dirent* directory_list;

                  // while (readdir(starting_directory_pointer) != nullptr) {
                  //     auto lst  = readdir(starting_directory_pointer);
                }
            }
          close(file_current.descriptor);
          // std::cout << "jump to new folder here" << std::endl;
        }
    }
  }


/* HIDING THIS MAIN FOR RUNNING WHOLE PROJECT




int main()
  {
    // std::filesystem::directory_entry starting("/Users/arielwhittingham/Downloads/demo/"); //test di
    // // std::filesystem::directory_entry starting(std::getenv("HOME"));
    // std::cout << starting.path() << std::endl;

    const char* str = "donwloads";
    const char* starting_path = "users/ariel";
    char new_path[4096];


    crawl::append_to_create_new_file_path(str, starting_path, new_path);
    std::cout <<  new_path << std::endl;


    crawl::crawl_filesystem("/Users/arielwhittingham/Documents/CodingNomads");
    std::cout << " file count " << crawl::global_file_counter  << std::endl;

    // std::cout << "Home: " <<  std::getenv("HOME") << std::endl;

    schema::FileRecord *random_record;
    // random_record->file_name = "Ariel";
    //
    // std::cout << random_record->file_name << "\n";
  }
*/
