CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(MACE)

message("Welcome to the MACE build!")

#This defines where all of our includes are located
set(MACE_INCLUDES "${CMAKE_SOURCE_DIR}/include")

set(CMAKE_VERBOSE_MAKEFILE ON)

message(STATUS "Finding dependencies. If they are not found, CMake will download them. An internet connection will be required.")

#Catch is for unit testing.
set(CATCH_URL "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp")
#This is where Catch should be located in
set(CATCH_DOWNLOAD_PATH "${MACE_INCLUDES}/Catch.h")

find_package(OpenGL REQUIRED)

if (NOT EXISTS "${CATCH_DOWNLOAD_PATH}")
	#Catch is one header file, so its easy to fix. Just download it!
	message(STATUS "Downloading Catch..")
    file(DOWNLOAD "${CATCH_URL}" "${CATCH_DOWNLOAD_PATH}")
	message(STATUS "Done downloading Catch.")
else()
	message(STATUS "Catch found.")
endif()

#Source name is the version to use
set(SDL2_SOURCE_NAME "SDL2-2.0.4")

set(SDL2_URL "https://www.libsdl.org/release/${SDL2_SOURCE_NAME}.tar.gz")
set(SDL2_DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/${SDL2_SOURCE_NAME}.tar.gz")
#This is where the source will be located in
set(SDL2_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${SDL2_SOURCE_NAME}")
#This is where the headers for SDL2 will be located in
set(SDL2_INCLUDES "${MACE_INCLUDES}/SDL")

if(NOT EXISTS ${SDL2_SOURCE_DIR})
	message(STATUS "SDL2 source not found. Fixing..")
	#First we check if the .tar.gz is downloaded. There may be chance it is already downloaded and not extracted
	if (NOT EXISTS "${SDL2_DOWNLOAD_PATH}")
		message(STATUS "Downloading SDL2 source...")
		file(DOWNLOAD "${SDL2_URL}" "${SDL2_DOWNLOAD_PATH}")
	else()
		message(STATUS "SDL2 source found in tar.gz")
	endif()

	message(STATUS "Extracting SDL2 source...")
	#Next we extract the .tar.gz. This works on Windows as well
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xvf ${SDL2_DOWNLOAD_PATH}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
	
	#If the extraction fails, go to here.
	if(NOT EXISTS ${SDL2_SOURCE_DIR})
		message(FATAL_ERROR "Failed to extract SDL2 source! Please retry or file a bug report.")
	else()
		message(STATUS "SDL2 extracted.")
	endif()
	
	#Remove the extracted tar. We dont need the original anymore
	file(REMOVE ${SDL2_DOWNLOAD_PATH})
	message(STATUS "Deleted temporary tar")
	message(STATUS "Done adding ${SDL2_SOURCE_NAME}")
else()
	message(STATUS "SDL2 found.")
endif()

#Just because we download the source doesnt mean we can just use the headers! We have to move them to our include directory.
if(NOT EXISTS ${SDL2_INCLUDES})
	message(STATUS "SDL2 includes not found. Fixing...")
	
	#Simply move the headers from the source into our directory
	file(COPY "${SDL2_SOURCE_NAME}/include/" DESTINATION "${SDL2_INCLUDES}")
	
	message(STATUS "SDL2 Includes added!")
else()
	message(STATUS "Found SDL2 includes.")
endif()

if(NOT EXISTS "${SDL2_INCLUDES}/SDL_net.h")
	message(STATUS "Moving SDL_net headers to include/SDL directory... ")
	file(COPY "${CMAKE_SOURCE_DIR}/SDL_net/SDLnetsys.h" DESTINATION "${SDL2_INCLUDES}")
	file(COPY "${CMAKE_SOURCE_DIR}/SDL_net/SDL_net.h" DESTINATION "${SDL2_INCLUDES}")

	message(STATUS "SDL_net Includes added!")
else()
	message(STATUS "Found SDL_net includes.")
endif()

message(STATUS "Dependencies ready. Completing installation...")

message(STATUS "Building ${SDL2_SOURCE_NAME}...")
add_subdirectory("${SDL2_SOURCE_NAME}")
add_subdirectory(SDL_net)
add_subdirectory(MC-System)
add_subdirectory(MC-Window)
add_subdirectory(MC-Graphics)
add_subdirectory(MC-Network)
add_subdirectory(MC-Audio)
add_subdirectory(MC-Testing)
add_subdirectory(MC-Example)

message("Done! Thank you for using MACE.")