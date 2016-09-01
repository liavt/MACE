cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(MACE CXX)

message("MACE project added")

#For the installation of other projects
include(ExternalProject)
include(FindPackageHandleStandardArgs)
#for glfw
set(LIB_SUFFIX ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#This defines where all of our includes are located
set(MACE_INCLUDES "${PROJECT_SOURCE_DIR}/include")

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
set(GLFW_DOWNLOAD_PATH "${PROJECT_SOURCE_DIR}/${GLFW_SOURCE_NAME}.tar.gz")
#This is where the source will be located in
set(GLFW_DOWNLOAD_SOURCE_DIR "${PROJECT_SOURCE_DIR}/${GLFW_SOURCE_NAME}")
#This is where the headers for GLFW will be located in
set(CATCH_URL "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp")
#This is where Catch should be located in
set(CATCH_DOWNLOAD_PATH "${MACE_INCLUDES}/Catch.h")

SET(GLEW_NAME "glew-1.13.0")
#this is the url to download GLEW from
set(GLEW_URL "https://sourceforge.net/projects/glew/files/glew/1.13.0/${GLEW_NAME}.tgz/download")
#if glew needs downloading, this is where it willbe downloaded to
set(GLEW_DOWNLOAD_PATH "${PROJECT_SOURCE_DIR}/${GLEW_NAME}.tar.gz")
#where the downloading tarbell will be extracted to
set(GLEW_DOWNLOAD_SOURCE_DIR "${PROJECT_SOURCE_DIR}/${GLEW_NAME}")

#stb_image is a single header only image loading library.
set(STB_IMAGE_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")
set(STB_IMAGE_DOWNLOAD_PATH "${MACE_INCLUDES}/stb_image.h")

if(${MACE_GRAPHICS} AND NOT ${MACE_WINDOW})
	message(FATAL_ERROR "MC-Graphics requires MC-Window! Please enable MC-Window and retry.")
endif()

# Try to find GLFW library and include path.
# Once done this will define
#
# GLFW_FOUND
# GLFW_INCLUDE_DIR
# GLFW_LIBRARY
#
message(STATUS "Finding GLFW...")
if (WIN32)
    find_path( GLFW_INCLUDE_DIR
        NAMES
            GLFW/glfw3.h
        PATHS
            ${PROJECT_SOURCE_DIR}/shared_external/glfw/include
            ${PROJECT_SOURCE_DIR}/../shared_external/glfw/include
            ${GLFW_LOCATION}/include
            $ENV{GLFW_LOCATION}/include
            $ENV{PROGRAMFILES}/GLFW/include
            ${GLFW_LOCATION}
            $ENV{GLFW_LOCATION}
            DOC "The directory where GLFW/glfw3.h resides" )
    if(ARCH STREQUAL "x86")
      find_library( GLFW_LIBRARY
          NAMES
              glfw3
          PATHS
              ${GLFW_LOCATION}/lib
              $ENV{GLFW_LOCATION}/lib
              $ENV{PROGRAMFILES}/GLFW/lib
              DOC "The GLFW library")
    else()
      find_library( GLFW_LIBRARY
          NAMES
              glfw3
          PATHS
              ${GLFW_LOCATION}/lib
              $ENV{GLFW_LOCATION}/lib
              $ENV{PROGRAMFILES}/GLFW/lib
              DOC "The GLFW library")
    endif()
endif ()

if (${CMAKE_HOST_UNIX})
    find_path( GLFW_INCLUDE_DIR
        NAMES
            GLFW/glfw3.h
        PATHS
            ${GLFW_LOCATION}/include
            $ENV{GLFW_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
            DOC "The directory where GLFW/glfw3.h resides"
    )
    find_library( GLFW_LIBRARY
        NAMES
            glfw3 glfw
        PATHS
            ${GLFW_LOCATION}/lib
            $ENV{GLFW_LOCATION}/lib
            /usr/lib64
            /usr/lib
            /usr/local/lib64
            /usr/local/lib
            /sw/lib
            /opt/local/lib
            /usr/lib/x86_64-linux-gnu
            NO_DEFAULT_PATH
            DOC "The GLFW library")
endif ()

find_package_handle_standard_args(GLFW DEFAULT_MSG
    GLFW_INCLUDE_DIR
    GLFW_LIBRARY
)

mark_as_advanced( GLFW_FOUND )

if(NOT ${GLFW_FOUND})
	message(FATAL_ERROR "GLFW not found!")
endif()

find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

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


include_directories(${GLEW_INCLUDE_DIR})
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${MACE_INCLUDES})

link_libraries(${GLFW_LIBRARY})
link_libraries(${GLEW_LIBRARY})
link_libraries(${OPENGL_LIBRARY})


message(STATUS "Done! Thank you for using MACE.")
