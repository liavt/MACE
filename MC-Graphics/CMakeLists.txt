CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

message(STATUS "Building MC-Graphics")

message(STATUS "Finding OpenGL...")
find_package(OpenGL REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Graphics/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

add_executable(MC-Graphics ${HEADERS} ${MCT_SRC})
target_include_directories (MC-Graphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})