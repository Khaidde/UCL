#ifndef COMMON_ADT_MAP_HPP
#define COMMON_ADT_MAP_HPP

#include "common/adt/hash.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

namespace ucl {

namespace impl {

// Robin hood algorithm based hashmap
template <typename K, typename V, typename H, typename E>
struct Map {
  using Key   = K;
  using Value = V;
  using Hash  = H;
  using Equal = E;

  using MapT = Map<K, V, H, E>;

  struct Entry {
    Key key;
    Value value;
  };

  struct Iterator {
    Entry &operator*() const { return parent->table[index].entry; }

    Entry *operator->() const { return &parent->table[index].entry; }

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
    MapT *parent;
  };

  struct TableSlot {
    Entry entry;
    i32 distance; // Distance from preferred hash slot, distance=0 means entry is empty
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

  Value *insert(Allocator *allocator, Key &key, Value &value) {
    ASSERT_MEMCHECK
    // Load factor of 0.5
    if (length + 1 > capacity >> 1) {
      auto *old_table  = table;
      i32 old_capacity = capacity;

      capacity = capacity ? capacity << 1 : 8;

      table = allocator->construct<TableSlot>(capacity);

      clear();

      for (i32 i = 0; i < old_capacity; ++i) {
        if (old_table[i].distance) insert(allocator, old_table[i].entry.key, old_table[i].entry.value);
      }
    }

    TableSlot temp_slot;
    auto *key_pointer   = &key;
    auto *value_pointer = &value;
    i32 index           = Hash()(key) & (capacity - 1);
    i32 distance        = 1;
    for (i32 off = 0; off < capacity; ++off) {
      if (table[index].distance == 0) {
        if (distance > max_distance) max_distance = distance;

        table[index].entry.key   = *key_pointer;
        table[index].entry.value = *value_pointer;
        table[index].distance    = distance;
        ++length;
        return nullptr;
      }

      if (Equal()(table[index].entry.key, *key_pointer)) return &table[index].entry.value;

      if (table[index].distance < distance) {
        if (distance > max_distance) max_distance = distance;

        temp_slot = table[index];

        table[index].entry.key   = *key_pointer;
        table[index].entry.value = *value_pointer;
        table[index].distance    = distance;

        key_pointer   = &temp_slot.entry.key;
        value_pointer = &temp_slot.entry.value;
        distance      = temp_slot.distance;
      }
      index = (index + 1) & (capacity - 1);
      ++distance;
    }
    assert(!"Map is unexpectedly full");
  }

  Value *insert(Allocator *allocator, Key &&key, Value &val) { return insert(allocator, key, val); }

  Value *insert(Allocator *allocator, Key &key, Value &&val) { return insert(allocator, key, val); }

  Value *insert(Allocator *allocator, Key &&key, Value &&val) { return insert(allocator, key, val); }

  Value *get(Key &key) {
    ASSERT_MEMCHECK
    i32 index = Hash()(key) & (capacity - 1);
    for (i32 off = 0; off < max_distance; ++off) {
      if (table[index].distance && Equal()(table[index].entry.key, key)) return &table[index].entry.value;
      index = (index + 1) & (capacity - 1);
    }
    return nullptr;
  }

  Value *get(Key &&key) { return get(key); }

  TableSlot *table;
  i32 length;
  i32 capacity;
  i32 max_distance;
  DEFINE_MEMCHECK
};

} // namespace impl

template <typename K, typename V>
using Map = impl::Map<K, V, HashFn<K>, EqualFn<K>>;

} // namespace ucl

#endif
