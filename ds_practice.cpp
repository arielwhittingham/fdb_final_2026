#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

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

int main() {

    // logic for inserting into a vector and maintaining sorted order
    std::vector<int> ff{4};
    ff.push_back(6);
    ff.push_back(7);
    ff.push_back(9);
    std::cout << ff[3] << "\n";
    int tester{5};

    auto indval = std::lower_bound(ff.begin(),ff.end(),tester);
    auto ind = std::distance(ff.begin(), indval);
    std::cout << "sindex of " << tester << " = " << ind << "\n";
    ff.insert(indval,tester);

    for (auto& c : ff) {
        std::cout << c << "\n";
    }

    return 0;

}