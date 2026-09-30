#pragma once

#include <cstddef>
#include <iostream>
#include <string_view>
#include <array>
#include <tuple>

namespace utils
  {
  using CharString =  std::array<char,255>;

  inline CharString convert_string_view_to_char_array(std::string_view input_string)
    {
      CharString target_char_array{};
      static constexpr std::size_t total_array_size = 255;
      std::size_t copy_length = std::min(input_string.length(), total_array_size - 1);
      std::copy_n(input_string.data(), copy_length, target_char_array.begin());
      std::fill(target_char_array.begin() + copy_length, target_char_array.end(), '\0');
      return target_char_array;
    }

  // std::tuple<CharString> range_from_text_search_predicate(std::string_view input_string)
  //   {
  //     std::tuple<CharString> output;
  //
  //   }
  inline std::tuple<CharString, CharString> range_from_text_search_exact_match(std::string_view input_string)
    {
      CharString conv = convert_string_view_to_char_array(input_string);
      CharString conv_end =convert_string_view_to_char_array(input_string);
      conv_end[conv_end.size() - 1] += 1;
      return std::tuple<std::array<char, 255>, std::array<char, 255>>(conv, conv_end);

    }

  inline std::tuple<CharString, CharString> range_from_text_search_prefix(std::string_view input_string)
    {
      CharString conv =convert_string_view_to_char_array(input_string);
      CharString conv_end =convert_string_view_to_char_array(input_string);

      std::size_t filled{};
      for (std::size_t chr = 0; chr < conv.size(); ++chr)
        {

          if (conv.at(chr) == '\0')
            {
              filled = chr - 1;
              break;
            }
        }
      conv[filled] = '\0';
      conv_end[filled] = '\0';
      conv_end[filled - 1] += 1 ;
      return std::tuple<CharString, CharString>(conv, conv_end);


    }
  }