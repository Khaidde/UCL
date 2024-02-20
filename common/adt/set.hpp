#ifndef COMMON_ADT_SET_HPP
#define COMMON_ADT_SET_HPP

#include "common/adt/hash.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

namespace ucl {

namespace impl {

// Robin hood algorithm based hashmap
template <typename T, typename H, typename E>
struct Set {
  using Hash  = H;
  using Equal = E;

  using SetT = Set<T, H, E>;

  struct TableSlot {
    T data;
    i32 distance; // Distance from preferred hash slot, distance=0 means entry is empty
  };

  struct Iterator {
    T &operator*() const { return parent->table[index].data; }

    T *operator->() const { return &parent->table[index].data; }

    Iterator &operator++() {
      index = parent->get_valid_entry_index(index + 1);
      return *this;
    }

    friend bool operator==(const Iterator &it1, const Iterator &it2) {
      assert(it1.parent == it2.parent);
      return it1.index == it2.index;
    }

    friend bool operator!=(const Iterator &it1, const Iterator &it2) {
      assert(it1.parent == it2.parent);
      return it1.index != it2.index;
    }

    i32 index;
    SetT *parent;
  };

  void init() {
    INIT_MEMCHECK
    capacity     = 0;
    table        = nullptr;
    length       = 0;
    max_distance = 0;
    clear();
  }

  void clear() {
    ASSERT_MEMCHECK
    length       = 0;
    max_distance = 0;
    memory_clear(table, capacity);
  }

  i32 get_valid_entry_index(i32 index) {
    ASSERT_MEMCHECK
    while (index < capacity && !table[index].distance) index++;
    return index;
  }

  Iterator begin() {
    ASSERT_MEMCHECK
    return {get_valid_entry_index(0), this};
  }

  Iterator end() {
    ASSERT_MEMCHECK
    return {capacity, this};
  }

  T *insert(Allocator *allocator, T &data) {
    ASSERT_MEMCHECK
    // Load factor of 0.5
    if (length + 1 > capacity >> 1) {
      auto *old_table  = table;
      i32 old_capacity = capacity;

      capacity = capacity ? capacity << 1 : 8;

      table = allocator->construct<TableSlot>(capacity);

      clear();

      for (i32 i = 0; i < old_capacity; ++i) {
        if (old_table[i].distance) insert(allocator, old_table[i].data);
      }
    }

    TableSlot temp_slot;
    auto *data_pointer = &data;
    i32 index          = Hash()(data) & (capacity - 1);
    i32 distance       = 1;
    for (i32 off = 0; off < capacity; ++off) {
      if (table[index].distance == 0) {
        if (distance > max_distance) max_distance = distance;

        table[index].data     = *data_pointer;
        table[index].distance = distance;
        ++length;
        return nullptr;
      }

      if (Equal()(table[index].data, *data_pointer)) return &table[index].data;

      if (table[index].distance < distance) {
        if (distance > max_distance) max_distance = distance;

        temp_slot = table[index];

        table[index].data     = *data_pointer;
        table[index].distance = distance;

        data_pointer = &temp_slot.data;
        distance     = temp_slot.distance;
      }
      index = (index + 1) & (capacity - 1);
      ++distance;
    }
    assert(!"Map is unexpectedly full");
  }

  bool has(T &data) {
    ASSERT_MEMCHECK
    i32 index = Hash()(data) & (capacity - 1);
    for (i32 off = 0; off < max_distance; ++off) {
      if (table[index].distance && Equal()(table[index].data, data)) return true;
      index = (index + 1) & (capacity - 1);
    }
    return false;
  }

  bool has(T &&data) { return has(data); }

  TableSlot *table;
  i32 length;
  i32 capacity;
  i32 max_distance;
  DEFINE_MEMCHECK
};

} // namespace impl

template <typename T>
using Set = impl::Set<T, HashFn<T>, EqualFn<T>>;

} // namespace ucl

#endif
