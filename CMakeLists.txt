CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(MACE)

message("Welcome to the MACE build!")

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#This defines where all of our includes are located
set(MACE_INCLUDES "${CMAKE_SOURCE_DIR}/include")

set(SDL_NET_SOURCE_NAME "SDL2_net-2.0.1")
set(SDL_NET_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${SDL_NET_SOURCE_NAME}")
set(SDL_NET_URL "https://www.libsdl.org/projects/SDL_net/release/${SDL_NET_SOURCE_NAME}.tar.gz")
set(SDL_NET_DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/${SDL_NET_SOURCE_NAME}.tar.gz")
#SDL_Net doesnt have a Cmake. So we made our own.
set(SDL_NET_CMAKE "SDLNet_CMake.txt")

#Source name is the version to use
set(SDL2_SOURCE_NAME "SDL2-2.0.4")

set(SDL2_URL "https://www.libsdl.org/release/${SDL2_SOURCE_NAME}.tar.gz")
set(SDL2_DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/${SDL2_SOURCE_NAME}.tar.gz")
#This is where the source will be located in
set(SDL2_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${SDL2_SOURCE_NAME}")
#This is where the headers for SDL2 will be located in
set(SDL2_INCLUDES "${MACE_INCLUDES}/SDL")
set(CATCH_URL "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp")
#This is where Catch should be located in
set(CATCH_DOWNLOAD_PATH "${MACE_INCLUDES}/Catch.h")

message(STATUS "Building ${SDL2_SOURCE_NAME}...")
add_subdirectory("${SDL2_SOURCE_NAME}")

message(STATUS "Building ${SDL_NET_SOURCE_NAME}...")
add_subdirectory("${SDL_NET_SOURCE_DIR}" )

add_subdirectory(MC-System)
add_subdirectory(MC-Window)
add_subdirectory(MC-Graphics)
add_subdirectory(MC-Network)
add_subdirectory(MC-Audio)
add_subdirectory(MC-Testing)

message("Done! Thank you for using MACE.")