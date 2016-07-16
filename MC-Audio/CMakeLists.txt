CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE MCA_SRC *.cpp)

add_library(MC-Audio ${MCA_SRC})
target_include_directories (MC-Audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})