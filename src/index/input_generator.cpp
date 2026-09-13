#include <iostream>
#include <array>
#include <vector>
#include <chrono>
#include "../schema/schema_objects.h"
#include "structs_bplustree.h"


namespace test_inputs

  {

  // std::vector<Struc::FileNameDataType> name_return_vector(uint32_t size )
  void name_return_vector(uint32_t size )
    {
      auto start = std::chrono::steady_clock::now();
      std::vector<Struc::FileNameDataType> returnvec;
      for (size_t x=0 ; x < size ; x++ )
        {
          auto pause = std::chrono::steady_clock::now();
          std::chrono::duration<double, std::chrono::nanoseconds::period> duration = pause - start;
          // Struc::FileNameDataType rec;
          auto ns = duration.count();

          // pseudo random number
          std::cout << static_cast<int>(ns) % 200  << std::endl;


        }


    }





  }

int main()
  {
    // std::vector<Struc::FileNameDataType> ret = test_inputs::name_return_vector(200);
    test_inputs::name_return_vector(200);
    return 0;
  }