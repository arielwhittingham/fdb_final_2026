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


namespace indexer
    {
    extern const int PAGE_SIZE_IN_BYTES = 4096;
    // using page_id_type = std::uint32_t;
    std::string DB_FILE_LOCATION = "../data/";
    std::string DB_FILE_NAME = "../data/databasefile.db";

    // std::string DB_FILE_NAME = "databasefile.db" ;
    enum class NodeType
        {
        Leaf,
        Internal
        };

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

    template<typename IndexDataType> // what data type is the index storing and ordering
    struct IndexRecord
        {
        uint32_t record_id;
        IndexDataType field_value;

        static bool less_than_comparator(
            const IndexRecord &lhs,
            const IndexRecord &rhs)
            {
            if (lhs.field_value  != rhs.field_value)
                {
                return lhs.field_value < rhs.field_value;
                }
            else
                {
                return lhs.record_id < rhs.record_id;
                }
            }
        };

    //-----------------HELPER Non-Member Methods ----------------

    template<std::size_t N>
    bool element_exists(uint32_t search_value, std::array<uint32_t, N> &search_array)
        {
        uint32_t return_index = 0;
        for (const auto &item: search_array)
            {
            if (item == search_value)
                {
                return true;
                }
            return_index++;
            }
        return false;
        }


    // return correct index of array to put it in sorted
    // only if it doesn't already exist
    template<std::size_t N, typename IndexDataType>
    std::size_t get_array_index_without_checking_exists(IndexRecord<IndexDataType> &search_value,
                                                        const std::array<IndexDataType, N> &keys,
                                                        std::size_t filled_slots)
        {
        auto index_iterator = std::lower_bound(
            keys.begin(),
            keys.end() + filled_slots,
            search_value,
            IndexRecord<IndexDataType>::less_than_comparator
            );

        auto index_value = std::distance(keys.begin(), index_iterator);

        std::cout << " search_value test: " << search_value << "\n";

        return index_value;
        }


    //------------------------Record Management------------------------------------------------

    struct StorageRecord
        {
        uint32_t record_id;
        schema::FileRecord row;
        };

    class BufferManager
        {
    public:
        fs::path dir = DB_FILE_LOCATION;
        std::fstream filestream;
        uint32_t record_count = 0;
        std::vector<uint32_t> records;

        BufferManager()
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
                std::cerr << "filesystem error, can't make directory" << e.what() << std::endl;
                return;
                }
            // create database file if not exists
            if (fs::exists(DB_FILE_NAME))
                {
                fs::remove(DB_FILE_NAME);
                std::ofstream touch(DB_FILE_NAME, std::ios::binary);
                } else
                {
                std::ofstream touch(DB_FILE_NAME, std::ios::binary);

                // open file
                filestream.open(DB_FILE_NAME, std::ios::binary | std::ios::in | std::ios::out);
                if (filestream.is_open())
                    {
                    std::cout << "open" << "\n";
                    } else
                    {
                    std::cout << "CANT open" << "\n";
                    }

                filestream.close();
                }
            }

        uint32_t create_new_record_id()
            {
            this->record_count++;
            return this->record_count;
            }


        uint32_t write_record_to_file(StorageRecord &record)
            {
            uint32_t new_record_id = this->create_new_record_id();
            this->records.push_back(new_record_id);
            filestream.open(DB_FILE_NAME, std::ios::binary | std::ios::in | std::ios::out);
            filestream.write(reinterpret_cast<const char *>(&record), sizeof(record));
            std::cout << "\n wrote page #: " << record.to_string() << "\n";
            std::cout << "\n recordcount /  new record created: " << new_record_id << "\n";
            filestream.close();
            return new_record_id;
            }

        void read_from_file_to_memory(uint32_t page_number)
            {
            filestream.open(DB_FILE_LOCATION, std::ios::binary | std::ios::in | std::ios::out);
            }
        };

    //------------------------B+Tree Management------------------------------------------------

    // this object is going to serve as the
    // container for any record I am indexing on in
    // my b+tree. This will include the immutable record_id from the
    // records DB and the field_value(of any type) that I am indexing on.

    struct TreeRootHeader
        {
        uint32_t root_id;
        uint32_t node_count;
        };


    template<typename IndexDataType> // what data type is the index storing and ordering
    struct BPlusTreeNode
        {
        uint32_t page_number;
        NodeType type;
        static constexpr std::size_t ARRAY_SIZE = 4096 / sizeof(IndexRecord<IndexDataType>);
        bool is_root = false;
        uint16_t filled_slots;
        uint32_t parent_page_number;
        uint32_t next_leaf_id;
        std::array<IndexRecord<IndexDataType>, ARRAY_SIZE> records;

        // calculate node size

        BPlusTreeNode()
            : filled_slots(0)
            {
            // constructor
            std::cout << "this is the B+ Node tree constructor\n" << "\n \n" << std::endl;
            }
        // always inrcement the record count when adding a new record
        void add_to_array(IndexRecord<IndexDataType> &record, std::size_t index_at)
            {
            records.insert(records.begin(), index_at, record);
            filled_slots++;

            }
        };

    /*

    */
    template<typename IndexDataType> // what data type is the index storing and ordering
    class BPlusTreeManager
        {
    public:
        const std::string &starting_search_directory;
        BPlusTreeNode<IndexDataType> root_node;
        static constexpr std::size_t ARRAY_SIZE = std::floor(PAGE_SIZE_IN_BYTES / sizeof(IndexRecord<IndexDataType>));
        std::vector<BPlusTreeNode<IndexDataType> > tree_nodes;
        uint32_t node_count = 1;


        // --------constructor below---------

        BPlusTreeManager(const std::string &starting_dir_param)

        // colon is the separator of variables before any constructor logic
        // initializes the root node and then assigns it to the vector of nodes
            : starting_search_directory{starting_dir_param},
              root_node{
              .page_number = 0,
              .type = NodeType::Leaf, // leaf or internal
              .item_count = 0,
              .is_root = true,
              .parent_page_number = invalid_page_id,
              .next_leaf_id = invalid_page_id
              },
              tree_nodes{
              root_node,
              }
            {
            //empty constructr
            }

        // uint32_t create_new_page_id()
        //     {
        //     uint32_t new_page_id = root_header.node_count;
        //     root_header.node_count++;
        //     return new_page_id;
        //     }

        //--------------------------------method 1. add_record() --------------------------------

        void add_record(const IndexRecord<IndexDataType> &record, const BPlusTreeNode<IndexDataType> &update_node)
            {
            /// first condition the empty tree has no
            /// records in it yet

            if (this->root_node.header.item_count == 0) //special first case make empty page with 0 records
                {
                this->root_node.records[0] = record;
                this->root_node.header.item_count = 1;
                }

            // start logic for adding record
            if (update_node->type == NodeType::Leaf) //if leaf
                {
                if (update_node->filled_slots < update_node.ARRAY_SIZE) //size is good
                    { // safe to add to array
                    // std::size_t get_array_index_without_checking_exists(IndexRecord<IndexDataType> &search_value, const std::array<IndexDataType, N> &keys)
                    std::size_t insert_index = get_array_index_without_checking_exists(record, update_node->records);
                    std::cout << "insert_index: " << insert_index << "\n";
                    }
                }
            }
        }; //class end
    } //namspace end

int main()
    {
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

    indexer::IndexRecord<char>

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
