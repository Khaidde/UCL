#include "common/lexer/lexer.hpp"

#include "common/lexer/nfa.hpp"
#include "common/lexer/regex.hpp"

namespace ucl {

void dump_graph(FILE *out, FAContext *fa_context) {
  fprintf(out, "digraph G {\n");
  for (auto *node : fa_context->graph) {
    fprintf(out, "  n%d[shape=", node->data.id);
    if (node->data.reference_count == 0) {
      fprintf(out, "rectangle");
    } else {
      if (node->data.accept_token != FANode::no_accept) {
        fprintf(out, "doublecircle");
      } else {
        fprintf(out, "circle");
      }
    }
    if (node->data.accept_token != FANode::no_accept) {
      fprintf(out, ",label=\"n%d\\n%d\"", node->data.id, node->data.accept_token);
    } else {
      fprintf(out, "");
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
  fa_context.visited.init();

  fa_context.entry_node = fa_context.graph.nodes.get(add_node(&fa_context));

  // TODO: get the correct accept_token value
  auto *regex_entry_node = generate_nfa(&fa_context, 555, strref("(ab|c)*"));
  if (!regex_entry_node) {
    error("Failed to generate nfa\n");
    return err;
  }
  auto *edge   = fa_context.graph.link(&fa_context.bump_allocator, fa_context.entry_node, regex_entry_node);
  edge->symbol = FAEdge::epsilon;

  reduce_nfa(&fa_context);

  dump_graph(stdout, &fa_context);

  return ok;
}

FANodeId add_node(FAContext *fa_context) {
  auto fa_node_id = FANodeId(fa_context->graph.nodes.length);

  FANode fa_node;
  fa_node.id              = fa_node_id;
  fa_node.accept_token    = FANode::no_accept;
  fa_node.visited         = false;
  fa_node.reference_count = 0;
  fa_context->graph.add_node(&fa_context->bump_allocator, fa_node);
  return fa_node_id;
}

void add_transition(FAContext *fa_context, FANodeId source_id, char symbol, FANodeId destination_id) {
  auto *source_node      = fa_context->graph.nodes.get(i32(source_id));
  auto *destination_node = fa_context->graph.nodes.get(i32(destination_id));
  auto *edge             = fa_context->graph.link(&fa_context->bump_allocator, source_node, destination_node);
  edge->symbol           = symbol;
  ++destination_node->data.reference_count;
}

} // namespace ucl
