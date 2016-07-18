CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

message(STATUS "Building MC-Network...")

if(NOT EXISTS "${SDL_NET_SOURCE_DIR}/CMakeLists.txt")
	message(STATUS "SDL_net doesn't have a CMakeLists! Adding one...")
	
	file(COPY "${CMAKE_SOURCE_DIR}/${SDL_NET_CMAKE}" DESTINATION "${SDL_NET_SOURCE_DIR}")
	
	file(RENAME "${SDL_NET_SOURCE_DIR}/${SDL_NET_CMAKE}" "${SDL_NET_SOURCE_DIR}/CMakeLists.txt")
else()
	message(STATUS "SDL_net has a CMakeLists")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Network/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

add_library(MC-Network ${HEADERS} ${MCT_SRC})
target_include_directories (MC-Network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})