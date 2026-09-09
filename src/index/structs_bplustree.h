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
            " is this running? "
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

  template<typename IndexDataType>
  struct SplitLeafOutput
    {
    uint32_t left_node_id_original;
    uint32_t right_node_id_new;
    IndexRecord<IndexDataType> separator_sent_up;

    std::string to_string()
      {
        return
            "\n Split output node id: " + std::to_string(left_node_id_original)
            + "\n right new node id: " + std::to_string(right_node_id_new)
            + "\n separator record: " + separator_sent_up.to_string()
        ;
      }
    };


  template<typename IndexDataType>
  // what data type is the index storing and ordering
  struct BPlusTreeNode
    {
    uint32_t page_number;
    NodeType type;
    uint16_t filled_slots = 0;
    uint32_t parent_page_number;
    uint32_t next_leaf_id;

    // static constexpr std::size_t ARRAY_SIZE =
    //     4096 / sizeof(IndexRecord<IndexDataType>);
    static constexpr std::size_t ARRAY_SIZE = 4;
    std::array<IndexRecord<IndexDataType>, ARRAY_SIZE> records{};
    bool is_root = false;

    // calculate node size
    /*Don't need a constructor yet*/
    // BPlusTreeNode()
    //   {
    //     // constructor
    //
    //     std::cout << "this is the B+ Node tree constructor\n" << "\n \n" <<
    //         std::endl;
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

        std::cout << " weird index first: " << index_value << std::endl;

        // std::cout << " search_value test: " << search_value.to_string() << "\n";

        if (index_value == 0)
          { return 0;}
        else return index_value - 1;
      }

    // always inrcement the record count when adding a new record
    void add_to_array
    (
      const IndexRecord<IndexDataType> &record, std::size_t index_at
    )
    // try
    //   {
      {
        std::cout << "index checker did it make it? " << std::endl;

        std::move_backward(
          this->records.begin() + index_at,
          this->records.begin() + filled_slots,
          this->records.begin() + filled_slots + 1
        );
        std::cout << "index checker did it make it? " << std::endl;
        std::cout << "what is the inex here: " << index_at << std::endl;


        this->records.at(index_at) = record;
        std::cout << " WE GONNA GET HERE? : " << index_at << std::endl;
        ++this->filled_slots;
        std::cout << "\n Filled: " << filled_slots << std::endl;
        // std::cout << " Index At: " << records.at(index_at).to_string() << std::endl;
        std::cout << "\n record being added: " << record.to_string() << std::endl;
      }

    // }
    // catch (...)

    void add_record_to_node(const IndexRecord<IndexDataType> &record)
      {
        std::size_t ind = get_array_index_without_checking_exists(
          record, this->records);
        std::cout << "WEIRD INDEX: " << ind << std::endl;

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
