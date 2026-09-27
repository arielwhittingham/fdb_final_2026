#include <array>
#include <cstddef>
#include <iostream>
#include <ostream>
#include "string_utils.h"


/*
 Testing the string incrementing logic for both a prefix type search and
 and exact value text.
 */

int main()
  {
    //exat val
    const auto &res = utils::range_from_text_search_exact_match("Ari");

    const auto & sec =  std::get<1>(res).data();
    const auto & fir =  std::get<0>(res).data();
    bool x =  sec == fir;
    std::cout << x  << std::endl;
    for (size_t i = 0; i < std::get<0>(res).size(); ++i)
      {
        const bool y = sec[i] == fir[i];
        std::cout << y << std::endl;

      }


    //prefix
    const auto &res1 = utils::range_from_text_search_prefix("Ariel");

    const auto & sec1 =  std::get<1>(res1).data();
    const auto & fir1 =  std::get<0>(res1).data();
    bool x1 =  sec1 == fir1;
    std::cout << x1  << std::endl;
    for (size_t i1 = 0; i1 < std::get<0>(res1).size(); ++i1)
      {
        const bool y1 = sec1[i1] == fir1[i1];
        std::cout  << sec1[i1] << " " << fir1[i1] << " " << y1 << std::endl;

      }

  }
