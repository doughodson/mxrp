
#include "mixr/base/util/string_utils.hpp"

#include <algorithm>
#include <cctype>
#include <string>

namespace mixr {
namespace base {

// is this an a ascii number?
bool isNumber(const std::string& s)
{
   if (s.empty()) {
      return false;
   }

   for (int i = 0; s[i] != '\0'; i++) {
      if ( !std::isdigit(s[i]) && s[i] != '.' ) {
         return false;
      }
   }
   return true;
}

// convert a ascii number string to a number
double getNumber(const std::string& s)
{
   if (isNumber(s)) {
      return std::atof(s.c_str());
   }
   return 0.0;
}

// is this an a ascii integer
bool isInteger(const std::string& s)
{
   if (s.empty()) {
      return false;
   }

   for (int i = 0; s[i] != '\0'; i++) {
      if ( !std::isdigit(s[i]) ) {
         return false;
      }
   }
   return true;
}

// convert a ascii number string to an integer
int getInteger(const std::string& s)
{
   if (isInteger(s)) {
      return std::atoi(s.c_str());
   }
   return 0;
}

}
}