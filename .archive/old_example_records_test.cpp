
/*
std::vector<int> ff{4};
    // std::array< int32_t, 5>
    ff.push_back(6);
    ff.push_back(7);
    ff.push_back(9);
    int tester{5};

    std::array<uint32_t, 5> test{1, 2, 3, 4, 5};

    int a = Index::element_exists(4, test);


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
    .inode = 20,
    .parent_inode = 1,
    .created_timestamp = 548722,
    .size_bytes = 8,
    .mode = 322,
    .type{"sql"},
    .file_name = {"query_file.sql"}
    };


    schema::FileRecord test_file_record_4{
    .inode = 7,
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

    schema::FileRecord test_file_record_6{
    .inode = 41,
    .parent_inode = 1,
    .created_timestamp = 548721,
    .size_bytes = 8,
    .mode = 321,
    .type{"sql"},
    .file_name = {"zquil.sql"}
    };

    schema::FileRecord test_file_record_7{
    .inode = 41,
    .parent_inode = 1,
    .created_timestamp = 548721,
    .size_bytes = 8,
    .mode = 321,
    .type{"sql"},
    .file_name = {"po.sql"}
    };

    schema::FileRecord test_file_record_8{
    .inode = 46,
    .parent_inode = 1,
    .created_timestamp = 548721,
    .size_bytes = 8,
    .mode = 321,
    .type{"sql"},
    .file_name = {"ag.sql"}
    };


    schema::FileRecord test_file_record_9{
    .inode = 14,
    .parent_inode = 1,
    .created_timestamp = 548721,
    .size_bytes = 8,
    .mode = 321,
    .type{"sql"},
    .file_name = {"ggzquil.sql"}
    };


    schema::FileRecord test_file_record_10{
    .inode = 12,
    .parent_inode = 1,
    .created_timestamp = 548721,
    .size_bytes = 8,
    .mode = 321,
    .type{"sql"},
    .file_name = {"1zquil.sql"}
    };


    Struc::IndexRecord test_record_1{1, test_file_record_1.file_name};
    Struc::IndexRecord test_record_2{2, test_file_record_2.file_name};
    Struc::IndexRecord test_record_3{3, test_file_record_3.file_name};
    Struc::IndexRecord test_record_4{4, test_file_record_4.file_name};
    Struc::IndexRecord test_record_5{5, test_file_record_5.file_name};
    Struc::IndexRecord test_record_6{6, test_file_record_6.file_name};
    Struc::IndexRecord test_record_7{7, test_file_record_7.file_name};
    Struc::IndexRecord test_record_8{8, test_file_record_8.file_name};
    Struc::IndexRecord test_record_9{9, test_file_record_9.file_name};
    Struc::IndexRecord test_record_10{10, test_file_record_10.file_name};

*/

    // Struc::BPlusTreeNode<Struc::FileNameDataType> test_node;
/* ORIGINAL TESTER CODE
    std::cout <<
        " \n\n\n\n\n\n\n -----------printing out test node below------------" <<
        std::endl;

    Index::BPlusTreeManager<Struc::FileNameDataType> bptree("test");
    bptree.add_record(test_record_1);
    bptree.debug_whole_tree();
    std::cout << "\n record 1 added\n" << std::endl;
    bptree.add_record(test_record_2);
    bptree.debug_whole_tree();
    std::cout << "\n record 2 added\n" << std::endl;
    bptree.add_record(test_record_3);
    bptree.debug_whole_tree();
    std::cout << "\n record 3 added\n" << std::endl;
    bptree.add_record(test_record_4);
    bptree.debug_whole_tree();
    std::cout << "\n record 4 added\n" << std::endl;
    bptree.add_record(test_record_5);
    bptree.debug_whole_tree();
    std::cout << "\n record 5 added\n" << std::endl;
    bptree.add_record(test_record_6);
    bptree.debug_whole_tree();
    std::cout << "\n record 6 added\n" << std::endl;
    bptree.add_record(test_record_7);
    bptree.debug_whole_tree();
    std::cout << "\n record 7 added\n" << std::endl;
    bptree.add_record(test_record_8);
    bptree.debug_whole_tree();
    std::cout << "\n record 8 added\n" << std::endl;
    bptree.add_record(test_record_9);
    bptree.debug_whole_tree();
    std::cout << "\n record 9 added\n" << std::endl;
    bptree.add_record(test_record_10);
    bptree.debug_whole_tree();
    std::cout << "\n record 10 added\n" << std::endl;

*/