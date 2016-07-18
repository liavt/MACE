CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

message(STATUS "Building MC-System...")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-System/*.h")
file(GLOB_RECURSE MCS_SRC *.cpp)

add_library(MC-System ${HEADERS} ${MCS_SRC})
target_include_directories (MC-System PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})