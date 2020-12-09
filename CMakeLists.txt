cmake_minimum_required(VERSION 3.11)
project(trap)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

include(cmake/zydis.cmake)

add_executable(trap main.cpp asm/inline_hook.cpp asm/inline_hook.h)
target_link_libraries(trap PRIVATE Zydis)
