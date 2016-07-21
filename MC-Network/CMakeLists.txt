CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-Network...")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Network/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

add_library(MC-Network ${HEADERS} ${MCT_SRC})
target_include_directories (MC-Network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})