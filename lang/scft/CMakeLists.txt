set(EXEC scftc)

set(SRCS
  driver.cpp
)

add_executable(${EXEC} ${SRCS})

define_cpp_flags(${EXEC})
