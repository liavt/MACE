cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(MACE CXX)

message("MACE project added")

#On unix systems this language is requied to compile GLFW and GLEW
enable_language(C)

include(FindPackageHandleStandardArgs)
include(CheckCXXCompilerFlag)

option(MACE_UNIT_TESTING "Whether to build the unit tests under MACE/Testing." OFF)
option(MACE_DOWNLOAD_DEPENDENCIES "Whether to download and extract dependencies that are not found. If turned off, will instead error." ON)
mark_as_advanced(MACE_DOWNLOAD_DEPENDENCIES)
option(MACE_STATIC_LIBRARY "Whether to build static library files" ON)

#for glfw
set(LIB_SUFFIX ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#This defines where all of our includes are located
set(MACE_INCLUDES "${PROJECT_SOURCE_DIR}/include")

#Source name is the version to use
set(GLFW_SOURCE_NAME "glfw-3.2")

set(GLFW_URL "http://downloads.sourceforge.net/project/glfw/glfw/3.2/${GLFW_SOURCE_NAME}.tar.gz?r=https%3A%2F%2Fsourceforge.net%2Fprojects%2Fglfw%2Ffiles%2Fglfw%2F3.2%2F&ts=1470492725&use_mirror=heanet")

set(GLEW_SOURCE_NAME "glew-1.13.0")
#this is the url to download GLEW from
set(GLEW_URL "https://sourceforge.net/projects/glew/files/glew/1.13.0/${GLEW_SOURCE_NAME}.tgz/download")

set(CATCH_URL "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp")
#This is where Catch should be located in
set(CATCH_DOWNLOAD_PATH "${MACE_INCLUDES}/Catch.h")

#stb_image is a single header only image loading library.
set(STB_IMAGE_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")
set(STB_IMAGE_DOWNLOAD_PATH "${MACE_INCLUDES}/stb_image.h")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(download_library name url)
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.tar.gz")
			if(NOT ${MACE_DOWNLOAD_DEPENDENCIES})
				message(FATAL_ERROR "${name} NOT FOUND")
			endif()
			message(STATUS "Downloading ${CMAKE_CURRENT_SOURCE_DIR}/${name} from ${url}")
			file(DOWNLOAD ${url} ${PROJECT_SOURCE_DIR}/${name}.tar.gz TIMEOUT 10)
		endif()
		
		file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/${name})
		
		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${name}.tar.gz WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	endif()
	
	message(STATUS "Found ${name} in source directory")
endfunction()

macro(get_library name)
	if(${name} STREQUAL "glfw3" OR ${name} STREQUAL "${GLFW_SOURCE_NAME}")
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
	elseif(${name} STREQUAL "glew" OR ${name} STREQUAL "${GLEW_SOURCE_NAME}")
		if (WIN32)
			find_path( GLEW_INCLUDE_DIR
				NAMES
					GL/glew.h
				PATHS
					${GLEW_LOCATION}/include
					$ENV{GLEW_LOCATION}/include
					$ENV{PROGRAMFILES}/GLEW/include
					${PROJECT_SOURCE_DIR}/extern/glew/include
					${GLEW_LOCATION}
					$ENV{GLEW_LOCATION}
					DOC "The directory where GL/glew.h resides" )
			find_file( GLEW_SOURCE
				NAMES
					glew.c
				PATHS
					${GLEW_LOCATION}/src
					$ENV{GLEW_LOCATION}/src
					$ENV{PROGRAMFILES}/GLEW/src
					${PROJECT_SOURCE_DIR}/extern/glew/src
					${GLEW_LOCATION}
					$ENV{GLEW_LOCATION}
					DOC "The directory where GL/glew.c resides" )
			if(ARCH STREQUAL "x86")
				find_library( GLEW_LIBRARY
					NAMES
						glew GLEW glew32s glew32
					PATHS
					${GLEW_LOCATION}/lib
					${GLEW_LOCATION}/lib/x86
					${GLEW_LOCATION}/lib/win32
						${GLEW_LOCATION}/lib/Release/win32
						${GLEW_LOCATION}/lib/Release MX/win32
						$ENV{GLEW_LOCATION}/lib
						$ENV{GLEW_LOCATION}/lib/Release/win32
						$ENV{GLEW_LOCATION}/lib/Release MX/win32
						$ENV{GLEW_LOCATION}/lib/x86
						$ENV{GLEW_LOCATION}/lib/win32
						$ENV{PROGRAMFILES}/GLEW/lib
						$ENV{PROGRAMFILES}/GLEW/lib/x86
						$ENV{PROGRAMFILES}/GLEW/lib/win32
						${PROJECT_SOURCE_DIR}/extern/glew/bin
						${PROJECT_SOURCE_DIR}/extern/glew/lib
						${PROJECT_SOURCE_DIR}/extern/glew/lib/x86
						${PROJECT_SOURCE_DIR}/extern/glew/lib/win32
						${GLEW_LOCATION}
						$ENV{GLEW_LOCATION}
						DOC "The GLEW library")
			else()
				find_library( GLEW_LIBRARY
					NAMES
						glew GLEW glew32s glew32
					PATHS
						${GLEW_LOCATION}/lib/x64
						${GLEW_LOCATION}/lib/Release/x64
						${GLEW_LOCATION}/lib/Release MX/x64
						$ENV{GLEW_LOCATION}/lib/x64
						$ENV{GLEW_LOCATION}/lib/Release/x64
						$ENV{GLEW_LOCATION}/lib/Release MX/x64
						$ENV{PROGRAMFILES}/GLEW/lib/x64
						${PROJECT_SOURCE_DIR}/extern/glew/bin
						${PROJECT_SOURCE_DIR}/extern/glew/lib/x64
						${GLEW_LOCATION}/lib
						$ENV{GLEW_LOCATION}/lib
						$ENV{PROGRAMFILES}/GLEW/lib
						${PROJECT_SOURCE_DIR}/extern/glew/lib
						${GLEW_LOCATION}
						$ENV{GLEW_LOCATION}
						DOC "The GLEW library")
			endif()
		endif ()

		if (${CMAKE_HOST_UNIX})
			find_path( GLEW_INCLUDE_DIR
				NAMES
					GL/glew.h
				PATHS
					${GLEW_LOCATION}/include
					$ENV{GLEW_LOCATION}/include
					/usr/include
					/usr/local/include
					/sw/include
					/opt/local/include
					NO_DEFAULT_PATH
					DOC "The directory where GL/glew.h resides"
			)
			find_library( GLEW_LIBRARY
				NAMES
					GLEW glew
				PATHS
					${GLEW_LOCATION}/lib
					$ENV{GLEW_LOCATION}/lib
					/usr/lib64
					/usr/lib
					/usr/local/lib64
					/usr/local/lib
					/sw/lib
					/opt/local/lib
					NO_DEFAULT_PATH
					DOC "The GLEW library")
		endif ()

		if (GLEW_INCLUDE_DIR AND EXISTS "${GLEW_INCLUDE_DIR}/GL/glew.h")

			file(STRINGS "${GLEW_INCLUDE_DIR}/GL/glew.h" GLEW_4_2 REGEX "^#define GL_VERSION_4_2.*$")
			if (GLEW_4_2)
				SET(OPENGL_4_2_FOUND TRUE)
			else ()
				message(WARNING
				"glew-1.7.0 or newer needed for supporting OpenGL 4.2 dependent features"
				)
			endif ()

			file(STRINGS "${GLEW_INCLUDE_DIR}/GL/glew.h" GLEW_4_3 REGEX "^#define GL_VERSION_4_3.*$")
			if (GLEW_4_3)
				SET(OPENGL_4_3_FOUND TRUE)
			else ()
				message(WARNING
				"glew-1.9.0 or newer needed for supporting OpenGL 4.3 dependent features"
				)
			endif ()

		endif ()

		if(GLEW_SOURCE)
		find_package_handle_standard_args(GLEW DEFAULT_MSG
			GLEW_INCLUDE_DIR
			GLEW_SOURCE
		)
		else()
		find_package_handle_standard_args(GLEW DEFAULT_MSG
			GLEW_INCLUDE_DIR
			GLEW_LIBRARY
		)
		endif()

		mark_as_advanced( GLEW_FOUND )
	else()
		find_package(${name})
	endif()
endmacro()

find_package(OpenGL REQUIRED)

get_library("glfw-3.2" ${GLFW_URL})
get_library("glew" ${GLEW_URL})

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
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	message(STATUS "Found C++ 11 support in ${CMAKE_CXX_COMPILER}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	message(STATUS "Found C++ 0X support in ${CMAKE_CXX_COMPILER}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++ 11 support. Please use a different C++ compiler.")
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

if(NOT ${GLFW_FOUND})
	download_library(${GLFW_SOURCE_NAME} ${GLFW_URL})
	add_subdirectory(${PROJECT_SOURCE_DIR}/${GLFW_SOURCE_NAME})
	
	set(GLFW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/${GLFW_SOURCE_NAME}/include)
	set(GLFW_LIBRARY glfw)
endif()

if(NOT ${GLEW_FOUND})
	download_library(${GLEW_SOURCE_NAME} ${GLEW_URL})
	add_subdirectory(${PROJECT_SOURCE_DIR}/${GLEW_SOURCE_NAME}/build/cmake)
	
	set(GLEW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/${GLEW_SOURCE_NAME}/include)
	set(GLEW_LIBRARY glew)
endif()

target_include_directories(MACE PUBLIC ${GLFW_INCLUDE_DIR})
target_link_libraries(MACE LINK_PUBLIC ${GLFW_LIBRARY})

target_include_directories(MACE PUBLIC ${GLEW_INCLUDE_DIR})
target_link_libraries(MACE LINK_PUBLIC ${GLEW_LIBRARY})

target_include_directories(MACE PUBLIC ${MACE_INCLUDES})

target_link_libraries(MACE LINK_PUBLIC ${OPENGL_LIBRARY})