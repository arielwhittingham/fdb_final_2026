#pragma once

#include <iostream>
#include <array>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>
#include "../schema/schema_objects.h"


namespace Struc
  {
  //------------------------Constants------------------------------------------------
  enum class NodeType
    {
    Leaf =0,
    Internal = 1
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

#pragma pack(push, 1)
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
#pragma pack(pop)

  //------------------------B+Tree Management------------------------------------------------

  // this object is going to serve as the
  // container for any record I am indexing on in
  // my b+tree. This will include the immutable record_id from the
  // records DB and the field_value(of any type) that I am indexing on.
// struc index_record
  template<typename IndexDataType>
  // what data type is the index storing and ordering
  struct IndexRecord
    {
    uint32_t record_id;
    /*This field_value for string values needs to either be intialized with {}
      or when I copy into this struct, i have to do
      memcopy(field_value, from_text.date(), from_text.size())
      where from_text is a string_view */
    IndexDataType field_value{};

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
            " record_id: " +
            std::to_string(record_id) +
            " valaue: " +
            field_value.data();
      }
    };


  struct TreeRootHeader
    {
    uint32_t root_id;
    uint32_t node_count;
    };
  // struc SplitOutput
  template<typename IndexDataType>
  struct SplitOutput
    {
    uint32_t left_node_id_original;
    uint32_t right_node_id_new;
    IndexRecord<IndexDataType> separator_sent_up;

    std::string to_string()
      {
        return
            "\n Split output node id: " + std::to_string(left_node_id_original)
            + "\n right new node id: " + std::to_string(right_node_id_new)
            + "\n separator record: " + separator_sent_up.to_string();
      }
    };

// struc BPlusTreeNode
  template<typename IndexDataType>
  // what data type is the index storing and ordering
  struct BPlusTreeNode
    {
    uint32_t page_number;
    NodeType type;
    uint16_t filled_slots = 0;
    uint32_t parent_page_number = invalid_page_id;
    uint32_t next_leaf_id = invalid_page_id;
    IndexRecord<IndexDataType> min;
    IndexRecord<IndexDataType> max;

    static constexpr std::size_t record_size_bytes = sizeof(IndexRecord<IndexDataType>);
    bool is_root = false;
    static constexpr std::size_t overhead =
    sizeof(page_number) +
    sizeof(parent_page_number) +
    sizeof(type) +
    sizeof(filled_slots) +
    sizeof(next_leaf_id) +
    sizeof(is_root);
    static constexpr std::size_t ARRAY_SIZE =(PAGE_SIZE_IN_BYTES - overhead - sizeof(uint32_t))/(sizeof(uint32_t) + record_size_bytes);

    std::array<IndexRecord<IndexDataType>, ARRAY_SIZE> records{};
    std::array<uint32_t, ARRAY_SIZE + 1> child_page_ids{};



    /*Don't need a constructor yet*/
    // BPlusTreeNode()
    //   {
    //   }

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


        std::size_t index_value = static_cast<std::size_t>(
          std::distance(keys.begin(), index_iterator)
        );

        // std::cout << " search_value test: " << search_value.to_string() << "\n";

        if (index_value == 0)
          {
            return 0;
          }
        else return index_value;
      }

    // always inrcement the record count when adding a new record
    void add_to_array
    (
      const IndexRecord<IndexDataType> &record, std::size_t index_at
    )
    // try
    //   {
      {

        std::move_backward(
          this->records.begin() + index_at,
          this->records.begin() + filled_slots,
          this->records.begin() + filled_slots + 1
        );

        this->records.at(index_at) = record;

        ++this->filled_slots;
        // std::cout << "\n Filled: " << filled_slots << std::endl;
        // std::cout << " Index At: " << records.at(index_at).to_string() << std::endl;
        // std::cout << "\n record being added: " << record.to_string() << std::endl;
      }

    // }
    // catch (...)

    void add_record_to_node(const IndexRecord<IndexDataType> &record)
      {
        std::size_t ind = get_array_index_without_checking_exists(
          record, this->records);

        add_to_array(record, ind);
      }
    void set_min_max_for_node()
      {
        this->min = this-> records[0];
        this->max = this-> records[filled_slots -1];
      }

    std::string to_string() const
      {
        std::string array_res = "\n ";
        std::string separator = " \n ";
        for (std::size_t i = 0; i < filled_slots;)
          {
            array_res += records[i].to_string() + separator;
            i++;
          }
        char node_type_string = type == NodeType::Internal ? 'I' : 'L';


        return
            "\n page num: " + std::to_string(page_number) +
            "\n Node Size in Bytes: " + std::to_string(sizeof(this)) +
            "\n array size: " + std::to_string(ARRAY_SIZE) +
            "\n record size in bytes: " + std::to_string(record_size_bytes) +
            "\n node type: " + node_type_string +
            "\n is root: " + std::to_string(is_root) +
            "\n filled: " + std::to_string(filled_slots) +
            "\n parent:" + std::to_string(parent_page_number) +
            "\n next leaf: " + std::to_string(next_leaf_id) +
              // "\n previous leaf: " + std::to_string(previous_leaf_id) +
            "\n records: " + array_res;
      }
    };


  /*

  */
  }
