#ifndef COMMON_LEXER_REGEX_HPP
#define COMMON_LEXER_REGEX_HPP

#include "common/adt/string.hpp"
#include "common/general.hpp"
#include "common/lexer/lexer.hpp"

namespace ucl {

Node<FANode, FAEdge> *generate_nfa(FAContext *fa_context, u32 accept_token, StringRef regex);

} // namespace ucl

#endif
