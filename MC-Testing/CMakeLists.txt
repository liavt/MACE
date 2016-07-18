CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

message(STATUS "Building MC-Testing (Unit Tests)")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Testing/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

add_executable(MC-Testing ${HEADERS} ${MCT_SRC})

target_link_libraries(MC-Testing LINK_PUBLIC ${OPENGL_LIBRARIES})
target_link_libraries(MC-Testing LINK_PUBLIC SDL2)
target_link_libraries(MC-Testing LINK_PUBLIC SDL2main)
target_link_libraries(MC-Testing LINK_PUBLIC MC-System)
target_link_libraries(MC-Testing LINK_PUBLIC MC-Window)
target_link_libraries(MC-Testing LINK_PUBLIC MC-Graphics)
target_link_libraries(MC-Testing LINK_PUBLIC MC-Network)
target_link_libraries(MC-Testing LINK_PUBLIC MC-Audio)