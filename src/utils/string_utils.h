#pragma once
#include <cstddef>
#include <iostream>
#include <string_view>
#include <array>
#include <tuple>

namespace utils
  {

  std::array<char, 255> convert_string_view_to_char_array(
                                                        std::string_view input_string)
    {
      std::array<char, 255> target_char_array{};
      static constexpr std::size_t total_array_size = 255;
      std::size_t copy_length = std::min(input_string.length(), total_array_size - 1);
      std::copy_n(input_string.data(), copy_length, target_char_array.begin());
      std::fill(target_char_array.begin() + copy_length, target_char_array.end(), '\0');
      return target_char_array;
    }

  // std::tuple<std::array<char,255>> range_from_text_search_predicate(std::string_view input_string)
  //   {
  //     std::tuple<std::array<char,255>> output;
  //
  //   }
  std::tuple<std::array<char, 255>, std::array<char, 255>> range_from_text_search_exact_match(std::string_view input_string)
    {std::tuple<std::array<char,255>, std::array<char,255>>
      std::array<char,255> conv =convert_string_view_to_char_array(input_string);
      std::array<char,255> conv_end =convert_string_view_to_char_array(input_string);
      conv_end[conv_end.size() - 1] += 1;
      return std::tuple<std::array<char, 255>, std::array<char, 255>>(conv, conv_end);

    }

  std::tuple<std::array<char, 255>, std::array<char, 255>> range_from_text_search_prefix(std::string_view input_string)
    {
      std::array<char,255> conv =convert_string_view_to_char_array(input_string);
      std::array<char,255> conv_end =convert_string_view_to_char_array(input_string);
      std::size_t filled{};
      for (std::size_t chr = 0; chr < conv.size(); ++chr)
        {
          if (conv.at(chr) == '\0')
            {
              filled = chr;
              break;
            }
        }
      conv_end[filled] += 1;
      return std::tuple<std::array<char,255>, std::array<char,255>>(conv, conv_end);


    }
  }


int main()
  {
    const auto &res = utils::range_from_text_search_exact_match("Ari");
    std::cout << std::get<0>(res).data()  << std::endl;
    // std::cout << std::get<1>(res).data()  << std::endl;

  }