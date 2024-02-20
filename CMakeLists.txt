cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME UnoriginalCompilerLibrary)

project(${PROJECT_NAME})

set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

set(CPP_FLAGS
  -Wall
  -Wpedantic
  -Wextra
  -Werror
  -Wsign-conversion
)

add_subdirectory(common)

macro(define_cpp_flags target)
  message("Target: ${target}...")
  target_link_libraries(${target} PRIVATE common_lib)
  target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR})
  target_compile_options(${target} PRIVATE ${CPP_FLAGS})
endmacro()

set(LANGS 
  scft
)

foreach(subdir IN LISTS LANGS)
    add_subdirectory(lang/${subdir})
endforeach()
