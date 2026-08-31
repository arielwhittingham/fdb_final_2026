// #include "include/fdb_final_2026/bplustree.h"
#include "iostream"
#include "array"
#include "vector"
#include "../schema/schema_objects.h"
#include <string>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <filesystem>


namespace indexer {
    extern const int PAGE_SIZE_IN_BYTES = 4096;
    // using page_id_type = std::uint32_t;
    std::string DB_FILE_LOCATION = "../data/" ;
    std::string DB_FILE_NAME = "../data/databasefile.db" ;
    // std::string DB_FILE_NAME = "databasefile.db" ;
    namespace fs = std::filesystem;


    // this is a sentinel commonly used to represent and invalid ID
    uint32_t invalid_page_id = UINT32_MAX;
    /*

    💡 Core ConceptThink of std::lower_bound as finding the earliest position where
    you can insert a value without breaking the
    sorted order of the container.If the value is found in the range,
    it returns an iterator pointing to the first
    occurrence of that value.If the value is not found, it returns an iterator pointing to the next
    larger element.

    If all elements in the range are smaller than the given value, it returns the end() iterator.

    */

    /*
    Remaining steps in b+tree:

    1. Finish ordered vector for storing by ordered key for both
    the key vector and the data vector(use the same index)
    2. Make function to do basic traversal based on keys
    3. Adding new values when there is existing room
    4. Traverse linked list laterally between nodes
    5. Removing values when there is no structural side effect
    6. Function to decide when adding new value will result in a break in rules such that the base needs to grow
    7.

    */


    template<std::size_t N>
    bool element_exists(uint32_t search_value, std::array<uint32_t, N> &search_array) {
        uint32_t return_index = 0;
        for (const auto &item: search_array) {
            if (item == search_value) {
                return true;
            }
            return_index++;
        }
        return false;
    }


    // return correct index of array to put it in sorted
    // only if it doesn't already exist
    template<std::size_t N>
    uint32_t get_array_index_without_checking_exists(uint32_t search_value, const std::array<uint32_t, N> &keys) {
        if constexpr (N == 0) {
            return 0;
        } else if constexpr (N == 1) {
            if (keys[0] > search_value) {
                return 1;
            } else {
                return 0;
            }
        } else {
            auto index_iterator = std::lower_bound(keys.begin(), keys.end(), search_value);

            auto index_value = std::distance(keys.begin(), index_iterator);

            std::cout << " search_value test: " << search_value << "\n";

            return index_value;
        }
    }

    //------------------------Record Management------------------------------------------------

        struct StorageRecord {
        uint32_t record_id;
        schema::FileRecord row;

    };
        class BufferManager {

        public:
            fs::path dir = DB_FILE_LOCATION;
            std::fstream filestream;
            uint32_t record_count = 0;
            std::vector<uint32_t> records;

            BufferManager() {
                //create dir if not exist
                try {
                    fs::create_directory(dir);
                    fs::permissions(
                        dir,
                        fs::perms::owner_all
                        );

                }
                catch (fs::filesystem_error& e) {
                    std::cerr << "filesystem error, can't make directory" << e.what() << std::endl;
                    return;
                }
                // create database file if not exists
                if (fs::exists(DB_FILE_NAME)) {
                    fs::remove(DB_FILE_NAME);
                    std::ofstream touch(DB_FILE_NAME, std::ios::binary);

                } else {

                    std::ofstream touch(DB_FILE_NAME, std::ios::binary);

                    // open file
                    filestream.open(DB_FILE_NAME, std::ios::binary | std::ios::in | std::ios::out);
                    if (filestream.is_open()) {
                        std::cout << "open" << "\n";
                    }
                    else {
                        std::cout << "CANT open" << "\n";
                    }

                    filestream.close();
                }
            }

            uint32_t create_new_record_id() {
                this->record_count++;
                return this->record_count;
            }


            uint32_t write_record_to_file(schema::FileRecord &record) {

                uint32_t new_record_id = this->create_new_record_id();
                this->records.push_back(new_record_id);
                filestream.open(DB_FILE_NAME, std::ios::binary | std::ios::in | std::ios::out);
                filestream.write( reinterpret_cast<const char*>(&record), sizeof(record) );
                std::cout << "\n wrote page #: " << record.to_string() << "\n";
                std::cout << "\n recordcount /  new record created: " << new_record_id << "\n";
                filestream.close();
                return new_record_id;

            }
            void read_from_file_to_memory(uint32_t page_number) {
                filestream.open(DB_FILE_LOCATION, std::ios::binary | std::ios::in | std::ios::out);

            }

        };

//------------------------B+Tree Management------------------------------------------------

    // this object is going to serve as the
    // container for any record I am indexing on in
    // my b+tree. This will include the immutable record_id from the
    // records DB and the field_value(of any type) that I am indexing on.
    template<typename T>
    struct IndexRecord {
        uint32_t record_id;
        T field_value;
    };

    struct TreeRootHeader {
        uint32_t root_id;
        uint32_t node_count;
    };

    struct NodeHeader {
        uint32_t page_number;
        uint8_t type; // leaf or internal
        uint16_t item_count;
        uint32_t parent_page_number;
    };

    struct BPlusTreeNode {
        NodeHeader header;
        uint32_t next_leaf_id;
        IndexRecord<typename T> records[std::floor(sizeof(T) / PAGE_SIZE_IN_BYTES )]; // calculate node size

        BPlusTreeNode() {
            // constructor
            std::cout << "this is the B+ tree constructor\n";
        }
    };

    //create a structure to store each new page
    // struct MemoryPage {
    //     char page_slot[PAGE_SIZE_IN_BYTES];
    //
    //     MemoryPage() {
    //         std::memset(page_slot, 0, PAGE_SIZE_IN_BYTES);
    //     }
    // };

        /*

        */
        class BPlusTreeManager {
        public:
            const std::string &starting_search_directory;
            TreeRootHeader root_header;
            BPlusTreeNode root_node;


            // --------costructor below---------

            BPlusTreeManager(const std::string &starting_dir_param)

            // colon is the separator of variables before any constructor logic
                : starting_search_directory{starting_dir_param},
                  root_header{0, 1} {
                root_node.header.type = 1; //leaf
                root_node.header.page_number = 0; //first node
                root_node.header.item_count = 0;
                root_node.header.parent_page_number = invalid_page_id; // invlaid noparent for root
                root_node.next_leaf_id = invalid_page_id; // invlaid no next leaf
            }

            uint32_t create_new_page_id() {
                uint32_t new_page_id = root_header.node_count;
                root_header.node_count++;
                return new_page_id;
            }

            void add_record(const schema::FileRecord &record) {
                /// first condition the empty tree has no
                /// records in it yet
                if (root_node.header.item_count == 0) {
                    root_node.records[0] = record;
                    root_node.header.item_count++;
                } else {
                    std::cout << " rest of logic for adding a new record to a non-empty tree. The"
                            "above code basically is just to initialize a tree with noting in it, and add "
                            "the very first record " << "\n";
                }
            }
        };
    }

    int main() {
        // logic for inserting into a vector and maintaining sorted order
        std::vector<int> ff{4};
        // std::array< int32_t, 5>
        ff.push_back(6);
        ff.push_back(7);
        ff.push_back(9);
        std::cout << ff[3] << "\n";
        int tester{5};

        std::array<uint32_t, 5> test{1, 2, 3, 4, 5};

        int a = indexer::element_exists(4, test);
        int d = indexer::get_array_index_without_checking_exists(4, test);
        std::cout << "exists: " << a << "index: " << d << "\n";


        schema::FileRecord testrecord_1{
            .inode = 1,
            .parent_inode = 5,
            .created_timestamp = 12345,
            .size_bytes = 4,
            .mode = 321,
            .type{"txt"},
            .file_name = {"file_1.txt"}
        };

        schema::FileRecord testrecord_2{
            .inode = 2,
            .parent_inode = 1,
            .created_timestamp = 548722,
            .size_bytes = 8,
            .mode = 322,
            .type{"sql"},
            .file_name = {"query_file.sql"}
        };

       // indexer::MemoryPage p;
       //  p.page_slot = {1,2,3,4,5};


        indexer::BPlusTreeManager testtree{"test_dir"};
        testtree.add_record(testrecord_1);

        indexer::BufferManager bufftest;

        auto rcount = bufftest.write_to_file(testrecord_2);

        std::cout << "\n records in \n" << rcount << "\n";
        std::cout << "\n TEST RECORD 1: \n" << testrecord_1.to_string() << "\n";

        /*

        std::cout << d << std::endl;


        auto indval = std::lower_bound(ff.begin(), ff.end(), tester);
        auto ind = std::distance(ff.begin(), indval);
        std::cout << "sindex of " << tester << " = " << ind << "\n";
        ff.insert(indval, tester);

        for (auto &c: ff) {
            std::cout << c << "\n";
        }

        return 0;
        */
    }
