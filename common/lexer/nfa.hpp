#ifndef COMMON_LEXER_NFA_HPP
#define COMMON_LEXER_NFA_HPP

#include "common/general.hpp"
#include "common/lexer/lexer.hpp"

namespace ucl {

void reduce_nfa(FAContext *fa_context);

} // namespace ucl

#endif
