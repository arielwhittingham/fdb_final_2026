#include <cstddef>
#include <string_view>
#pragma once


template <std::size_t N>
void convert_string_view_to_char_array(std::size_t array_size, std::string_view input_string);