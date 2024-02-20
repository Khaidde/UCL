#ifndef COMMON_ADT_STRING_HPP
#define COMMON_ADT_STRING_HPP

#include "common/general.hpp"

namespace ucl {

struct StringRef {
  cstr str;
  i32 len;
};

inline StringRef strref(cstr string) {
  StringRef result{string, 0};
  while (*string) {
    ++result.len;
    ++string;
  }
  return result;
}

} // namespace ucl

#endif
