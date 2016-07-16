CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE MCW_SRC *.cpp)

add_library(MC-Window ${MCW_SRC})
target_include_directories (MC-Window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})