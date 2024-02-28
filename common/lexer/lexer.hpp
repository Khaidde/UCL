#ifndef COMMON_LEXER_LEXER_HPP
#define COMMON_LEXER_LEXER_HPP

#include "common/adt/graph.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

namespace ucl {

struct FANodeId : Encapsulate<i32> {
  FANodeId() = default;
  explicit FANodeId(i32 data) : Encapsulate<i32>(data) {}
};

struct FANode {
  static const u32 no_accept = u32(-1);

  i32 id;
  u32 accept_token;
  bool visited;

  i32 reference_count;
};

struct FAEdge {
  static const char epsilon = 0;

  char symbol;
  Node<FANode, FAEdge> *dest;
};

struct FAContext {
  BumpAllocator bump_allocator;

  Graph<FANode, FAEdge> graph;
  Node<FANode, FAEdge> *entry_node;

  Vec<Node<FANode, FAEdge> *> visited;
};

Result generate_lexer();

FANodeId add_node(FAContext *fa_context);

void add_transition(FAContext *fa_context, FANodeId source_id, char symbol, FANodeId destination_id);

} // namespace ucl

#endif
