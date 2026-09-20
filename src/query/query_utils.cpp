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
  struct ParsedWhereClause
    {
    std::string &where_field;
    InputDataType &start{};
    InputDataType &end{};
    };

  std::vector<std::string> get_clause_tokens(std::string input)
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
                  std::cout << slice << std::endl;
                  new_start = ind + 1;
                  result.push_back(slice);
                }
            }
          else
            {
              std::string slice(input.begin() + new_start, input.end());
              result.push_back(slice);
              std::cout << slice << std::endl;
            }
        }
      return result;
    }

  std::vector<std::string> primary_where_clause_parser(std::string &where_clause)
    {
      // everything after the where clause

      //Numeric ONLY
      std::vector<std::string> result_token_vector{};
      // std::string example1 = "FIELD BETWEEN val_1 AND val_2";
      // std::string example2 = "FIELD > val ....... FIELD < val";
      // //Numeric AND Text
      // std::string example3 = "FIELD = val";
      // //text only
      // std::string example4 = "FIELD = val*";
      if (where_clause.empty())
        {
          throw std::runtime_error("empty where clause");

        }

      if (where_clause.find("between") != std::string::npos)
        { // input range
          std::cout << "\n 1 "  << std::endl;

          std::cout << "a" << std::endl;
          result_token_vector = get_clause_tokens(where_clause);
          result_token_vector.insert(result_token_vector.begin(), "range");
          /// expect result_token_vector.size() == 5
          return result_token_vector;
        }

      //no input range
      if (where_clause.find("<") != std::string::npos ||
            where_clause.find(">") != std::string::npos)
        {
          std::cout << "\n 2 "  << std::endl;
          // has an < or >
          result_token_vector = get_clause_tokens(where_clause);
          result_token_vector.insert(result_token_vector.begin(), "inequality");
          /// expect result_token_vector.size() == 4
          return result_token_vector;
        }

      /// equality or prefix
      // expect = || = and *
      if (where_clause.find("*") != std::string::npos)
        {
          std::cout << "\n 3 "  << std::endl;
          result_token_vector = get_clause_tokens(where_clause);
          result_token_vector.insert(result_token_vector.begin(), "prefix");
          return result_token_vector;
        }
      if (!(where_clause.find("*") != std::string::npos))
        {
          std::cout << "\n 4 "  << std::endl;
          result_token_vector = get_clause_tokens(where_clause);
          result_token_vector.insert(result_token_vector.begin(), "exact");
          return result_token_vector;
        }
      std::cout << "\n 5 "  << std::endl;
      std::string err = "Invalid where-clause format in:\n" + where_clause;
      throw std::runtime_error(err.c_str());
      /// throw error

    }
  }

int main()
  {
    std::string s = "field ";

    auto x = query::primary_where_clause_parser(s);
    return 1;
  }
