#include <array>
#include <vector>
#include <iostream>
#include "../utils/string_utils.h"
#include <string>
#include <string_view>



namespace query
  {
  /*
   Basic where clause parser. For the time being, the only part of the
   query that is parsed for inputs to the B+tree. So far there will only be one
   table of the all files and their chosed metadata
   */

  template <typename InputDataType>
  struct ParsedWhereClause
    {
    std::string& where_field;
    InputDataType &start{};
    InputDataType &end{};

    };

  std::vector<std::string> get_clause_tokens(std::string input)
    {
      std::vector<std::string> result;
      std::size_t new_start = 0;
      std::size_t ind{0};
      for (; ind <= input.size(); ++ind )
        {
          if  (!( input.begin() + ind == input.end())) {
              if (input[ind] == ' ')

                {
                  std::string slice(input.begin() + new_start, input.begin() + ind);
                  std::cout << slice << std::endl;
                  new_start = ind +1;
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

  void primary_where_clause_parser(std::string &where_clause)
    {
      // everything after the where clause

      //Numeric ONLY
      std::string example1 = "field BETWEEN val_1 AND val_2";
      std::string example2 = "FIELD > val ....... FIELD < val";
      //Numeric AND Text
      std::string example3 = "FIELD = val";
      //text only
      std::string example4 = "FIELD = val*";

      if (where_clause.find("between") != std::string::npos)
        { // input range
          std::cout << "a"  << std::endl;

        }
      else
        { //no input range

          std::cout << "a"  << std::endl;
        }


    }
  }

  int main()
    {


      auto x = query::get_clause_tokens("aw is the best");

      return 1;
    }





