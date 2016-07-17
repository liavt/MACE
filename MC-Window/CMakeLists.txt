CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

message(STATUS "Building MC-Window")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Window/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

add_executable(MC-Window ${HEADERS} ${MCT_SRC})
target_include_directories (MC-Window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})