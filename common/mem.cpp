#include "common/mem.hpp"

namespace ucl {

#if DEBUG
GlobalMemoryStats global_mem_statistics;
#endif

void GlobalMemoryStats::print_memory_usage() {
  printf("Bytes Requested: %8db\n", bytes_requested);
  printf("Bytes Used:      %8db\n", bytes_used);
  printf("Bytes Malloc'd:  %8db\n", bytes_malloc);
}

} // namespace ucl
