#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <string>
#include <stdexcept>
#include "../schema/schema_objects.h"


namespace Struc
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
    std::string TEST_STARTING_DIRECTORY = "/Users/arielwhittingham/prod_workspace/";
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

    template<typename IndexDataType>
    // what data type is the index storing and ordering
    struct IndexRecord
        {
        uint32_t record_id;
        IndexDataType field_value;

        static bool less_than_comparator
        (
            const IndexRecord &lhs,
            const IndexRecord &rhs
        )
            {
            if (lhs.field_value != rhs.field_value)
                {
                return lhs.field_value < rhs.field_value;
                }
            else
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

    template<typename IndexDataType>
    struct SplitLeafOutput
        {
        uint32_t left_node_id_original;
        uint32_t right_node_id_new;
        IndexRecord<IndexDataType> separator_sent_up;
        };


    template<typename IndexDataType>
    // what data type is the index storing and ordering
    struct BPlusTreeNode
        {
        uint32_t page_number;
        NodeType type;
        uint16_t filled_slots;
        uint32_t parent_page_number;
        uint32_t next_leaf_id;

        static constexpr std::size_t ARRAY_SIZE =
                4096 / sizeof(IndexRecord<IndexDataType>);
        std::array<IndexRecord<IndexDataType>, ARRAY_SIZE> records{};
        bool is_root = false;

        // calculate node size

        BPlusTreeNode()
            : filled_slots(0)
            {
            // constructor
            std::cout << "this is the B+ Node tree constructor\n" << "\n \n" <<
                    std::endl;
            }

        template<std::size_t N>
        std::size_t get_array_index_without_checking_exists
        (
            const IndexRecord<IndexDataType> &search_value,
            const std::array<IndexRecord<IndexDataType>, N> &keys
        )
            {
            auto index_iterator = std::lower_bound(
                keys.begin(),
                keys.begin() + filled_slots,
                search_value,
                IndexRecord<IndexDataType>::less_than_comparator
            );

            auto index_value = std::distance(keys.begin(), index_iterator);

            std::cout << " search_value test: " << search_value.to_string() << "\n";

            return index_value;
            }

        // always inrcement the record count when adding a new record
        void add_to_array
        (
            const IndexRecord<IndexDataType> &record, std::size_t index_at
        )
            {
            std::move_backward(records.begin() + index_at,
                               records.begin() + filled_slots,
                               records.begin() + filled_slots + 1
            );
            records[index_at] = record;
            filled_slots++;
            }

        void add_record_to_node(const IndexRecord<IndexDataType>& record)
            {
            std::size_t ind = get_array_index_without_checking_exists(
                record, records);
            add_to_array(record, ind);
            }

        std::string to_string()
            {
            std::string array_res = "\n ";
            std::string separator = " \n ";
            for (std::size_t i = 0; i < filled_slots;)
                {
                array_res += records[i].to_string() + separator;
                i++;
                }


            return
                    "\n page num: " + std::to_string(page_number) +
                    "\n array size: " + std::to_string(ARRAY_SIZE) +
                    "\n is root: " + std::to_string(is_root) +
                    "\n filled: " + std::to_string(filled_slots) +
                    "\n parent:" + std::to_string(parent_page_number) +
                    "\n next leaf: " + std::to_string(next_leaf_id) +
                    "\n records: " + array_res;
            }
        };


    /*

    */
    }
