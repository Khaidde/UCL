#include "common/lexer/nfa.hpp"

namespace ucl {

void gather_transitions(FAContext *fa_context, Node<FANode, FAEdge> *source, Node<FANode, FAEdge> *current_dfs) {
  current_dfs->data.visited = true;
  fa_context->visited.push_back(&fa_context->bump_allocator, current_dfs);

  if (current_dfs->data.accept_token < source->data.accept_token) {
    source->data.accept_token = current_dfs->data.accept_token;
  }

  for (auto *edge : current_dfs->edges) {
    if (edge->symbol == FAEdge::epsilon) {
      if (!edge->dest->data.visited) gather_transitions(fa_context, source, edge->dest);
    } else if (source != current_dfs) {
      auto *new_edge   = fa_context->graph.link(&fa_context->bump_allocator, source, edge->dest);
      new_edge->symbol = edge->symbol;
      ++edge->dest->data.reference_count;
    }
  }
}

void cleanup_zero_reference_nodes(Node<FANode, FAEdge> *node) {
  if (node->data.reference_count > 0) return;
  for (auto *edge : node->edges) {
    --edge->dest->data.reference_count;
    cleanup_zero_reference_nodes(edge->dest);
  }
}

void delete_episilon_transitions(Node<FANode, FAEdge> *node) {
  for (i32 i = 0; i < node->edges.length; ++i) {
    if (node->edges.get_reference(i)->symbol == FAEdge::epsilon) {
      --node->edges.get_reference(i)->dest->data.reference_count;
      cleanup_zero_reference_nodes(node->edges.get_reference(i)->dest);

      node->edges.data[i] = node->edges.back();
      node->edges.pop_back();
    }
  }
}

void reduce_nfa(FAContext *fa_context) {
  auto post_ordering = fa_context->graph.post_order(&fa_context->bump_allocator);

  for (auto *node : post_ordering) {
    for (auto *visited_node : fa_context->visited) {
      (*visited_node)->data.visited = false;
    }
    fa_context->visited.clear();

    gather_transitions(fa_context, node, node);
    delete_episilon_transitions(node);
  }

  for (i32 i = 0; i < fa_context->graph.nodes.length; ++i) {
    fa_context->graph.nodes.get(i)->data.id = i;
  }

  /*
  for (auto *node : post_ordering) {
    if (node == fa_context->entry_node) continue;

    if (node->data.reference_count == 0) {
      fa_context->graph.nodes.back()->data.id     = node->data.id;
      fa_context->graph.nodes.data[node->data.id] = fa_context->graph.nodes.back();
      fa_context->graph.nodes.pop_back();
    }
  }
  */
}

} // namespace ucl
