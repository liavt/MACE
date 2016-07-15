CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${MCSys_INCLUDE_DIRS})

file(GLOB_RECURSE MCA_SRC *.cpp)

add_library(MC-Audio ${MCA_SRC})
target_include_directories (MC-Audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})