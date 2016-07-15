CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${MCSys_INCLUDE_DIRS})

file(GLOB_RECURSE MCG_SRC *.cpp)

add_library(MC-Graphics ${MCG_SRC})
target_include_directories (MC-Graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})