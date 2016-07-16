CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
 
PROJECT(MACE)

set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(SDL2)
add_subdirectory(SDL_net)
add_subdirectory(MC-System)
add_subdirectory(MC-Window)
add_subdirectory(MC-Graphics)
add_subdirectory(MC-Network)
add_subdirectory(MC-Audio)
add_subdirectory(MC-Test)
#add_subdirectory(MC-Testing)

#Catch.h is missing in MC-Testing