#Because SDL_net doesn't have a CMake, we made our own. This does not mean we made SDL_net.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding SDLNet...")

include_directories(${SDL2_INCLUDES})

file(GLOB_RECURSE HEADERS "${SDL2_INCLUDES}/*.h")
file(GLOB_RECURSE MCT_SRC *.c)

if(${MACE_STATIC_LIBRARY})
	add_library(SDLNet-static STATIC ${HEADERS} ${MCT_SRC})
	target_link_libraries(SDLNet-static LINK_PUBLIC SDL2-static)
	target_include_directories (SDLNet-static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
else()
	add_library(SDLNet SHARED ${HEADERS} ${MCT_SRC})
	target_link_libraries(SDLNet LINK_PUBLIC SDL2)
	target_include_directories (SDLNet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
