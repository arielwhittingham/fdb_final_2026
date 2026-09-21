#include <array>
#include <vector>
#include <iostream>
# include <format>
#include "../utils/string_utils.h"
#include <string>
#include <string_view>
#include <stdexcept>


namespace query
  {
  /*
   Basic where clause parser. For the time being, the only part of the
   query that is parsed for inputs to the B+tree. So far there will only be one
   table of the all files and their chosed metadata
   */

  template<typename InputDataType>
  struct where_clause_range_object
    {
    const std::string &where_field;
    const InputDataType &start{};
    const InputDataType &end{};

    std::string to_string()
      {
        std::string ret = "\n where field = " + where_field
        + " start value = " + std::to_string(start)
        + " end value = " + std::to_string(end);
        return ret;
      }
    };

  std::vector<std::string> get_clause_tokens(std::string &input)
    {
      std::vector<std::string> result;
      std::size_t new_start = 0;
      std::size_t ind{0};
      for (; ind <= input.size(); ++ind)
        {
          if (!(input.begin() + ind == input.end()))
            {
              if (input[ind] == ' ')

                {
                  std::string slice(input.begin() + new_start, input.begin() + ind);
                  // std::cout << slice << std::endl;
                  new_start = ind + 1;
                  result.push_back(slice);
                }
            }
          else
            {
              std::string slice(input.begin() + new_start, input.end());
              result.push_back(slice);
              // std::cout << slice << std::endl;
            }
        }
      return result;
    }

  std::uint32_t substring_count(std::string &substring, std::string &instring)
    {
      std::size_t position{0};
      std::uint32_t occurence_count{0};

      while (instring.find(substring, position) != std::string::npos)
        {
          auto pos = instring.find(substring, position);
          occurence_count++;
          position = pos + substring.length();
        }
      return occurence_count;
    }


  const static std::string trim_leading(std::string where_clause)
    {
      const std::string all_chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

      size_t start_pos = where_clause.find_first_of(all_chars);
      std::cout << start_pos << std::endl;
      return where_clause.substr(start_pos, where_clause.size());
    }

  std::string get_where_clause(std::string query_text)
    {
      std::string sep("where");
      auto where_pos = query_text.find(sep);
      if (where_pos == std::string::npos)
        {
          throw std::runtime_error("where clause not found");
        }
      return trim_leading(query_text.substr(where_pos + sep.size()));
    }

  std::vector<std::string> primary_where_clause_parser(std::string &where_clause)
    {
      // everything after the where clause

      //Numeric ONLY
      std::vector<std::string> result_token_vector{};
      std::string wildcard{"%"};
      std::string equality{"="};
      std::string greater_than{">"};
      std::string less_than{"<"};
      std::string between{"between"};
      std::string and_{"and"};
      if (where_clause.empty())
        {
          throw std::runtime_error("empty where clause");
        }
      if (!(get_clause_tokens(where_clause).size() == 3 ||
            get_clause_tokens(where_clause).size() == 5))
        {
          std::string err =
              "Where clause too short, too long, or has too many "
              "conditions. Only one field "
              "condition is supported currently:\n"
              + where_clause;
          throw std::runtime_error(err.c_str());
        }

      //invalid conditions for more tokens thatn expected
      if (
        substring_count(equality, where_clause) > 1 ||
        substring_count(between, where_clause) > 1 ||
        substring_count(wildcard, where_clause) > 1 ||
        substring_count(greater_than, where_clause) +
        substring_count(less_than, where_clause) > 1 ||
        substring_count(and_, where_clause) > 1
      )

        {
          std::string err = "Too many conditions in where clause:\n" + where_clause;
          throw std::runtime_error(err.c_str());
        }
      // special cases of invalid
      if (where_clause.find(between) != std::string::npos && where_clause.find(and_) ==
          std::string::npos)
        {
          std::string err = "Incomplete between type where clause:\n" + where_clause;
          throw std::runtime_error(err.c_str());
        }
      if (where_clause.find(between) == std::string::npos && where_clause.find(and_) !=
          std::string::npos)
        {
          std::string err = "Incomplete between type where clause:\n" + where_clause;
          throw std::runtime_error(err.c_str());
        }

      if (where_clause.find(between) != std::string::npos)
        { // input range
          result_token_vector = get_clause_tokens(where_clause);
          result_token_vector.insert(result_token_vector.begin(), "range");
          /// expect result_token_vector.size() == 5
          return result_token_vector;
        }

      //no input range
      if (where_clause.find(greater_than) != std::string::npos ||
          where_clause.find(less_than) != std::string::npos)
        {
          // std::cout << "\n 2 " << std::endl;
          // has an < or >
          result_token_vector = get_clause_tokens(where_clause);
          result_token_vector.insert(result_token_vector.begin(), "inequality");
          /// expect result_token_vector.size() == 4
          return result_token_vector;
        }

      /// equality or prefix
      // expect = || = and %
      if (where_clause.find(wildcard) != std::string::npos && where_clause.find(equality) !=
          std::string::npos)
        {
          result_token_vector = get_clause_tokens(where_clause);
          result_token_vector.insert(result_token_vector.begin(), "prefix");
          return result_token_vector;
        }
      if (where_clause.find(equality) != std::string::npos)
        {
          {
            result_token_vector = get_clause_tokens(where_clause);
            result_token_vector.insert(result_token_vector.begin(), "exact");
            return result_token_vector;
          }
        }
      result_token_vector.push_back("invalid");
      return result_token_vector;
    }
  }

//
// int main()
//   {
//     // std::string s = "field > 45457665";
//     // std::string equality{"="};
//     // std::cout << query::substring_count(equality,s) << std::endl;
//     //
//     // auto x = query::primary_where_clause_parser(s);
//     //
//     // for (auto& val : x)
//     //   {
//     //     std::cout << val << std::endl;
//     //   }
//
//
//     std::string start {" where field > 45457665"};
//
//
//     std::cout << "\nlen star: "<< start.size()  << " clause: " << start << std::endl;
//
//     auto input = query::get_where_clause(start);
//
//     std::cout << "\nlen input: "<< input.size()  << " clause: " << input << std::endl;
//
//     auto trimmed = query::trim_leading_trailing(input);
//
//     std::cout << "\nlen trimmed: "<< trimmed.size()  << " clause: " << trimmed << std::endl;
//
//     return 1;
//   }
