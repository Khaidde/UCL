#include "common/general.hpp"

#include <cstdarg>
#include <sys/resource.h>

namespace ucl {

const ResultOk ok;

const ResultErr err;

void error(cstr msg, ...) {
  fprintf(stderr, "error: ");
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
}

void panic(cstr msg, ...) {
#if DEBUG
  rlimit core_limit = {RLIM_INFINITY, RLIM_INFINITY};
  assert(setrlimit(RLIMIT_CORE, &core_limit) == 0);
  printf("Enabling core dumps...\n");
#endif
  fprintf(stderr, "PANIC: ");
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, msg, args);
  va_end(args);
  abort();
}

} // namespace ucl
