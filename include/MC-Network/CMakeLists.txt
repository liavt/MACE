CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

file(GLOB_RECURSE MCN_SRC *.cpp)

add_library(MC-Network ${MCN_SRC})
target_include_directories (MC-Network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})