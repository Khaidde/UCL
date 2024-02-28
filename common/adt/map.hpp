#ifndef COMMON_ADT_MAP_HPP
#define COMMON_ADT_MAP_HPP

#include "common/adt/hash.hpp"
#include "common/adt/set.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

namespace ucl {

template <typename K, typename V>
struct Entry {
  K key;
  V value;
};

template <typename K, typename V>
struct HashFn<Entry<K, V>> {
  HashValue operator()(Entry<K, V> entry) { return HashFn<K>()(entry.key); }
};

template <typename K, typename V>
struct EqualFn<Entry<K, V>> {
  bool operator()(Entry<K, V> entry1, Entry<K, V> entry2) { return EqualFn<K>()(entry1.key, entry2.key); }
};

template <typename K, typename V>
struct Map {
  using Key   = K;
  using Value = V;

  using EntryT = Entry<K, V>;

  using SetT = Set<EntryT>;

  void init() {
    INIT_MEMCHECK
    set.init();
  }

  void clear() {
    ASSERT_MEMCHECK
    set.clear();
  }

  typename SetT::Iterator begin() {
    ASSERT_MEMCHECK
    return set.begin();
  }

  typename SetT::Iterator end() {
    ASSERT_MEMCHECK
    return set.end();
  }

  Value *insert(Allocator *allocator, Key &key, Value &value) {
    EntryT entry;
    entry.key    = key;
    entry.value  = value;
    auto *result = set.insert(allocator, entry);
    return result ? &result->value : nullptr;
  }

  Value *insert(Allocator *allocator, Key &&key, Value &val) { return insert(allocator, key, val); }

  Value *insert(Allocator *allocator, Key &key, Value &&val) { return insert(allocator, key, val); }

  Value *insert(Allocator *allocator, Key &&key, Value &&val) { return insert(allocator, key, val); }

  Value *get(Key &key) {
    EntryT entry;
    entry.key  = key;
    auto *data = set.get(entry);
    return data ? &data->value : nullptr;
  }

  Value *get(Key &&key) { return get(key); }

  SetT set;
  DEFINE_MEMCHECK
};

} // namespace ucl

#endif
