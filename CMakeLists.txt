CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(MACE)

message("Welcome to the MACE build!")

set(CMAKE_VERBOSE_MAKEFILE ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

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
	file(COPY "${SDL2_SOURCE_NAME}/include/" DESTINATION "${MACE_INCLUDES}/SDL")
	
	message(STATUS "SDL2 Includes added!")
else()
	message(STATUS "Found SDL2 includes.")
endif()

if(NOT EXISTS "${SDL_NET_SOURCE_DIR}")
	if(NOT EXISTS "${SDL_NET_DOWNLOAD_PATH}")
		message(STATUS "Download SDL_net...")

		file(DOWNLOAD "${SDL_NET_URL}" "${SDL_NET_DOWNLOAD_PATH}")
	else()
		message(STATUS "Found SDL_net source!")
	endif()
	
	message(STATUS "Extracting SDL_net source...")
	#Next we extract the .tar.gz. This works on Windows as well
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xvf ${SDL_NET_DOWNLOAD_PATH}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
		
	#If the extraction fails, go to here.
	if(NOT EXISTS ${SDL_NET_SOURCE_DIR})
		message(FATAL_ERROR "Failed to extract SDL_net source! Please retry or file a bug report.")
	else()
		message(STATUS "SDL_net extracted.")
	endif()
		
	#Remove the extracted tar. We dont need the original anymore
	file(REMOVE ${SDL_NET_DOWNLOAD_PATH})

else()
	message(STATUS "Found SDL_net source")
endif()

if(NOT EXISTS "${SDL2_INCLUDES}/SDL_net.h")
	message(STATUS "Moving SDL_net headers to include/SDL directory... ")
	file(COPY "${SDL_NET_SOURCE_DIR}/SDLnetsys.h" DESTINATION "${SDL2_INCLUDES}")
	file(COPY "${SDL_NET_SOURCE_DIR}/SDL_net.h" DESTINATION "${SDL2_INCLUDES}")

	message(STATUS "SDL_net Includes added!")
else()
	message(STATUS "Found SDL_net includes.")
endif()

#Catch is for unit testing.

if (NOT EXISTS "${CATCH_DOWNLOAD_PATH}")
	#Catch is one header file, so its easy to fix. Just download it!
	message(STATUS "Downloading Catch..")
    file(DOWNLOAD "${CATCH_URL}" "${CATCH_DOWNLOAD_PATH}")
	message(STATUS "Done downloading Catch.")
else()
	message(STATUS "Catch found.")
endif()

if(NOT EXISTS "${SDL_NET_SOURCE_DIR}/CMakeLists.txt")
	message(STATUS "SDL_net doesn't have a CMakeLists! Adding one...")
	
	file(COPY "${CMAKE_SOURCE_DIR}/${SDL_NET_CMAKE}" DESTINATION "${SDL_NET_SOURCE_DIR}")
	
	file(RENAME "${SDL_NET_SOURCE_DIR}/${SDL_NET_CMAKE}" "${SDL_NET_SOURCE_DIR}/CMakeLists.txt")
else()
	message(STATUS "SDL_net has a CMakeLists")
endif()

message(STATUS "Finding OpenGL...")
find_package(OpenGL REQUIRED)

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