#pragma once

#include "iostream"
#include "array"
#include "vector"
#include "../schema/schema_objects.h"


namespace TreeStructs
    {
    //------------------------Constants------------------------------------------------
    enum class NodeType
        {
        Leaf,
        Internal
        };

    extern const int PAGE_SIZE_IN_BYTES = 4096;
    // using page_id_type = std::uint32_t;
    std::string DB_FILE_LOCATION = "../data/";
    std::string DB_FILE_NAME = "../data/databasefile.db";
    // std::string DB_FILE_NAME = "databasefile.db"
    // this is a sentinel commonly used to represent and invalid ID
    uint32_t invalid_page_id = UINT32_MAX;
    using FileNameDataType = std::array<char, 255>;


    //------------------------Record Management------------------------------------------------

    struct StorageRecord
        {
        uint32_t record_id;
        schema::FileRecord row;

        std::string to_string() const
            {
            return
                    "\n record_id: " +
                    std::to_string(record_id) +
                    "\n valaue: " +
                    row.to_string();
            }
        };

    //------------------------B+Tree Management------------------------------------------------

    // this object is going to serve as the
    // container for any record I am indexing on in
    // my b+tree. This will include the immutable record_id from the
    // records DB and the field_value(of any type) that I am indexing on.

    template<typename IndexDataType> // what data type is the index storing and ordering
    struct IndexRecord
        {
        uint32_t record_id;
        IndexDataType field_value;

        static bool less_than_comparator(
            const IndexRecord &lhs,
            const IndexRecord &rhs)
            {
            if (lhs.field_value != rhs.field_value)
                {
                return lhs.field_value < rhs.field_value;
                } else
                {
                return lhs.record_id < rhs.record_id;
                }
            }

        std::string to_string() const
            {
            return
                    "\n record_id: " +
                    std::to_string(record_id) +
                    "\n valaue: " +
                    field_value.data();
            }
        };


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
    }
