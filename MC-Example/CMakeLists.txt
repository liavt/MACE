CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE MCE_SRC *.cpp)

add_executable(MC-Example ${MCE_SRC})

target_link_libraries(MC-Example ${OPENGL_LIBRARIES})
target_link_libraries(MC-Example LINK_PUBLIC SDL2)
target_link_libraries(MC-Example LINK_PUBLIC SDL2main)
target_link_libraries(MC-Example LINK_PUBLIC MC-System)
target_link_libraries(MC-Example LINK_PUBLIC MC-Window)
target_link_libraries(MC-Example LINK_PUBLIC MC-Graphics)
target_link_libraries(MC-Example LINK_PUBLIC MC-Network)
target_link_libraries(MC-Example LINK_PUBLIC MC-Audio)