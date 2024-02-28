#ifndef COMMON_ADT_HASH_HPP
#define COMMON_ADT_HASH_HPP

#include "common/general.hpp"
#include <cstring>

namespace ucl {

using HashValue = i32;

template <typename K>
struct HashFn {
  static_assert(!sizeof(K /*unused*/), "No hash function implemented for type");
  HashValue operator()(K /*unused*/) { return 0; }
};

template <typename K>
struct HashFn<K *> {
  HashValue operator()(K *ptr) { return uintptr_t(ptr) / sizeof(K); }
};

template <>
struct HashFn<i32> {
  HashValue operator()(i32 num) { return num; }
};

template <>
struct HashFn<cstr> {
  HashValue operator()(cstr ptr) {
    HashValue hash = 1;
    while (*ptr) {
      hash = ((hash << 5U) - hash) + HashValue(*ptr);
      ++ptr;
    }
    return hash;
  }
};

template <typename K>
struct EqualFn {
  static_assert(!sizeof(K /*unused*/), "No equal function implemented for type");
  bool operator()(K /*unused*/, K /*unused*/) { return false; }
};

template <typename K>
struct EqualFn<K *> {
  bool operator()(K *ptr1, K *ptr2) { return ptr1 == ptr2; }
};

template <>
struct EqualFn<i32> {
  bool operator()(i32 num1, i32 num2) { return num1 == num2; }
};

template <>
struct EqualFn<cstr> {
  bool operator()(cstr ptr1, cstr ptr2) { return strcmp(ptr1, ptr2); }
};

} // namespace ucl

#endif
