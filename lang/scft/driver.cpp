#include "common/adt/graph.hpp"
#include "common/adt/string.hpp"
#include "common/adt/vec.hpp"
#include "common/general.hpp"
#include "common/lexer/lexer.hpp"
#include "common/mem.hpp"

i32 main(i32 argc, cstr *argv) {
  if (argc != 2) {
    fprintf(stderr, "err: expected file to compile");
    return ucl::err;
  }

  ucl::BumpAllocator alloc;
  alloc.init();

  ucl::Vec<i32> other;
  other.init();

  other.push_back(&alloc, 123);
  for (i32 i = 1; i < 56; ++i) other.push_back(&alloc, other.get(i - 1) + 123);

  for (auto *num : other) printf("::%d\n", *num);

  struct TestEdge {
    i32 cost;
    ucl::Node<i32, TestEdge> *dest;
  };

  ucl::Graph<i32, TestEdge> test_graph;
  test_graph.init();
  auto *root = test_graph.add_node(&alloc, 0);
  for (i32 i = 1; i < 10; ++i) {
    auto *node = test_graph.add_node(&alloc, i * 10);
    for (i32 k = 0; k < i; ++k) {
      auto *edge = test_graph.link(&alloc, root, node);
      edge->cost = i * 10 + k;
    }
  }

  for (auto *node : test_graph) {
    for (auto *edge : node->edges) {
      printf(":-:%d==%d->%d\n", edge->cost, node->data, edge->dest->data);
    }
  }

  printf("Compiling %s...\n", argv[1]);

  ucl::generate_lexer();

  ucl::global_mem_statistics.print_memory_usage();

  ucl::panic("lol\n");
  return 0;
}
