// #include "include/fdb_final_2026/bplustree.h"
#include <iostream>
#include <array>
#include <vector>
#include "../schema/schema_objects.h"
#include "structs_bplustree.h"
#include <string>
#include <cxxabi.h>
#include <cstdint>
#include <algorithm>
#include <fstream>
#include <filesystem>


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
    const std::string &starting_search_directory;
    uint32_t root_id{};
    static constexpr std::size_t ARRAY_SIZE = std::floor(
                                                Struc::PAGE_SIZE_IN_BYTES / sizeof(
                                                  Struc::IndexRecord<IndexDataType>)) - 1;
    std::vector<Struc::BPlusTreeNode<IndexDataType> > tree_nodes;
    uint32_t node_count = 1;


    // --------constructor below---------

    BPlusTreeManager(const std::string &starting_dir_param)

    // colon is the separator of variables before any constructor logic
    // initializes the root node and then assigns it to the vector of nodes
      : starting_search_directory{starting_dir_param},
        tree_nodes{}
      {
        Struc::BPlusTreeNode<IndexDataType> root_node{};
        root_node.page_number = 0;
        root_node.type = Struc::NodeType::Leaf;
        root_node.is_root = true;
        root_node.filled_slots = 0;
        root_node.parent_page_number = Struc::invalid_page_id;
        root_node.next_leaf_id = Struc::invalid_page_id;
        tree_nodes.push_back(root_node);
        this->root_id = root_node.page_number;
      }


    uint32_t create_new_page(Struc::NodeType node_type)
      {
        uint32_t new_page_id = this->node_count;
        if (node_type == Struc::NodeType::Leaf)
          {
            /* NEW LEAF NODE    */
            Struc::BPlusTreeNode<IndexDataType> blank_leaf{
            .page_number = new_page_id,
            .type = Struc::NodeType::Leaf,
            .filled_slots = 0,
            .parent_page_number = Struc::invalid_page_id,
            .next_leaf_id = Struc::invalid_page_id
            };
            //ADD TO VECTOR
            this->tree_nodes.emplace_back(blank_leaf);
          }
        else
          {
            /* NEW INTERNAL NODE   */
            Struc::BPlusTreeNode<IndexDataType> blank_leaf
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

    //--------------------------------method 1. add_record() --------------------------------

    Struc::BPlusTreeNode<IndexDataType> &return_page_by_id(
      uint32_t page_number)
      {
        return this->tree_nodes.at(page_number);
      }


    Struc::SplitLeafOutput<IndexDataType> split_node(Struc::BPlusTreeNode<IndexDataType>
      &update_node)
      {
        std::size_t middle_index;
        Struc::IndexRecord<IndexDataType> separator_record{};


        if (update_node.filled_slots % 2 == 0)
          {
            middle_index = update_node.filled_slots / 2;
          }
        else
          {
            middle_index = std::floor(update_node.filled_slots / 2);
          }
        std::cout << middle_index << std::endl;
        separator_record = update_node.records.at(middle_index);

        /* create new blank leaf node and return split output*/
        uint32_t new_right_leaf = create_new_page(Struc::NodeType::Leaf);

        /* Split array here,update split_node with new array values
         and assign the other half to the  the new right node */

        std::array<Struc::IndexRecord<IndexDataType>,update_node.ARRAY_SIZE>  left_new{};
        std::array<Struc::IndexRecord<IndexDataType>, update_node.ARRAY_SIZE>  right_new{};

        std::copy(update_node.begin(), update_node.begin() + middle_index, left_new.begin());
        std::copy(update_node.begin() + middle_index, update_node.end(), right_new.begin());


        update_node.records = left_new;
        update_node.is_root = false;
        update_node.filled_slots = left_new.size();
        update_node.parent_page_number  = Struc::invalid_page_id;
        update_node.next_leaf_id = Struc::invalid_page_id;

        this->tree_nodes[new_right_leaf].records = right_new;

        Struc::SplitLeafOutput<IndexDataType> split_return
        {
        .left_node_id_original = update_node.page_number,
        .right_node_id_new = new_right_leaf,
        .separator_sent_up = separator_record
        };
        return split_return;

        /* when there is no parent means we are the root*/
        // if (update_node.parent_page_number == Struc::invalid_page_id)
        //   {
        //     //create new root
        //     uint32_t new_root_id = create_new_page(Struc::NodeType::Internal);
        //     this->root_id = new_root_id;
        //   }
        /*   */
      }


    void add_record(const Struc::IndexRecord<IndexDataType> &record)
      {
        // Struc::SplitLeafOutput<IndexDataType> split_output;
        if (this->node_count == 1) /* root node ONLY logic */
          {
            Struc::BPlusTreeNode<IndexDataType> &update_node = return_page_by_id(this->root_id);

            // if (update_node.filled_slots < update_node.ARRAY_SIZE)
            /*
             hardcode smaller array size to test. When done, uncomment line above
             */
            if (update_node.filled_slots < 4)
            //size is good
              { // safe to add to array
                update_node.add_record_to_node(record);
                return;
              }
            else /* NEED To SPLIT NODE here and as a result make 2 new nodes,
              if we are on the root node */
              {
                update_node.add_record_to_node(record); /*contains overflow record */
                // split_output = split_node(update_node);


              }
            // std::cout << split_output.to_string() << std::endl;



          }
        /* More than one node */
        /*CONTINUE by SEARCHING LEAF here, since this
        This block comes after the root node block
        */
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

    int a = Index::element_exists(4, test);

    std::cout << "exists: " << a << "\n";


    schema::FileRecord test_file_record_1{
    .inode = 1,
    .parent_inode = 5,
    .created_timestamp = 12345,
    .size_bytes = 4,
    .mode = 321,
    .type{"txt"},
    .file_name = {"file_1.txt"}
    };

    schema::FileRecord test_file_record_2{
    .inode = 3,
    .parent_inode = 1,
    .created_timestamp = 548722,
    .size_bytes = 8,
    .mode = 322,
    .type{"txt"},
    .file_name = {"macbook_pro.txt"}
    };

    schema::FileRecord test_file_record_3{
    .inode = 2,
    .parent_inode = 1,
    .created_timestamp = 548722,
    .size_bytes = 8,
    .mode = 322,
    .type{"sql"},
    .file_name = {"query_file.sql"}
    };


    schema::FileRecord test_file_record_4{
    .inode = 4,
    .parent_inode = 1,
    .created_timestamp = 548722,
    .size_bytes = 8,
    .mode = 322,
    .type{"sql"},
    .file_name = {"ariel_query_file.sql"}
    };


    schema::FileRecord test_file_record_5{
    .inode = 5,
    .parent_inode = 1,
    .created_timestamp = 548722,
    .size_bytes = 8,
    .mode = 322,
    .type{"sql"},
    .file_name = {"zriel_query_file.sql"}
    };


    std::cout << "1 HERE" << std::endl;

    Struc::IndexRecord test_record_1{1, test_file_record_1.file_name};
    Struc::IndexRecord test_record_2{2, test_file_record_2.file_name};
    Struc::IndexRecord test_record_3{3, test_file_record_3.file_name};
    Struc::IndexRecord test_record_4{4, test_file_record_4.file_name};
    Struc::IndexRecord test_record_5{5, test_file_record_5.file_name};
    //
    // std::cout << test_record_1.to_string() << std::endl;
    // std::cout << test_record_2.to_string() << std::endl;
    // std::cout << test_record_3.to_string() << std::endl;
    // std::cout << test_record_4.to_string() << std::endl;


    // Struc::BPlusTreeNode<Struc::FileNameDataType> test_node;
    // test_node.page_number = 1;
    // test_node.type = Struc::NodeType::Leaf;
    // test_node.parent_page_number = Struc::invalid_page_id;
    // test_node.next_leaf_id = Struc::invalid_page_id;
    //
    //
    //
    // test_node.add_record_to_node(test_record_1);
    // test_node.add_record_to_node(test_record_2);
    // test_node.add_record_to_node(test_record_3);
    // test_node.add_record_to_node(test_record_4);
    // test_node.add_record_to_node(test_record_5);


    std::cout <<
        " \n\n\n\n\n\n\n -----------printing out test node below------------" <<
        std::endl;


    Index::BPlusTreeManager<Struc::FileNameDataType> bptree("test");
    bptree.add_record(test_record_1);
    bptree.add_record(test_record_2);
    bptree.add_record(test_record_3);
    bptree.add_record(test_record_4);
    bptree.add_record(test_record_5);


    for (auto &x :  bptree.tree_nodes)
      {
        std::cout << x.to_string() << "\n";
      }




    // std::cout << bptree.tree_nodes[0].to_string() << "\n";

    auto& test_array = bptree.tree_nodes[0].records;




    std::array<uint32_t, 14> left_original{16,21,30,41,50,62,72,89,90,101,110,120,130,135};
    std::array<uint32_t, 14>  left_new{};
    std::array<uint32_t, 14>  right_new{};

    std::copy(left_original.begin(), left_original.begin() + 7, left_new.begin());

    // auto x = abi::__cxa_demangle(typeid(split_iterator).name(), nullptr, nullptr, nullptr);

    for (auto it : left_new)
      {
        std::cout << it << std::endl;
      }






    // int d = Index::get_array_index_without_checking_exists(test_record_1);
    // Index::MemoryPage p;
    //  p.page_slot = {1,2,3,4,5};


    // Index::BPlusTreeManager testtree{"test_dir"};
    // testtree.add_record(testrecord_1);

    Index::BufferManager bufftest;

    // auto rcount = bufftest.write_record_to_file();

    // std::cout << "\n records in \n" << rcount << "\n";
    // std::cout << "\n TEST RECORD 1: \n" << testrecord_1.to_string() << "\n";

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
