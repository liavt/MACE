cmake_minimum_required(VERSION 2.8.11)

project(MACE CXX)

message("MACE project added")

#For the installation of other projects
include(ExternalProject)


#all the variables are now being set here
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
#for glfw
set(LIB_SUFFIX ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})


#This defines where all of our includes are located
set(MACE_INCLUDES "${CMAKE_SOURCE_DIR}/include")

set(MACE_WINDOW ON CACHE BOOL "Whether to build the windowing library under MC-Window.")
set(MACE_GRAPHICS ON CACHE BOOL "Whether to build the graphics library under MC-Graphics. MACE_WINDOW is required for this.")
set(MACE_AUDIO ON CACHE BOOL "Whether to build the audio library MC-Audio.")
set(MACE_NETWORK ON CACHE BOOL "Whether to build the networking library under MC-Network.")
set(MACE_UNIT_TESTING OFF CACHE BOOL "Whether to build the unit tests under MC-Testing.")
set(MACE_DOWNLOAD_DEPENDENCIES ON CACHE BOOL "Whether to download and extract dependencies that are not found. If turnd off, will instead error.")
set(MACE_STATIC_LIBRARY ON CACHE BOOL "Whether to build static library files")

#Source name is the version to use
set(GLFW_SOURCE_NAME "glfw-3.2")

set(GLFW_URL "http://downloads.sourceforge.net/project/glfw/glfw/3.2/${GLFW_SOURCE_NAME}.tar.gz?r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fglfw%2Ffiles%2Fglfw%2F3.2%2F&ts=1470492725&use_mirror=heanet")
set(GLFW_DOWNLOAD_PATH "${CMAKE_SOURCE_DIR}/${GLFW_SOURCE_NAME}.tar.gz")
#This is where the source will be located in
set(GLFW_SOURCE_DIR "${CMAKE_SOURCE_DIR}/${GLFW_SOURCE_NAME}")
#This is where the headers for GLFW will be located in
set(GLFW_INCLUDES "${MACE_INCLUDES}/GLFW")
set(GLFW_LIBRARY_NAME "glfw3")
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

#stb_image is a single header only image loading library.
set(STB_IMAGE_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")
set(STB_IMAGE_DOWNLOAD_PATH "${MACE_INCLUDES}/stb_image.h")

if(${MACE_GRAPHICS} AND NOT ${MACE_WINDOW})
	message(FATAL_ERROR "MC-Graphics requires MC-Window! Please enable MC-Window and retry.")
endif()


if(NOT EXISTS ${GLFW_SOURCE_DIR})
	if(${MACE_DOWNLOAD_DEPENDENCIES})
		message(STATUS "GLFW source not found. Fixing..")
		#First we check if the .tar.gz is downloaded. There may be chance it is already downloaded and not extracted
		if (NOT EXISTS "${GLFW_DOWNLOAD_PATH}")
			message(STATUS "Downloading GLFW source...")
			file(DOWNLOAD "${GLFW_URL}" "${GLFW_DOWNLOAD_PATH}")
		else()
			message(STATUS "GLFW source found in tar.gz")
		endif()

		message(STATUS "Extracting GLFW source...")
		#Next we extract the .tar.gz. This works on Windows as well
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E tar xvf ${GLFW_DOWNLOAD_PATH}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		)
		
		#If the extraction fails, go to here.
		if(NOT EXISTS ${GLFW_SOURCE_DIR})
			message(FATAL_ERROR "Failed to extract GLFW source! Please retry or file a bug report.")
		else()
			message(STATUS "GLFW extracted.")
		endif()
		
		#Remove the extracted tar. We dont need the original anymore
		file(REMOVE ${GLFW_DOWNLOAD_PATH})
		message(STATUS "Deleted temporary tar")
		message(STATUS "Done adding ${GLFW_SOURCE_NAME}")
	else()
		message(FATAL_ERROR "GLFW source not found!")
	endif()
else()
	message(STATUS "GLFW found.")
endif()


#Just because we download the source doesnt mean we can just use the headers! We have to move them to our include directory.
if(NOT EXISTS ${GLFW_INCLUDES})
	message(STATUS "GLFW includes not found. Fixing...")
	
	#Simply move the headers from the source into our directory
	file(COPY "${GLFW_SOURCE_NAME}/include/" DESTINATION "${MACE_INCLUDES}")
	
	message(STATUS "GLFW Includes added!")
else()
	message(STATUS "Found GLFW includes.")
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

if(${MACE_WINDOW})
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
			if(NOT EXISTS ${GLEW_DOWNLOAD_SOURCE_DIR})
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
				
				file(REMOVE ${GLEW_DOWNLOAD_PATH})
			else()
				message(STATUS "Found GLEW source directory")
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

if(${MACE_GRAPHICS})		
	if (NOT EXISTS "${STB_IMAGE_DOWNLOAD_PATH}")
		if(${MACE_DOWNLOAD_DEPENDENCIES})
			#Catch is one header file, so its easy to fix. Just download it!
			message(STATUS "Downloading STB_IMAGE..")
			file(DOWNLOAD "${STB_IMAGE_URL}" "${STB_IMAGE_DOWNLOAD_PATH}")
			message(STATUS "Done downloading STB_IMAGE.")
		else()
			message(FATAL_ERROR "STB_IMAGE is not  found!")
		endif()
	else()
		message(STATUS "STB_IMAGE found.")
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

#[[
ExternalProject_Add(
    ${GLFW_LIBRARY_NAME}
    URL "https://github.com/glfw/glfw/archive/3.1.tar.gz"
    URL_HASH SHA1=fe17a0610a239311a726ecabcd2dbd669fb24ca8
    CMAKE_ARGS -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY} -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -DCMAKE_BINARY_OUTPUT_DIRECTORY=${CMAKE_BINARY_OUTPUT_DIRECTORY}
	BINARY_DIR ${CMAKE_BINARY_OUTPUT_DIRECTORY}
)]]

#find_package(GLFW REQUIRED)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)

message(STATUS "Adding ${GLFW_SOURCE_NAME}...")
add_subdirectory(${GLFW_SOURCE_NAME})

add_subdirectory(MC-System)
if(${MACE_WINDOW})
	add_subdirectory(MC-Window)
endif()
if(${MACE_GRAPHICS})
	add_subdirectory(MC-Graphics)
endif()
if(${MACE_NETWORK})
	add_subdirectory(MC-Network)
endif()
if(${MACE_AUDIO})
	add_subdirectory(MC-Audio)
endif()
if(${MACE_UNIT_TESTING})
	add_subdirectory(MC-Testing)
endif()

add_subdirectory(MC-Test)

message(STATUS "Done! Thank you for using MACE.")