#include "string_utils.h"
#include <iostream>
#include <string>
#include <array>


namespace utils
  {
  // turn string view into char array of fixed length
  // this is for writing to the char members of the file record struct


  template <std::size_t N>
  std::array<char, N> convert_string_view_to_char_array(std::size_t total_array_size, std::string_view input_string)
    {

      std::array<char, N> target_char_array{};
      std::size_t copy_length = std::min(input_string.length(), total_array_size - 1);
      std::cout << copy_length << "\n";
      std::copy_n(input_string.data(), copy_length, target_char_array.begin());
      std::fill(target_char_array + copy_length, target_char_array.end(), '\0');
      return target_char_array;
    }
  }
  int main()
    {
      const std::string_view input_string = "ariel";
      const auto a_size = 255;
      utils::convert_string_view_to_char_array<a_size>(a_size, input_string);
      return 0;

    }
