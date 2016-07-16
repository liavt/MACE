CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE MCTEST_SRC *.cpp)

add_executable(MC-Test ${MCTEST_SRC})

target_link_libraries(MC-Test SDL2)
target_link_libraries(MC-Test SDL2main)
target_link_libraries(MC-Test LINK_PUBLIC MC-System)
target_link_libraries(MC-Test LINK_PUBLIC MC-Window)
target_link_libraries(MC-Test LINK_PUBLIC MC-Graphics)
target_link_libraries(MC-Test LINK_PUBLIC MC-Network)
target_link_libraries(MC-Test LINK_PUBLIC MC-Audio)