cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(MACE CXX)

message("MACE project added")

enable_language(C)

#For the installation of other projects
include(ExternalProject)
include(FindPackageHandleStandardArgs)
#for glfw
set(LIB_SUFFIX ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#This defines where all of our includes are located
set(MACE_INCLUDES "${PROJECT_SOURCE_DIR}/include")

set(MACE_UNIT_TESTING OFF CACHE BOOL "Whether to build the unit tests under MACE/Testing.")
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

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MACE/Window/*.h" "${MACE_INCLUDES}/MACE/Graphics/*.h" "${MACE_INCLUDES}/MACE/Audio/*.h" "${MACE_INCLUDES}/MACE/Network/*.h" "${MACE_INCLUDES}/MACE/System/*.h")
file(GLOB_RECURSE SRC "src/*.cpp")

foreach(FILE ${HEADERS}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "Source Files${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "Header Files${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

foreach(FILE ${SRC}) 
    # Get the directory of the source file
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    # Group into "Source Files" and "Header Files"
    if ("${FILE}" MATCHES ".*\\.cpp")
       set(GROUP "Source Files${GROUP}")
    elseif("${FILE}" MATCHES ".*\\.h")
       set(GROUP "Header Files${GROUP}")
    endif()

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

if(${MACE_STATIC_LIBRARY})
	add_library(MACE STATIC ${HEADERS} ${SRC})
else()
	add_library(MACE SHARED ${HEADERS} ${SRC})
endif()

if(${MACE_UNIT_TESTING})
	file(GLOB_RECURSE TESTS_SRC "tests/*.cpp")
	add_executable(MACE-Testing ${TESTS_SRC})
	
	target_link_libraries(MACE-Testing LINK_PUBLIC MACE)
endif()


target_include_directories(MACE PUBLIC ${GLEW_INCLUDE_DIR})
target_include_directories(MACE PUBLIC ${GLFW_INCLUDE_DIR})
target_include_directories(MACE PUBLIC ${MACE_INCLUDES})

target_link_libraries(MACE LINK_PUBLIC ${GLEW_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${GLFW_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${OPENGL_LIBRARY})


message(STATUS "Done! Thank you for using MACE.")
