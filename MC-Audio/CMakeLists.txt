CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

message(STATUS "Building MC-Audio...")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Audio/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

add_library(MC-Audio ${HEADERS} ${MCT_SRC})
target_include_directories (MC-Audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})