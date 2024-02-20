#include "common/lexer/lexer.hpp"

#include "common/lexer/regex.hpp"

namespace ucl {

void dump_graph(FILE *out, FAContext *fa_context) {
  fprintf(out, "digraph G {\n");
  for (auto *node : fa_context->graph) {
    fprintf(out, "  n%d[shape=", node->data.id);
    if (node->data.accept_token != FANode::no_accept) {
      fprintf(out, "doublecircle,label=\"n%d\\n%d\"", node->data.id, node->data.accept_token);
    } else {
      fprintf(out, "circle");
    }
    fprintf(out, "]\n");
    for (auto *fa_edge : node->edges) {
      fprintf(out, "  n%d->n%d", node->data.id, fa_edge->dest->data.id);
      if (fa_edge->symbol) {
        fprintf(out, "[label=\"%c\"]", fa_edge->symbol);
      } else {
        fprintf(out, "[style=dotted]");
      }
      fprintf(out, "\n");
    }
  }
  fprintf(out, "}\n");
}

Result generate_lexer() {
  FAContext fa_context;
  fa_context.bump_allocator.init();
  fa_context.graph.init();

  if (generate_nfa(&fa_context, strref("(ab|c)*|d"))) {
    error("Failed to generate nfa\n");
    return err;
  }

  dump_graph(stdout, &fa_context);

  return ok;
}

FANodeId add_node(FAContext *fa_context) {
  auto fa_node_id = FANodeId(fa_context->graph.nodes.length);

  FANode fa_node;
  fa_node.id           = fa_node_id;
  fa_node.accept_token = FANode::no_accept;
  fa_context->graph.add_node(&fa_context->bump_allocator, fa_node);
  return fa_node_id;
}

void add_transition(FAContext *fa_context, FANodeId source_id, char symbol, FANodeId destination_id) {
  auto *source_node      = fa_context->graph.nodes.get(i32(source_id));
  auto *destination_node = fa_context->graph.nodes.get(i32(destination_id));
  auto *edge             = fa_context->graph.link(&fa_context->bump_allocator, source_node, destination_node);
  edge->symbol           = symbol;
}

} // namespace ucl
