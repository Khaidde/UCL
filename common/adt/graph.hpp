#ifndef COMMON_ADT_GRAPH_HPP
#define COMMON_ADT_GRAPH_HPP

#include "common/adt/set.hpp"
#include "common/adt/vec.hpp"
#include "common/general.hpp"
#include "common/mem.hpp"

namespace ucl {

template <typename T, typename E>
struct Node {
  T data;
  Vec<E> edges;
};

template <typename T>
struct Edge {
  Node<T, Edge<T>> *dest;
};

template <typename T, typename E = Edge<T>>
struct Graph {
  using NodeType = Node<T, E>;
  using NodeList = Vec<NodeType *>;

  using EdgeType = E;

  void init() {
    INIT_MEMCHECK
    nodes.init();
  }

  struct Iterator {
    NodeType *operator*() const { return *current; }

    Iterator &operator++() {
      ++current;
      return *this;
    }

    friend bool operator!=(const Iterator &iterator1, const Iterator &iterator2) {
      return iterator1.current != iterator2.current;
    }

    NodeType **current;
  };

  Iterator begin() {
    ASSERT_MEMCHECK
    return {&nodes.data[0]};
  }

  Iterator end() {
    ASSERT_MEMCHECK
    return {&nodes.data[nodes.length]};
  }

  struct OrderedIterator {
    struct Ordering {
      NodeType *operator*() const { return *ordering; }

      Iterator &operator++() {
        ++ordering;
        return *this;
      }

      friend bool operator!=(const Iterator &iterator1, const Iterator &iterator2) {
        assert(iterator1.graph == iterator2.graph);
        return iterator1.ordering != iterator2.ordering;
      }

      NodeType **ordering;
    };

    Iterator begin() { return {ordering}; }

    Iterator end() { return {ordering + graph->nodes.length}; }

    NodeType **ordering;
    Graph<T, E> *graph;
  };

  void impl_post_order(Allocator *allocator, NodeType ***ordering, NodeType *current, Set<NodeType *> *visited) {
    visited->insert(allocator, current);

    for (auto *edge : current->edges) {
      if (!visited->has(edge->dest)) impl_post_order(allocator, ordering, edge->dest, visited);
    }

    **ordering = current;
    ++(*ordering);
  }

  OrderedIterator post_order(Allocator *allocator) {
    ASSERT_MEMCHECK
    OrderedIterator post_ordering;
    post_ordering.ordering = allocator->construct<NodeType *>(nodes.length);
    post_ordering.graph    = this;

    Set<NodeType *> visited;
    visited.init();
    auto *ordering = post_ordering.ordering;
    for (i32 i = 0; i < nodes.length; ++i) {
      if (!visited.has(nodes.get(i))) impl_post_order(allocator, &ordering, nodes.get(i), &visited);
    }
    return post_ordering;
  }

  NodeType *add_node(Allocator *allocator, T &node_data) {
    ASSERT_MEMCHECK
    nodes.reserve(allocator, nodes.length + 1);
    ++nodes.length;

    auto *node   = allocator->construct<NodeType>();
    nodes.back() = node;

    node->data = node_data;
    node->edges.init();
    return node;
  }

  NodeType *add_node(Allocator *allocator, T &&node_data) {
    ASSERT_MEMCHECK
    return add_node(allocator, node_data);
  }

  EdgeType *link(Allocator *allocator, NodeType *source_node, NodeType *destination_node) {
    source_node->edges.reserve(allocator, source_node->edges.length + 1);
    ++source_node->edges.length;
    auto *edge = &source_node->edges.back();
    edge->dest = destination_node;
    return edge;
  }

  NodeList nodes;
  DEFINE_MEMCHECK
};

} // namespace ucl

#endif
