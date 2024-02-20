#ifndef COMMON_GENERAL_HPP
#define COMMON_GENERAL_HPP

#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#if !defined(__clang__)
#error Compilation only supported with clang
#endif

#if !defined(__linux__)
#error Compilation only supported with linux
#endif

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#error Big endian compilation not supported
#endif

#if defined(NDEBUG)
#define DEBUG 0
#else
#define DEBUG 1
#endif

#define CT_SIZEOF(x)                                                                                                   \
  static const int temp_##x = sizeof(x);                                                                               \
  template <int>                                                                                                       \
  struct sizeof_##x;                                                                                                   \
  static_assert(sizeof_##x<temp_##x>::temp_##x);

using u8    = uint8_t;
using u16   = uint16_t;
using u32   = uint32_t;
using u64   = uint64_t;
using usize = size_t;

using i8   = int8_t;
using i16  = int16_t;
using i32  = int32_t;
using i64  = int64_t;
using size = ssize_t;

using f32 = float;

using cstr = const char *;

namespace ucl {

template <typename T>
struct Encapsulate {
  Encapsulate() = default;
  Encapsulate(T data) : encapsulated_data(data) {}
  operator T() { return encapsulated_data; }

  T encapsulated_data;
};

struct ResultOk {
  operator bool() const { return false; }
};

extern const ResultOk ok;

struct ResultErr {
  operator bool() const { return true; }
};

extern const ResultErr err;

struct Result {
  Result(ResultOk) : value(false) {}
  Result(ResultErr) : value(true) {}
  operator bool() { return value; }

  bool value;
};

void error(cstr msg, ...);

[[noreturn]] void panic(cstr msg, ...);

#if DEBUG
#define DEFINE_MEMCHECK u32 check;
#define INIT_MEMCHECK check = 0xFEEEFEEE;
#define DESTROY_MEMCHECK check = 0xBAADF00D;
#define ASSERT_MEMCHECK                                                                                                \
  if (check != 0xFEEEFEEE) panic("Uninitialized memory access\n");
#else
#define DEFINE_MEMCHECK
#define INIT_MEMCHECK
#define DESTROY_MEMCHECK
#define ASSERT_MEMCHECK
#endif

} // namespace ucl

#endif

