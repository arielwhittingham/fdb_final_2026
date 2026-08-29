#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>



int main() {

    // extern const int RECORDS_PER_PAGE = 10;
    // extern const int PAGE_SIZE_IN_BYTES = 4096;
    using page_id_type = std::uint32_t;
    std::string DB_FILE_LOCATION = "../data/" ;
    std::string DB_FILE_NAME = "databasefile.db" ;
    namespace fs = std::filesystem;
    fs::path dir = DB_FILE_LOCATION;
    std::fstream filestream;

    try {
        fs::create_directory(dir);
        fs::permissions(
            dir,
            fs::perms::owner_all
            );
    }
    catch (fs::filesystem_error& e) {
        std::cerr << "filesystem error, can't make directory" << e.what() << std::endl;
    }

    // create database file if not exists
    std::ofstream touch(DB_FILE_NAME, std::ios::binary);

    filestream.open("databasefile.db" , std::ios::binary | std::ios::in | std::ios::out);

    // filestream.seekg (0, std::ios::beg);
    char a[] = "ariel";
    filestream.write( reinterpret_cast<const char*>(&a), sizeof(a) );
    std::cout << filestream.is_open()  << std::endl;

    filestream.close();

    return 0;

}