CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-Graphics")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Graphics/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

add_library(MC-Graphics ${HEADERS} ${MCT_SRC})
target_include_directories (MC-Graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})