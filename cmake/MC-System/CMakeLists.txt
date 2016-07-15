CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

file(GLOB_RECURSE MCS_SRC *.cpp)

add_library(MC-System ${MCS_SRC})
target_include_directories (MC-System PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})