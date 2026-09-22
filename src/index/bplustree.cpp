// #include "include/fdb_final_2026/bplustree.h"
#include <iostream>
#include <array>
#include <vector>
#include "../schema/schema_objects.h"
#include "structs_bplustree.h"
#include "input_generator.cpp"
#include <string>
#include <cxxabi.h>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <variant>
#include <optional>
#include "../utils/string_utils.h"
#include "../query/query_utils.h"
#include <concepts>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>


namespace Index
  {
  namespace fs = std::filesystem;

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


  /*

💡 Core ConceptThink of std::lower_bound as finding the earliest position where
you can insert a value without breaking the
sorted order of the container.If the value is found in the range,
it returns an iterator pointing to the first
occurrence of that value.If the value is not found, it returns an iterator pointing to the next
larger element.

If all elements in the range are smaller than the given value, it returns the end() iterator.

*/

  //------------------------Record Management------------------------------------------------


  class BufferManager
    {
  public:
    fs::path dir = Struc::DB_FILE_LOCATION;
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
            std::cerr << "filesystem error, can't make directory" << e.what() <<
                std::endl;
            return;
          }
        // create database file if not exists
        if (fs::exists(Struc::DB_FILE_NAME))
          {
            fs::remove(Struc::DB_FILE_NAME);
            std::ofstream touch(Struc::DB_FILE_NAME, std::ios::binary);
          }
        else
          {
            std::ofstream touch(Struc::DB_FILE_NAME, std::ios::binary);

            // open file
            filestream.open(
              Struc::DB_FILE_NAME,
              std::ios::binary | std::ios::in | std::ios::out);
            if (filestream.is_open())
              {
                std::cout << "open" << "\n";
              }
            else
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


    uint32_t write_record_to_file(Struc::StorageRecord &record)
      {
        uint32_t new_record_id = this->create_new_record_id();
        this->records.push_back(new_record_id);
        filestream.open(
          Struc::DB_FILE_NAME,
          std::ios::binary | std::ios::in | std::ios::out);
        filestream.write(
          reinterpret_cast<const char *>(&record),
          sizeof(record));
        std::cout << "\n wrote page #: " << record.to_string() << "\n";
        std::cout << "\n recordcount /  new record created: " << new_record_id <<
            "\n";
        filestream.close();
        return new_record_id;
      }

    void read_from_file_to_memory(uint32_t page_number)
      {
        filestream.open(
          Struc::DB_FILE_LOCATION,
          std::ios::binary | std::ios::in | std::ios::out);
      }
    };


  //------------------------B+Tree Management------------------------------------------------

  /*

  */
  template<typename IndexDataType>
  // what data type is the index storing and ordering
  class BPlusTreeManager
    {
  public:
    using Node = Struc::BPlusTreeNode<IndexDataType>;
    using Record = Struc::IndexRecord<IndexDataType>;
    const std::string &starting_search_directory;
    static constexpr uint32_t overflow_record_cushion = 2;
    uint32_t root_id{};
    // static constexpr std::size_t ARRAY_SIZE = std::floor(
    //                                             Struc::PAGE_SIZE_IN_BYTES / sizeof(
    //                                               Record)) -
    //                                           overflow_record_cushion;
    std::vector<Node> tree_nodes;
    uint32_t node_count = 1;
    uint32_t record_count = 0;


    // --------constructor below---------

    BPlusTreeManager(const std::string &starting_dir_param)

    // colon is the separator of variables before any constructor logic
    // initializes the root node and then assigns it to the vector of nodes
      : starting_search_directory{starting_dir_param},
        tree_nodes{}
      {
        Node root_node{};
        root_node.page_number = 0;
        root_node.type = Struc::NodeType::Leaf;
        root_node.is_root = true;
        root_node.filled_slots = 0;
        root_node.parent_page_number = Struc::invalid_page_id;
        root_node.next_leaf_id = Struc::invalid_page_id;
        tree_nodes.push_back(root_node);
        this->root_id = root_node.page_number;
        std::cout << "\n ARRAY SIZE:: " << root_node.ARRAY_SIZE << std::endl;
      }

    void debug_whole_tree()
      {
        std::size_t node_index = 0;
        for (; node_index < tree_nodes.size(); node_index++)
          {
            std::cout << "\n Total Record Count: " << this->record_count << "\n";
            std::cout << " Node: " << node_index << " -- Values: " << tree_nodes[node_index].
                to_string() << "\n";
          }
      }

    Node &return_page_by_id(
      uint32_t page_number)
      {
        return this->tree_nodes.at(page_number);
      }


    std::size_t get_child_page_index(const Record &record,
                                     uint32_t search_node_id)
      {
        auto iterator = std::upper_bound(
          this->tree_nodes[search_node_id].records.begin(),
          this->tree_nodes[search_node_id].records.begin() + this->tree_nodes[search_node_id].
          filled_slots,
          record,
          Record::less_than_comparator
        );
        return static_cast<std::size_t>(std::distance(
          this->tree_nodes[search_node_id].records.begin(), iterator));
      }

    // func get_child
    std::size_t get_child_index_from_child_id(uint32_t child_id, uint32_t search_node_id)
    const
      {
        std::size_t x = 0;
        for (; x <= this->tree_nodes[search_node_id].filled_slots; x++)
          {
            if (this->tree_nodes[search_node_id].child_page_ids[x] == child_id)
              {
                return x;
              }
          }
        /* throw a runtime error instead of returning a bad key
         to prevent the error from occuring farther downstream */
        throw std::runtime_error(
          " \n This tree node: " + this->tree_nodes.at(search_node_id).to_string() +
          " with node id: " + std::to_string(search_node_id)
          + " does not contain child id: " + std::to_string(child_id));
      }

    // func create_new_page
    uint32_t create_new_page(Struc::NodeType node_type)
      {
        uint32_t new_page_id = this->node_count;
        if (node_type == Struc::NodeType::Leaf)
          {
            /* NEW LEAF NODE*/
            Node blank_leaf{
            .page_number = new_page_id,
            .type = Struc::NodeType::Leaf,
            .filled_slots = 0,
            .parent_page_number = Struc::invalid_page_id,
            .next_leaf_id = Struc::invalid_page_id
            };
            //ADD TO VECTOR
            this->tree_nodes.emplace_back(blank_leaf);
          }
        else // if internal or root
          {
            /* NEW INTERNAL NODE   */
            Node blank_leaf
            {
            .page_number = new_page_id,
            .type = Struc::NodeType::Internal,
            .filled_slots = 0,
            .parent_page_number = Struc::invalid_page_id,
            .next_leaf_id = Struc::invalid_page_id
            };
            //ADD TO VECTOR
            this->tree_nodes.emplace_back(blank_leaf);
          }

        this->node_count++;

        return new_page_id;
      }

    /*
create_new_page is overloaded since tin this special case
where we create a root, it has to result from a Split nd requires
a split output

     */
    void create_new_page(const Struc::SplitOutput<IndexDataType> &split_result)
      {
        uint32_t new_root_id = create_new_page(Struc::NodeType::Internal); {
          //create a separate scope
          Node &new_root_node = return_page_by_id(new_root_id);
          this->tree_nodes.at(new_root_id).type = Struc::NodeType::Internal;
          this->tree_nodes.at(new_root_id).is_root = true;
          this->tree_nodes.at(new_root_id).filled_slots = 1;
          this->tree_nodes.at(new_root_id).records[0] = split_result.separator_sent_up;
          this->tree_nodes.at(new_root_id).child_page_ids[0] = split_result.left_node_id_original;
          this->tree_nodes.at(new_root_id).child_page_ids[1] = split_result.right_node_id_new;
          this->tree_nodes.at(new_root_id).parent_page_number = Struc::invalid_page_id;
        } { ///updating nww left and right nodes to not be the root and
          // point to the new root
          Node &left_node = return_page_by_id(split_result.left_node_id_original);
          left_node.parent_page_number = new_root_id;
          left_node.is_root = false;
          Node &right_node = return_page_by_id(split_result.right_node_id_new);
          right_node.parent_page_number = new_root_id;
          right_node.is_root = false;
        }

        this->root_id = new_root_id;
      }

    // func insert_parent
    void insert_parent(std::size_t index, const Record &separator,
                       uint32_t new_child_id, uint32_t target_node_id)
      {
        auto filled_slots = this->tree_nodes[target_node_id].filled_slots;
        auto &node = this->tree_nodes[target_node_id];

        /* make room by shifting the vaues back and then inserting the new sep record */
        std::move_backward(
          node.records.begin() + index,
          node.records.begin() + filled_slots,
          node.records.begin() + filled_slots + 1);
        node.records.at(index) = separator;

        std::move_backward(
          node.child_page_ids.begin() + index + 1,
          node.child_page_ids.begin() + filled_slots + 1,
          node.child_page_ids.begin() + filled_slots + 2);
        node.child_page_ids.at(index + 1) = new_child_id;

        ++this->tree_nodes[target_node_id].filled_slots;
      }


    Struc::SplitOutput<IndexDataType> split_internal_node(uint32_t node_id,
                                                          std::size_t insertsize,
                                                          const Record separator,
                                                          uint32_t new_child_id)
      {
        constexpr std::size_t ARRAY_S = Node::ARRAY_SIZE;

        // 1. make buffers thatn canfit one more than their base
        // capacity
        std::array<Record, ARRAY_S + 1> records{};
        std::array<uint32_t, ARRAY_S + 2> child_page_id_array{};
        uint32_t parent_id = Struc::invalid_page_id; {
          const Node &node = return_page_by_id(node_id);
          parent_id = node.parent_page_number;

          //looper vars
          std::size_t a;
          std::size_t b;
          for (a = 0; a < ARRAY_S; ++a)
            {
              records[a] = node.records[a];
            }
          for (b = 0; b <= ARRAY_S; ++b)
            {
              child_page_id_array[b] = node.child_page_ids[b];
            }
        }

        // insertpairs ssame shape as insert_separator_and_child
        for (std::size_t i = ARRAY_S; i > insertsize; --i) records[i] = records[i - 1];
        records[insertsize] = separator;

        for (std::size_t i = ARRAY_S + 1; i > insertsize + 1; --i)
          child_page_id_array[i] = child_page_id_array[i - 1];
        child_page_id_array[insertsize + 1] = new_child_id;
        //  x+1 for keys, x+2 for the children array

        // findthe middle key — to push up
        constexpr std::size_t update_array_size = ARRAY_S + 1;
        constexpr std::size_t middle_index = update_array_size / 2;

        constexpr std::size_t left_keys = middle_index;
        constexpr std::size_t right_keys = update_array_size - middle_index - 1;

        Struc::IndexRecord<IndexDataType> promoted = records[middle_index];

        // allocate AFTER reading - needd to reaquire left node
        /// to avoid stale reference
        uint32_t new_node_id = create_new_page(Struc::NodeType::Internal);

        // left half keeps keys [0 - midleindex -1], children [0 - middle_ind]
        {
          Node &left_node = return_page_by_id(node_id);
          left_node.filled_slots = static_cast<uint16_t>(left_keys);

          //loopers
          std::size_t x;
          std::size_t y;
          for (x = 0; x < left_keys; ++x)
            {
              left_node.records[x] = records[x];
            }
          for (y = 0; y <= left_keys; ++y)
            {
              left_node.child_page_ids[y] = child_page_id_array[y];
            }
        }

        //right half keys = [midleindex+1 - end], children [middle_ind+1 - end]
        {
          Node &right_node = return_page_by_id(new_node_id);
          right_node.page_number = new_node_id;
          right_node.type = Struc::NodeType::Internal;
          right_node.is_root = false;
          right_node.next_leaf_id = Struc::invalid_page_id;
          right_node.parent_page_number = parent_id;
          right_node.filled_slots = static_cast<uint16_t>(right_keys);

          //loopers
          std::size_t x;
          std::size_t y;

          for (x = 0; x < right_keys; ++x)
            {
              right_node.records[x] = records[middle_index + 1 + x];
            }
          for (y = 0; y <= right_keys; ++y)
            {
              right_node.child_page_ids[y] = child_page_id_array[middle_index + 1 + y];
            }
        }

        //  looper
        std::size_t z;
        // update parent node on child records
        for (std::size_t z = 0; z <= right_keys; ++z)
          {
            return_page_by_id(child_page_id_array[middle_index + 1 + z]).parent_page_number =
                new_node_id;
          }

        // send the separatr up to parent node
        Struc::SplitOutput<IndexDataType> plit_internal_output{};
        plit_internal_output.left_node_id_original = node_id;
        plit_internal_output.right_node_id_new = new_node_id;
        plit_internal_output.separator_sent_up = promoted;
        return plit_internal_output;
      }

    // func split_node
    Struc::SplitOutput<IndexDataType> split_node(uint32_t update_node_id,
                                                 const Record &
                                                 overflow_rec)
      {
        std::size_t middle_index{};
        constexpr size_t added_slot{1};
        Record separator_record{};
        std::array<Record,
          Node::ARRAY_SIZE + added_slot> temp_array;
        auto &update_node = this->tree_nodes.at(update_node_id);
        uint32_t previous_next_node_id = update_node.next_leaf_id;
        uint32_t previous_parent_node_id = update_node.parent_page_number;
        // uint32_t previous_previous_node_id = update_node.previous_leaf_id;
        const auto &source_array = update_node.records;
        constexpr size_t update_array_size = Node::ARRAY_SIZE + 1;

        std::size_t insert_overflow_position = update_node.get_array_index_without_checking_exists(
          overflow_rec, update_node.records);

        std::copy_n(
          source_array.begin(),
          insert_overflow_position,
          temp_array.begin()
        );
        temp_array[insert_overflow_position] = overflow_rec;
        //copy rest of array into temp
        std::copy_n(
          source_array.begin() + insert_overflow_position,
          update_node.filled_slots - insert_overflow_position,
          temp_array.begin() + insert_overflow_position + 1
        );


        if (update_array_size % 2 == 0)
          {
            middle_index = update_array_size / 2;
          }
        else
          {
            middle_index = std::floor(update_array_size / 2);
          }
        separator_record = temp_array.at(middle_index);


        uint32_t new_right_leaf = create_new_page(Struc::NodeType::Leaf);

        auto &update_node_reaquire = this->tree_nodes.at(update_node_id);


        /* Split array here,update split_node with new array values
         and assign the other half to the  the new right node */

        std::array<Record, Struc::BPlusTreeNode<
          IndexDataType>::ARRAY_SIZE> left_new{};
        std::array<Record, Struc::BPlusTreeNode<
          IndexDataType>::ARRAY_SIZE> right_new{};


        std::copy(temp_array.begin(), temp_array.begin() + middle_index, left_new.begin());
        std::copy(temp_array.begin() + middle_index, temp_array.end(), right_new.begin());
        size_t new_right_filled = update_array_size - middle_index;
        std::copy_n(
          left_new.begin(), middle_index, this->tree_nodes[update_node_id].records.begin());

        this->tree_nodes[update_node_id].is_root = false;
        this->tree_nodes[update_node_id].filled_slots = middle_index;
        this->tree_nodes[update_node_id].next_leaf_id = new_right_leaf;

        this->tree_nodes[new_right_leaf].records = right_new;
        this->tree_nodes[new_right_leaf].filled_slots = new_right_filled;
        this->tree_nodes[new_right_leaf].parent_page_number = previous_parent_node_id;
        this->tree_nodes[new_right_leaf].next_leaf_id = previous_next_node_id;
        // this->tree_nodes[new_right_leaf].previous_leaf_id = update_node_id;

        Struc::SplitOutput<IndexDataType> split_return
        {
        .left_node_id_original = update_node_reaquire.page_number,
        .right_node_id_new = new_right_leaf,
        .separator_sent_up = separator_record
        };

        return split_return;
      }


    /* when there is no parent means we are the root*/
    // if (update_node.parent_page_number == Struc::invalid_page_id)
    //   {
    //     //create new root
    //     uint32_t new_root_id = create_new_page(Struc::NodeType::Internal);
    //     this->root_id = new_root_id;
    //   }
    /*   */


    // func find_leaf
    uint32_t find_leaf_from_record_val(const Record &record)
      {
        uint32_t current_id = this->root_id;
        while (return_page_by_id(current_id).type != Struc::NodeType::Leaf)
          {
            auto &node = return_page_by_id(current_id);
            current_id = node.child_page_ids[get_child_page_index(record, current_id)];
          }
        return current_id;
      }

    // func find_leaf
    Record create_search_record_from_field_value(const IndexDataType &value)
      {
        return Struc::IndexRecord<IndexDataType>{0, value};
      }

    uint32_t get_leaf_from_search_value(IndexDataType &searchvalue)
      {
        Record resulting_record = create_search_record_from_field_value(searchvalue);
        auto starter_node_id = find_leaf_from_record_val(resulting_record);
        while (starter_node_id != Struc::invalid_page_id)
          {
            auto &starter_leaf = return_page_by_id(starter_node_id);
            auto end = starter_leaf.records.begin() + starter_leaf.filled_slots;

            // first record that is greater than or equal to resulting_record
            auto lb = lower_bound(
              starter_leaf.records.begin(), end, resulting_record, Record::less_than_comparator);
            if (lb != end)
              { // found the right leaf
                return starter_node_id;
              }
            /// doesnt find leaf, scan to next node
            starter_node_id = starter_leaf.next_leaf_id;
          }

        return Struc::invalid_page_id;
      }


    // func add_record
    void add_record(const Record &record)
      {
        constexpr std::size_t array_size = Node::ARRAY_SIZE;

        uint32_t leaf_id = find_leaf_from_record_val(record);

        // room in the LEAF NODE?
        {
          auto &leaf = return_page_by_id(leaf_id);
          auto end = leaf.records.begin() + leaf.filled_slots;

          auto it = std::lower_bound(
            leaf.records.begin(), end, record,
            Record::less_than_comparator);

          // if (it != end && !Record::less_than_comparator(record, *it))
          //   throw std::runtime_error("duplicate key: " + record.to_string());

          /*
           Increment Record Count by 1
           */
          ++this->record_count;
          if (leaf.filled_slots < array_size)
            {
              leaf.add_to_array(
                record, static_cast<std::size_t>(std::distance(leaf.records.begin(), it)));
              leaf.set_min_max_for_node();
              return;
            }
        }


        auto split_output = split_node(leaf_id, record);


        while (true)
        /* keep splitting up from leaf intil top is reached*/
          {
            uint32_t left_node_id = split_output.left_node_id_original;
            uint32_t right_node_id = split_output.right_node_id_new;
            auto &left_node = return_page_by_id(left_node_id);

            uint32_t parent_id = left_node.parent_page_number;

            if (parent_id == Struc::invalid_page_id) /* this is an invalid result--return*/
              {
                create_new_page(split_output);
                return;
              }
            return_page_by_id(right_node_id).parent_page_number = parent_id;

            std::size_t childindex = get_child_index_from_child_id(left_node_id, parent_id);

            if (return_page_by_id(parent_id).filled_slots < array_size)
              {
                insert_parent(childindex, split_output.separator_sent_up, right_node_id, parent_id);
                return;
              }
            split_output = split_internal_node
            (
              parent_id,
              childindex,
              split_output.separator_sent_up,
              right_node_id
            );
            // std::cerr << "\n Did we make it to the split internal record?\n";
          }
      } // add record func


    static query::where_clause_range_object<IndexDataType> process_where_clause(
      const std::string &query_text)
      {
        std::string start_range_string{""};
        std::string end_range_string{""};
        std::uint32_t start_range_uint32_t = 0;
        std::uint32_t end_range_uint32_t = 0;
        std::cout << query_text << std::endl;
        std::string where_clause = query::get_where_clause(query_text);
        std::vector<std::string> tokens{};
        try
          {
            tokens = query::primary_where_clause_parser(where_clause);
            for (auto &i: tokens)
              {
                std::cout << i << std::endl;
              }
          } catch (const std::runtime_error &e)
          {
            std::cout << "Error parsing where clause tokens: " << e.what() << std::endl;
          }


        /*
         Value types need to be converted and looked at eventually since
         some are int and some are str. Once that happens this function can return
         for each search, the field it is filtering on and the range(all search values
         have a range) using functions in string_utils.h

         */


        if (tokens.size() == 6)
          { // between | int only
            if constexpr (__is_same(IndexDataType, uint32_t))
              {
                if (tokens[0] == "range")
                  {
                    std::string &field_name = tokens[1];
                    start_range_uint32_t = static_cast<uint32_t>(std::stoul(tokens[3]));
                    end_range_uint32_t = static_cast<uint32_t>(std::stoul(tokens[5]));

                    std::cout <<  "\n start and end of range below: " << std::endl;

                    {
                      for (auto & x : tokens) std::cout << x << std::endl;
                    }

                    std::cout << start_range_uint32_t  << std::endl;
                    std::cout << end_range_uint32_t  << std::endl;
                    std::cout << "\n "  << std::endl;


                    query::where_clause_range_object<IndexDataType> rng = {
                    .where_field = field_name,
                    .start = start_range_uint32_t,
                    .end = end_range_uint32_t
                    };
                    return rng;
                  }
              }
          }
        if (tokens.size() == 4)
          {
            // equality, prefix, inequality | int or str
            if constexpr (__is_same(IndexDataType, uint32_t))
              { //get range for these search types when field is int
                if (tokens[0] == "exact")
                  {
                    std::string &field_name = tokens[1];
                    uint32_t start_range_uint32_t = static_cast<uint32_t>(std::stoul(tokens[3]));
                    uint32_t end_range_uint32_t = start_range_uint32_t + 1;
                    query::where_clause_range_object<IndexDataType> rng = {
                      .where_field = field_name,
                      .start = start_range_uint32_t,
                      .end = end_range_uint32_t
                      };
                    return rng;
                  }
                if (tokens[0] == "inequality")
                  {
                    std::string &field_name = tokens[1];
                    uint32_t input_val = static_cast<uint32_t>(std::stoul(tokens[3]));
                    uint32_t start_range{};
                    uint32_t end_range{};
                    if (tokens[2] == "<")
                      {
                        start_range_uint32_t = 0;
                        end_range_uint32_t = input_val - 1;
                      }
                    else
                      {
                        start_range_uint32_t = input_val + 1;
                        end_range_uint32_t = UINT32_MAX;
                      }

                    query::where_clause_range_object<IndexDataType> rng = {
                      .where_field = field_name,
                      .start = start_range_uint32_t,
                      .end = end_range_uint32_t
                      };
                    return rng;
                  }
              }
            if constexpr (__is_same(IndexDataType, utils::CharString))
              {
                if (tokens[0] == "exact")
                  {
                    std::string &field_name = tokens[1];
                    std::tuple<utils::CharString, utils::CharString> range_generated =
                        utils::range_from_text_search_exact_match(tokens[3]);
                    query::where_clause_range_object<IndexDataType> rng = {
                      .where_field = field_name,
                      .start = std::get<0>(range_generated),
                      .end = std::get<1>(range_generated)
                      };

                    return rng;
                  }
                if (tokens[0] == "prefix")
                  {
                    std::string &field_name = tokens[1];
                    std::tuple<utils::CharString, utils::CharString> range_generated =
                        utils::range_from_text_search_prefix(tokens[3]);
                    query::where_clause_range_object<IndexDataType> rng = {
                      .where_field = field_name,
                      .start = std::get<0>(range_generated),
                      .end = std::get<1>(range_generated)
                      };
                    return rng;
                  }
              }
          }
        std::string err = "Can't process where clause: " + where_clause;
        throw std::runtime_error (err);
      }




  // void return_range_from_value;

  /*std::vector<uint32_t>*/
  void return_matching_record_ids(IndexDataType &search_record)
    {
        /* TM:
         * 1. Get the range search record ID return logic working
         * 2. Finalize Crawler
         */
      std::cout << "Take the search record sd" << std::endl;

    }

  // void verify_node
  }; // class
} //namespace


//func main
int main()
  {
    // logic for inserting into a vector and maintaining sorted order
    std::vector<int> ff{4};
    // std::array< int32_t, 5>
    ff.push_back(6);
    ff.push_back(7);
    ff.push_back(9);
    int tester{5};

    //
    std::array<uint32_t, 14> left_original{
    16, 21, 30, 41, 50, 62, 72, 89, 90, 101, 110, 120, 130, 135
    };
    std::array<uint32_t, 14> left_new{};
    std::array<uint32_t, 14> right_new{};

    query::where_clause_range_object<std::uint32_t> x = Index::BPlusTreeManager<std::uint32_t>::process_where_clause(
      "select * from tbl where filename < 100");


    std::cout << x.to_string()  << std::endl;

    /*

    Index::BPlusTreeManager<Struc::FileNameDataType> bptree_2("test");

    auto test_vals = test_inputs::get_sample_file_name_records();

    for (int i = 0; i < test_vals.size(); i++)
      {
        //loop for each sample record
        if (i > 0)
          {
            auto x = test_vals[i];
            int numcol = 0;
            auto col_1_original = std::get<0>(x);
            auto col_2_original = std::get<1>(x);
            std::stringstream ss(col_1_original);
            ss >> numcol;

            // std::cout << numcol <<  col_2_original << std::endl;
            std::array<char, 255> name_array{};
            std::size_t data_len = col_2_original.copy(name_array.data(), 254);
            name_array[data_len] = '\0';

            Struc::IndexRecord<Struc::FileNameDataType> test_record{
            static_cast<uint32_t>(numcol), name_array
            };
            bptree_2.add_record(test_record);
          } //END loop for each sample record
      }
    // const std::string_view search_val =  "gga_a_very.csv";
    // bptree_2.debug_whole_tree();
    std::cout << "DONE" << std::endl;
    std::array<char, 255> conv = utils::convert_string_view_to_char_array<255>(
      255, "a_a_crystalline.txt");
    auto real_dummy_record = Struc::IndexRecord<Struc::FileNameDataType>{59, "a_a_crystalline.txt"};
    auto real = bptree_2.find_leaf_from_record_val(real_dummy_record);
    auto res = bptree_2.get_leaf_from_search_value(conv);
    bptree_2.debug_whole_tree();
    std::cout << "based on new logic leaf node number for   a_a_crystalline.txt: " << res << std::endl;
    std::cout << "Real for descender, diff than the searcher because this needs to"
                 "say where this record should be added in the data.  a_a_crystalline.txt: " << real << std::endl;
*/
    return 0;
  }


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
