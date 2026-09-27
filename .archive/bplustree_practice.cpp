#include <iostream>
#include <string>
#include <vector>

namespace practice {


    /*
    1. Start with the b+tree data structure and constructor
    2. Node/keys data structure (first simple with integers as data)
    3. Ordering page
    4. Lnked list for connectning leaves
    5. .... splitting and adding logic

    */


    struct RowData {
        int pk;
        std::string name;
    };

    struct Node {

        bool is_leaf;
        //how do we order this vector always?
        int records;
        std::vector<int> row_pks;
        std::vector<RowData> rows;

        void add_row(RowData row) {
            int index = row.pk;
            if ( records == 0) {
                rows.push_back(row);
                row_pks.push_back(index);
            }
            else {
                //continue insert logic here!

            }

        }
    };

    class BPlusTree {



    };

}