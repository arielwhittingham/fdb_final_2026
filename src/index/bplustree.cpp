// #include "include/fdb_final_2026/bplustree.h"
#include "iostream"
#include "array"
#include "vector"
#inluude "schema_objects.h"
#include <string>
#include <cstdint>

#include <algorithm>

namespace indexer {
    extern const int SORTED_KEYS_LENGTH = 5;
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
                // This now works perfectly
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

    class BPlusTreeNode {

        bool leaf_node;
        uint16_t keys_in_node;
        uint32_t next_node_id;


        uint32_t

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

    int a  = indexer::element_exists(4,test);
    int d = indexer::get_array_index_without_checking_exists(4, test);
    std::cout << "exists: " << a <<"index: " << d << "\n";


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
