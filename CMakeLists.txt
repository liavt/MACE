CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(MACE)

message("Welcome to the MACE build!")

set(CMAKE_VERBOSE_MAKEFILE ON)

message(STATUS "Finding dependencies. If they are not found, CMake will download them. An internet connection will be required.")

set(CATCH_URL "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp")
set(CATCH_DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/include/Catch.hpp")

if (NOT EXISTS "${CATCH_DOWNLOAD_PATH}")
	message(STATUS "Downloading Catch.hpp...")
    file(DOWNLOAD "${CATCH_URL}" "${CATCH_DOWNLOAD_PATH}")
else()
	message(STATUS "Catch found.")
endif()

set(SDL2_SOURCE_NAME "SDL2-2.0.4")
set(SDL2_URL "https://www.libsdl.org/release/${SDL2_SOURCE_NAME}.tar.gz")
set(SDL2_DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/${SDL2_SOURCE_NAME}.tar.gz")
set(SDL2_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${SDL2_SOURCE_NAME}")

if(NOT EXISTS ${SDL2_SOURCE_DIR})
	message(STATUS "SDL2 source not found. Fixing..")
	if (NOT EXISTS "${SDL2_DOWNLOAD_PATH}")
		message(STATUS "Downloading SDL2 source...")
		file(DOWNLOAD "${SDL2_URL}" "${SDL2_DOWNLOAD_PATH}")
	else()
		message(STATUS "SDL2 source found in tar.gz")
	endif()

	message(STATUS "Extracting SDL2 source...")
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xvf ${SDL2_DOWNLOAD_PATH}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
	
	if(NOT EXISTS ${SDL2_SOURCE_DIR})
		message(FATAL_ERROR "Failed to extract SDL2 source! Please retry or file a bug report.")
	else()
		message(STATUS "SDL2 extracted.")
	endif()
	
	file(REMOVE ${SDL2_DOWNLOAD_PATH})
	message(STATUS "Deleted temporary tar")
	message(STATUS "Done adding ${SDL2_SOURCE_NAME}")
else()
	message("SDL2 found.")
endif()

message(STATUS "Dependencies ready. Completing installation...")

add_subdirectory(${SDL_SOURCE_NAME})
#add_subdirectory(SDL_net)
add_subdirectory(MC-System)
add_subdirectory(MC-Window)
add_subdirectory(MC-Graphics)
add_subdirectory(MC-Network)
add_subdirectory(MC-Audio)
add_subdirectory(MC-Testing)

message("Done! Thank you for using MACE.")