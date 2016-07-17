CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

include_directories(${SDL2_INCLUDES})

file(GLOB_RECURSE SDL_NET_SRC *.c)

add_library(SDLNet ${SDL_NET_SRC})
target_include_directories (SDLNet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})