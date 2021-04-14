#include <string.h>
#include <algorithm>
#include <eosio/print.hpp>

namespace eosio
{
   namespace internal_use_do_not_use
   {
      extern "C" void printui(uint64_t value)
      {
         char s[21];
         char* ch = s;
         do
         {
            *ch++ = '0' + (value % 10);
            value /= 10;
         } while (value);
         std::reverse(s, ch);
         prints_l(s, ch - s);
      }

   }  // namespace internal_use_do_not_use
}  // namespace eosio
