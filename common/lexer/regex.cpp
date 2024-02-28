#include "common/lexer/regex.hpp"

#include "common/mem.hpp"

namespace ucl {

struct RegexParser {
  i32 index;
  StringRef regex;

  FAContext *fa_context;
};

bool is_end(RegexParser *regex_parser) { return regex_parser->index == regex_parser->regex.len; }

char peek(RegexParser *regex_parser) {
  assert(!is_end(regex_parser));
  return regex_parser->regex.str[regex_parser->index];
}

void next(RegexParser *regex_parser) { ++regex_parser->index; }

void error_with_info(RegexParser *regex_parser, cstr msg) {
  char *regex = CAllocator::construct<char>(regex_parser->regex.len + 1);
  i32 len     = regex_parser->regex.len;
  regex[len]  = '\0';
  memory_copy(regex, (char *)regex_parser->regex.str, len);

  error("in regex at %d in '%s': %s\n", regex_parser->index, regex, msg);
}

struct NFAComponent {
  FANodeId entry_id;
  FANodeId exit_id;
};

Result parse_infix(RegexParser *regex_parser, NFAComponent *lvalue, i32 min_precedence) {
  if (is_end(regex_parser)) {
    error_with_info(regex_parser, "Expected more characters");
    return err;
  };

  switch (peek(regex_parser)) {
  case '|':
  case '*': error_with_info(regex_parser, "Expected character instead of operator"); return err;
  case '(':
    next(regex_parser);

    parse_infix(regex_parser, lvalue, 0);

    if (peek(regex_parser) != ')') error_with_info(regex_parser, "Expected ) to match previous (");
    next(regex_parser);
    break;
  case ')': error_with_info(regex_parser, "Unexpected )"); break;
  default:
    if ((u8)peek(regex_parser) >= 128) {
      error_with_info(regex_parser, "ASCII value >= 128 not supported");
      return err;
    }
    lvalue->entry_id = add_node(regex_parser->fa_context);
    lvalue->exit_id  = add_node(regex_parser->fa_context);
    add_transition(regex_parser->fa_context, lvalue->entry_id, peek(regex_parser), lvalue->exit_id);
    next(regex_parser);
    break;
  }

  while (!is_end(regex_parser)) {
    char current_char = peek(regex_parser);
    if (current_char == ')') break;

    i8 precedence = 2;
    switch (current_char) {
    case '*': precedence = 3; break;
    case '|': precedence = 1; break;
    default: break;
    }

    if (precedence < min_precedence) break;

    switch (current_char) {
    case '*': {
      next(regex_parser);

      auto star_entry_id = add_node(regex_parser->fa_context);
      auto star_exit_id  = add_node(regex_parser->fa_context);
      add_transition(regex_parser->fa_context, lvalue->exit_id, FAEdge::epsilon, lvalue->entry_id);
      add_transition(regex_parser->fa_context, star_entry_id, FAEdge::epsilon, star_exit_id);
      add_transition(regex_parser->fa_context, star_entry_id, FAEdge::epsilon, lvalue->entry_id);
      add_transition(regex_parser->fa_context, lvalue->exit_id, FAEdge::epsilon, star_exit_id);
      lvalue->entry_id = star_entry_id;
      lvalue->exit_id  = star_exit_id;
      break;
    }
    case '|': {
      next(regex_parser);

      NFAComponent rvalue;
      if (parse_infix(regex_parser, &rvalue, precedence)) return err;

      auto union_entry_id = add_node(regex_parser->fa_context);
      auto union_exit_id  = add_node(regex_parser->fa_context);
      add_transition(regex_parser->fa_context, union_entry_id, FAEdge::epsilon, lvalue->entry_id);
      add_transition(regex_parser->fa_context, union_entry_id, FAEdge::epsilon, rvalue.entry_id);
      add_transition(regex_parser->fa_context, lvalue->exit_id, FAEdge::epsilon, union_exit_id);
      add_transition(regex_parser->fa_context, rvalue.exit_id, FAEdge::epsilon, union_exit_id);

      lvalue->entry_id = union_entry_id;
      lvalue->exit_id  = union_exit_id;
      break;
    }
    default: {
      NFAComponent rvalue;
      if (parse_infix(regex_parser, &rvalue, precedence)) return err;

      add_transition(regex_parser->fa_context, lvalue->exit_id, FAEdge::epsilon, rvalue.entry_id);
      lvalue->exit_id = rvalue.exit_id;
      break;
    }
    }
  }
  return ok;
}

Node<FANode, FAEdge> *generate_nfa(FAContext *fa_context, u32 accept_token, StringRef regex) {
  RegexParser regex_parser;
  regex_parser.index      = 0;
  regex_parser.regex      = regex;
  regex_parser.fa_context = fa_context;

  NFAComponent result_nfa;
  if (parse_infix(&regex_parser, &result_nfa, 0)) return nullptr;

  fa_context->graph.nodes.get(result_nfa.exit_id)->data.accept_token = accept_token;

  return fa_context->graph.nodes.get(result_nfa.entry_id);
}

} // namespace ucl
