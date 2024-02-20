set(COMMON_LIB common_lib)

set(SRCS
  lexer/lexer.cpp
  lexer/regex.cpp
  general.cpp
  mem.cpp
)

add_library(${COMMON_LIB} ${SRCS})
target_include_directories(${COMMON_LIB} PRIVATE ${CMAKE_SOURCE_DIR})
target_compile_options(${COMMON_LIB} PRIVATE ${CPP_FLAGS})
