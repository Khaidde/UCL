#ifndef COMMON_MEM_HPP
#define COMMON_MEM_HPP

#include "common/general.hpp"
#include <malloc.h>

namespace ucl {

#if DEBUG
extern struct GlobalMemoryStats {
  void print_memory_usage();

  i32 bytes_requested = 0;
  i32 bytes_used      = 0;
  i32 bytes_malloc    = 0;
} global_mem_statistics;
#endif

// Simple allocator which is just a wrapper for malloc and free
struct CAllocator {
  template <typename T>
  static T *construct(size bytes = 1) {
#if DEBUG
    global_mem_statistics.bytes_malloc += bytes * size(sizeof(T));
#endif
    return (T *)malloc(usize(bytes) * sizeof(T));
  }

  template <typename T>
  static void destruct(T *pointer) {
    free(pointer);
  }
};

struct BumpAllocator {
  static const i32 capacity = 1024 * 1024;

  void init() {
    INIT_MEMCHECK
    offset = 0;
    data   = CAllocator::construct<i8>(capacity);
  }

  void destroy() {
    CAllocator::destruct(data);
    DESTROY_MEMCHECK
  }

  template <typename T>
  T *construct(size bytes = 1) {
    ASSERT_MEMCHECK

    // Ensure all allocations are aligned to the size of a pointer
    size pointer_size = size(sizeof(intptr_t));
    i32 aligned_bytes = i32((size(sizeof(T)) * bytes + pointer_size - 1) & ~(pointer_size - 1));

#if DEBUG
    global_mem_statistics.bytes_requested += size(sizeof(T)) * bytes;
    global_mem_statistics.bytes_used += aligned_bytes;
    if (offset + aligned_bytes <= capacity) {
#endif
      T *pointer = (T *)(data + offset);
      offset += aligned_bytes;
      return pointer;
#if DEBUG
    }
    panic("BumpAllocator cannot exceed total of %d bytes\n", capacity);
#endif
  }

  i8 *data;
  i32 offset;
  DEFINE_MEMCHECK
};

using Allocator = BumpAllocator;

template <typename T>
void memory_copy(T *destination, T *source, i32 count) {
#if DEBUG
  for (i32 i = 0; i < count; ++i) destination[i] = source[i];
#else
  memcpy(destination, source, usize(count) * sizeof(T));
#endif
}

template <typename T>
void memory_clear(T *destination, i32 count) {
#if DEBUG
  for (i32 i = 0; i < i32(sizeof(T)) * count; ++i) *(((u8 *)destination) + 1) = 0;
#else
  memset(destination, 0, usize(count) * sizeof(T));
#endif
}

} // namespace ucl

#endif
