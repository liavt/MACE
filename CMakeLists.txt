cmake_minimum_required(VERSION 2.8.11)

project(MACE)

message("MACE project added")

#For the installation of other projects
include(ExternalProject)


#all the variables are now being set here

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#This defines where all of our includes are located
set(MACE_INCLUDES "${CMAKE_SOURCE_DIR}/include")

set(MACE_WINDOW ON CACHE BOOL "Whether to build the windowing library under MC-Window.")
set(MACE_GRAPHICS ON CACHE BOOL "Whether to build the graphics library under MC-Graphics. MACE_WINDOW is required for this.")
set(MACE_AUDIO ON CACHE BOOL "Whether to build the audio library MC-Audio.")
set(MACE_NETWORK ON CACHE BOOL "Whether to build the networking library under MC-Network.")
set(MACE_UNIT_TESTING OFF CACHE BOOL "Whether to build the unit tests under MC-Testing.")
set(MACE_DOWNLOAD_DEPENDENCIES ON CACHE BOOL "Whether to download and extract dependencies that are not found. If turnd off, will instead error.")

set(SDL_NET_SOURCE_NAME "SDL2_net-2.0.1")
set(SDL_NET_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${SDL_NET_SOURCE_NAME}")
set(SDL_NET_URL "https://www.libsdl.org/projects/SDL_net/release/${SDL_NET_SOURCE_NAME}.tar.gz")
set(SDL_NET_DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/${SDL_NET_SOURCE_NAME}.tar.gz")
#SDL_Net doesnt have a Cmake. So we made our own.
set(SDL_NET_CMAKE "SDLNet_CMake.cmake")

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

SET(GLEW_NAME "glew-1.13.0")
#this is the url to download GLEW from
set(GLEW_URL "https://sourceforge.net/projects/glew/files/glew/1.13.0/${GLEW_NAME}.tgz/download")
#if glew needs downloading, this is where it willbe downloaded to
set(GLEW_DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/${GLEW_NAME}.tar.gz")
#where the downloading tarbell will be extracted to
set(GLEW_DOWNLOAD_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${GLEW_NAME}")

if(${MACE_GRAPHICS} AND NOT ${MACE_WINDOW})
	message(FATAL_ERROR "MC-Graphics requires MC-Window! Please enable MC-Window and retry.")
endif()

if(NOT EXISTS ${SDL2_SOURCE_DIR})
	if(${MACE_DOWNLOAD_DEPENDENCIES})
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
		message(FATAL_ERROR "SDL2 source not found!")
	endif()
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

if(${MACE_NETWORK})
	if(NOT EXISTS "${SDL_NET_SOURCE_DIR}")
		if(${MACE_DOWNLOAD_DEPENDENCIES})
		
			if(NOT EXISTS "${SDL_NET_DOWNLOAD_PATH}")
				message(STATUS "Downloading SDL_net...")

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
				message(FATAL_ERROR "Failed to extract SDL_net source!")
			else()
				message(STATUS "SDL_net extracted.")
			endif()
				
			#Remove the extracted tar. We dont need the original anymore
			file(REMOVE ${SDL_NET_DOWNLOAD_PATH})
		else()
			message(FATAL_ERROR "SDL_net source not found!")
		endif()
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
	
	if(NOT EXISTS "${SDL_NET_SOURCE_DIR}/CMakeLists.txt")
		message(STATUS "SDL_net doesn't have a CMakeLists! Adding one...")
		
		file(COPY "${CMAKE_SOURCE_DIR}/${SDL_NET_CMAKE}" DESTINATION "${SDL_NET_SOURCE_DIR}")
		
		file(RENAME "${SDL_NET_SOURCE_DIR}/${SDL_NET_CMAKE}" "${SDL_NET_SOURCE_DIR}/CMakeLists.txt")
	else()
		message(STATUS "SDL_net has a CMakeLists")
	endif()
endif()

#Catch is for unit testing.
if(${MACE_UNIT_TESTING})
	if (NOT EXISTS "${CATCH_DOWNLOAD_PATH}")
		if(${MACE_DOWNLOAD_DEPENDENCIES})
			#Catch is one header file, so its easy to fix. Just download it!
			message(STATUS "Downloading Catch..")
			file(DOWNLOAD "${CATCH_URL}" "${CATCH_DOWNLOAD_PATH}")
			message(STATUS "Done downloading Catch.")
		else()
			message(FATAL_ERROR "Catch is not  found!")
		endif()
	else()
		message(STATUS "Catch found.")
	endif()
endif()

if(${MACE_GRAPHICS})		
	message(STATUS "Finding GLEW...")
	if(${MACE_DOWNLOAD_DEPENDENCIES})
		find_package(GLEW)
	else()
		find_package(GLEW REQUIRED)
	endif()
	
	if(NOT GLEW_FOUND)
		if(${MACE_DOWNLOAD_DEPENDENCIES})
		#	ExternalProject_Add(${GLEW_NAME} URL ${GLEW_URL} TIMEOUT 10 BINARY_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" SOURCE_DIR ${GLEW_SOURCE}/build/cmake/)
			#set(GLEW_LIBRARY "${GLEW_SOURCE}")
			#message(STATUS "Downloaded GLEW to ${GLEW_PROJECT_DIR}")
			if(NOT EXISTS ${GLEW_DOWNLOAD_PATH})
				message(STATUS "GLEW not found! Downloading...")
				file(DOWNLOAD "${GLEW_URL}" "${GLEW_DOWNLOAD_PATH}")
				
				if(NOT EXISTS ${GLEW_DOWNLOAD_PATH})
					message(FATAL_ERROR "Error downloading GLEW! Check your internet connection!")
				endif()
			else()
				message(STATUS "GLEW source found.")
			endif()
			
			if(NOT EXISTS ${GLEW_DOWNLOAD_SOURCE_DIR})
				message(STATUS "Extracting GLEW..")
				execute_process(
					COMMAND ${CMAKE_COMMAND} -E tar xvf ${GLEW_DOWNLOAD_PATH}
					WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
				)
				
				if(NOT EXISTS ${GLEW_DOWNLOAD_SOURCE_DIR})
					message(FATAL_ERROR "Error occurred during extraction of GLEW!")
				endif()
			else()
				message(STATUS "GLEW source directory found.")
			endif()
				
			message(STATUS "Adding GLEW")
			set(GLEW_LIBRARY "${GLEW_DOWNLOAD_SOURCE_DIR}/lib")
			set(GLEW_INCLUDE_DIR "${GLEW_DOWNLOAD_SOURCE_DIR}/include")
		
			add_subdirectory("${GLEW_DOWNLOAD_SOURCE_DIR}/build/cmake")
		else()
			message(FATAL_ERROR "GLEW not found! Please install and retry")
		endif()
	else()
		message(STATUS "Found GLEW")
	endif()
endif()

#verify that we are using C++ 11
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	message(STATUS "Found CXX11 support in ${CMAKE_CXX_COMPILER}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	message(STATUS "Found CXX0X support in ${CMAKE_CXX_COMPILER}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

message(STATUS "Adding ${SDL2_SOURCE_NAME}...")
add_subdirectory("${SDL2_SOURCE_NAME}")


add_subdirectory(MC-System)
if(${MACE_WINDOW})
	add_subdirectory(MC-Window)
endif()
if(${MACE_GRAPHICS})
	add_subdirectory(MC-Graphics)
endif()
if(${MACE_NETWORK})
	add_subdirectory("${SDL_NET_SOURCE_DIR}" )
	add_subdirectory(MC-Network)
endif()
if(${MACE_AUDIO})
	add_subdirectory(MC-Audio)
endif()
if(${MACE_UNIT_TESTING})
	add_subdirectory(MC-Testing)
endif()

message(STATUS "Done! Thank you for using MACE.")