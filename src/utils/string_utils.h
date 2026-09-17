#pragma once
#include <cstddef>
#include <string_view>

namespace utils
  {
  template<std::size_t N>
  std::array<char, N> convert_string_view_to_char_array(std::size_t total_array_size,
                                                        std::string_view input_string)
    {
      std::array<char, N> target_char_array{};
      std::size_t copy_length = std::min(input_string.length(), total_array_size - 1);
      std::copy_n(input_string.data(), copy_length, target_char_array.begin());
      std::fill(target_char_array.begin() + copy_length, target_char_array.end(), '\0');
      return target_char_array;
    }
  }
