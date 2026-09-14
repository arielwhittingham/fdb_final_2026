#include <iostream>
#include <array>
#include <vector>
#include <chrono>
#include "../schema/schema_objects.h"
#include "structs_bplustree.h"
#include <filesystem>
#include <fstream>

//
// namespace test_inputs
//
//   {
//
//     std::string TEST_FILE_NAME = "/Users/arielwhittingham/Downloads/test_data_fdb_1.csv";
//     std::ifstream input_file(TEST_FILE_NAME, std::ios::in);
//
//   // std::vector<Struc::FileNameDataType> name_return_vector(uint32_t size )
//   void name_return_vector(uint32_t size )
//     {
//       // auto start = std::chrono::steady_clock::now();
//       std::vector<Struc::FileNameDataType> returnvec;
//       // for (size_t x=0 ; x < size ; x++ )
//         {
//           // auto pause = std::chrono::steady_clock::now();
//           // std::chrono::duration<double, std::chrono::nanoseconds::period> duration = pause - start;
//           // // Struc::FileNameDataType rec;
//           // auto ns = duration.count();
//           //
//           // // pseudo random number
//           // std::cout << static_cast<int>(ns) % 200  << std::endl;
//
//
//         }
//
//
//     }
//
//
//
//
//
//   }

int main()
  {
    std::vector<std::string> ret;
    // test_inputs::name_return_vector(200);
    std::string TEST_FILE_NAME = "/Users/arielwhittingham/prod_workspace/fdb_final_2026/test_data_fdb_1.csv";
    std::ifstream input_file;

    std::string line;
    input_file.open(TEST_FILE_NAME, std::ios::in);
    while (std::getline(input_file, line))
      {
        std::stringstream ss(line);
        ret.push_back(ss.str());
        std::cout << ss.str()  << std::endl;



      }


    return 0;
  }
