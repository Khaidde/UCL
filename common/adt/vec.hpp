#ifndef COMMON_ADT_VEC_HPP
#define COMMON_ADT_VEC_HPP

#include "common/general.hpp"
#include "common/mem.hpp"

namespace ucl {

template <typename T>
struct Vec {
  struct Iterator {
    T *operator*() const { return current; }

    T *operator->() const { return current; }

    Iterator &operator++() {
      ++current;
      return *this;
    }

    friend bool operator!=(const Iterator &iterator1, const Iterator &iterator2) {
      return iterator1.current != iterator2.current;
    }

    T *current;
  };

  void init() {
    INIT_MEMCHECK
    data     = nullptr;
    length   = 0;
    capacity = 0;
  }

  void assert_memcheck(){ASSERT_MEMCHECK}

  Iterator begin() {
    ASSERT_MEMCHECK
    return {data};
  }

  Iterator end() {
    ASSERT_MEMCHECK
    return {&data[length]};
  }

  void reserve(Allocator *allocator, i32 new_capacity) {
    ASSERT_MEMCHECK
    assert(new_capacity >= 0);
    if (capacity < new_capacity) {
      while (capacity < new_capacity) {
        // cap = cap * 1.5 + 8
        capacity = (capacity << 1) - (capacity >> 1) + 8;
      }
      resize(allocator, capacity);
    }
  }

  void resize(Allocator *allocator, i32 new_capacity) {
    ASSERT_MEMCHECK
    T *new_data = allocator->construct<T>(new_capacity);
    if (data) memory_copy(new_data, data, length);
    data     = new_data;
    capacity = new_capacity;
  }

  void clear() { length = 0; }

  T &front() {
    ASSERT_MEMCHECK
    assert(length > 0);
    return data[0];
  }

  T &back() {
    ASSERT_MEMCHECK
    assert(length > 0);
    return data[length - 1];
  }

  void push_back(Allocator *allocator, T &value) {
    ASSERT_MEMCHECK
    reserve(allocator, length + 1);
    data[length++] = value;
  }

  void push_back(Allocator *allocator, T &&value) { push_back(allocator, value); }

  void pop_back() {
    ASSERT_MEMCHECK
    --length;
  }

  T get(i32 index) {
    ASSERT_MEMCHECK
    assert(index < length);
    return data[index];
  }

  T *get_reference(i32 index) {
    ASSERT_MEMCHECK
    assert(index < length);
    return &data[index];
  }

  T *data;
  i32 length;
  i32 capacity;
  DEFINE_MEMCHECK
};

} // namespace ucl

#endif
