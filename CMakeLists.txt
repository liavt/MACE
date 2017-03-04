#The MIT License (MIT)
#
#Copyright (c) 2016 Liav Turkia
#
#Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(MACE CXX)

if(COMMAND cmake_policy)
	cmake_policy(VERSION 2.8.12)
	if(POLICY CMP0017)
		cmake_policy(SET CMP0017 NEW)
	endif()
	
	if(POLICY CMP0025)
		cmake_policy(SET CMP0025 OLD)
	endif()
endif()

message("MACE project added")

#On unix systems this line is requied to compile GLFW and GLEW
enable_language(C)

set(CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules" "${PROJECT_SOURCE_DIR}/cmake-modules")

include(FindPackageHandleStandardArgs)
include(CheckCXXCompilerFlag)
include(DownloadLibrary)
include(CheckIncludeFiles)
include(TestBigEndian)
include(CheckTypeSize)

option(MACE_TESTS "Whether to build the unit tests under /tests." OFF)
option(MACE_DEMOS "Whether to build the demo programs under /demos" OFF)
option(MACE_DOWNLOAD_DEPENDENCIES "Whether to download and extract dependencies that are not found. If turned off, will instead error." ON)
mark_as_advanced(MACE_DOWNLOAD_DEPENDENCIES)

#for glfw
set(LIB_SUFFIX ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

#This defines where all of our includes are located
set(MACE_INCLUDES "${PROJECT_SOURCE_DIR}/include")

set(GLFW_SOURCE_VERSION "3.0.4")
set(GLFW_SOURCE_NAME "glfw-${GLFW_SOURCE_VERSION}")
set(GLFW_URL "https://github.com/glfw/glfw/archive/${GLFW_SOURCE_VERSION}.tar.gz")

set(OPENAL_SOURCE_VERSION "1.17.2")
set(OPENAL_SOURCE_NAME "openal-soft-openal-soft-${OPENAL_SOURCE_VERSION}")
set(OPENAL_URL "https://github.com/kcat/openal-soft/archive/openal-soft-${OPENAL_SOURCE_VERSION}.tar.gz")

set(GLEW_SOURCE_VERSION "2.0.0")
set(GLEW_SOURCE_NAME "glew-${GLEW_SOURCE_VERSION}")
set(GLEW_URL "https://sourceforge.net/projects/glew/files/glew/${GLEW_SOURCE_VERSION}/${GLEW_SOURCE_NAME}.tgz/download")

set(FREETYPE_SOURCE_VERSION "2.5.2")
set(FREETYPE_SOURCE_NAME "freetype-${FREETYPE_SOURCE_VERSION}")
set(FREETYPE_URL "http://download.savannah.gnu.org/releases/freetype/${FREETYPE_SOURCE_NAME}.tar.gz")

set(CATCH_URL "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp")
#This is where Catch should be located in
set(CATCH_DOWNLOAD_PATH "${MACE_INCLUDES}/Catch.h")

#stb_image is a single header only image loading library.
set(STB_IMAGE_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")
set(STB_IMAGE_DOWNLOAD_PATH "${MACE_INCLUDES}/stb_image.h")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenGL REQUIRED)
find_package(GLEW ${GLEW_SOURCE_VERSION})
find_package(GLFW ${GLFW_SOURCE_VERSION})
find_package(Freetype ${FREETYPE_SOURCE_VERSION})
find_package(OpenAL)

find_package(OpenCV)

if(${OpenCV_FOUND})
	set(MACE_OPENCV 1)
	
	target_link_libraries(MACE LINK_PUBLIC ${OpenCV_LIBRARIES})
	target_include_directories(MACE PUBLIC ${OpenCV_INCLUDE_DIRS})
endif()

#Checks
check_include_files("unistd.h" MACE_POSIX)
check_include_files("windows.h" MACE_WINAPI)

test_big_endian(MACE_BIG_ENDIAN)

#Macros for configuration
if(APPLE)
	set(MACE_OSX 1)
endif()

if(UNIX)
	set(MACE_UNIX 1)
endif()

if(WIN32)
	set(MACE_WINDOWS 1)
endif()

if(BUILD_SHARED_LIBS)
	set(MACE_SHARED_LIBS 1)
else()
	set(MACE_STATIC_LIBS 1)
endif()

if(MSYS)
	set(MACE_MINGW 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR MSVC)
	set(MACE_MSVC 1)

	if(CMAKE_CL_64)
		set(MACE_MSVC_CL 1)
	endif()

	if(CMAKE_COMPILER_2005)
		set(MACE_MSVC_2005 1)
	endif()
	
	if(MSVC_IDE)
		set(MACE_MSVC_IDE 1)
	endif()
elseif(CYGWIN)
	set(MACE_CYGWIN 1)
elseif(BORLAND)
	set(MACE_BORLAND 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
	set(MACE_CLANG 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_COMPILER_IS_GNUXX)
	set(MACE_GNU 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "HP")
	set(MACE_HP 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
	set(MACE_INTEL 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "SunPro")
	set(MACE_ORACLE 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "TI_DSP")
	set(MACE_TEXAS_INSTRUMENTS 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Watcom" OR WATCOM)
	set(MACE_WATCOM 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "ADSP")
	set(MACE_ANALOG_VISUAL 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Cray")
	set(MACE_CRAY 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MIPSpro")
	set(MACE_MIPSPRO 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "PGI")
	set(MACE_PORTLAND 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "PathScale")
	set(MACE_PATHSCALE 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "XL" OR CMAKE_CXX_COMPILER_ID MATCHES "VisualAge" OR CMAKE_CXX_COMPILER_ID MATCHES "zOS")
	set(MACE_IBM 1)
endif()

set(MACE_DIRECTORY "${PROJECT_SOURCE_DIR}")

set(MACE_SYSTEM "${CMAKE_SYSTEM_NAME}")
set(MACE_SYSTEM_VERSION "${CMAKE_SYSTEM_VERSION}")
set(MACE_PROCESSOR "${CMAKE_SYSTEM_PROCESSOR}")

if("${CMAKE_BUILD_TYPE}" MATCHES "Debug")
	set(MACE_DEBUG 1)
endif()

configure_file("${PROJECT_SOURCE_DIR}/Configure.h.in" "${CMAKE_BINARY_DIR}/MACE-Configure.h" ESCAPE_QUOTES @ONLY)

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

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MACE/*.h"  "${MACE_INCLUDES}/MACE/Graphics/*.h" "${MACE_INCLUDES}/MACE/System/*.h" "${MACE_INCLUDES}/MACE/Utility/*.h")
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

add_library(MACE ${HEADERS} ${SRC})

if(${MACE_TESTS})
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

	file(GLOB_RECURSE TESTS_SRC "tests/*.cpp")
	add_executable(MACE-Testing ${TESTS_SRC})
		
	target_link_libraries(MACE-Testing LINK_PUBLIC MACE)
	target_include_directories(MACE-Testing PUBLIC ${MACE_INCLUDES})

	set_target_properties(MACE-Testing PROPERTIES FOLDER "MC")
endif()

if(${MACE_DEMOS})
	file(GLOB_RECURSE DEMOS_SRC "demos/*.cpp")
	foreach(FILE ${DEMOS_SRC})
		get_filename_component(FILE_NAME ${FILE} NAME_WE)
		
		set(PROJECT_NAME MACE-${FILE_NAME})
		
		add_executable(${PROJECT_NAME} ${FILE})

		target_link_libraries(${PROJECT_NAME} LINK_PUBLIC MACE)
		
		target_compile_definitions(${PROJECT_NAME} PUBLIC MACE_DEMO_ASSETS="${PROJECT_SOURCE_DIR}/demos/assets/")
		
		set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "MC/Demos")
	endforeach()
endif()

#COMPILER CHECKS

#verify that we are using C++ 11
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	message(STATUS "Found C++ 11 support in ${CMAKE_CXX_COMPILER}")
	target_compile_options(MACE PUBLIC -std=c++11)
elseif(COMPILER_SUPPORTS_CXX0X)
	message(STATUS "Found C++ 0X support in ${CMAKE_CXX_COMPILER}")
	target_compile_options(MACE PUBLIC -std=c++0x)
else()
	message("The compiler ${CMAKE_CXX_COMPILER} has no standard C++ 11 support. C++ 11 must be manually set on this compiler, or it doesn't support it.")
endif()

if(MSVC)
	check_cxx_compiler_flag("-W4" COMPILER_SUPPORTS_WARNING_LEVEL_4)
	if(COMPILER_SUPPORTS_WARNING_LEVEL_4)
		target_compile_options(MACE PRIVATE "$<$<CONFIG:DEBUG>:-W4>")
	endif()
else()
	check_cxx_compiler_flag("-Wall" COMPILER_SUPPORTS_ALL_WARNINGS)
	if(COMPILER_SUPPORTS_ALL_WARNINGS)
		target_compile_options(MACE PRIVATE "$<$<CONFIG:DEBUG>:-Wall>")
	endif()
endif()

if(CMAKE_COMPILER_IS_GNUXX)
	check_cxx_compiler_flag("-g" COMPILER_SUPPORTS_DEBUGGING)
	if(COMPILER_SUPPORTS_DEBUGGING)
		target_compile_options(MACE PRIVATE "$<$<CONFIG:DEBUG>:-g>")
	endif()
endif()


target_compile_definitions(MACE PRIVATE "$<$<CONFIG:DEBUG>:MACE_DEBUG=true>")

if(NOT ${GLFW_FOUND})
	if(NOT MACE_DOWNLOAD_DEPENDENCIES)
		message(FATAL_ERROR "GLFW not found!")
	else()
		download_library(${GLFW_SOURCE_NAME} ${GLFW_URL})
		add_subdirectory(${PROJECT_SOURCE_DIR}/${GLFW_SOURCE_NAME})
		
		if(MSVC)
			#the /rtc flag breaks glfw. we have to remove it.
		    foreach(flag_var
			CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
			CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
				if(${flag_var} MATCHES "/MD")
					string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
					string (REGEX REPLACE "/RTC(su|[1su])" "" ${flag_var} "${${flag_var}}")
				endif()
			endforeach(flag_var)
		endif()
		
		set(GLFW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/${GLFW_SOURCE_NAME}/include)
		set(GLFW_LIBRARY glfw)
	endif()
endif()

if(NOT ${GLEW_FOUND})
	if(NOT MACE_DOWNLOAD_DEPENDENCIES)
		message(FATAL_ERROR "GLEW not found!")
	else()
		download_library(${GLEW_SOURCE_NAME} ${GLEW_URL})
		add_subdirectory(${PROJECT_SOURCE_DIR}/${GLEW_SOURCE_NAME}/build/cmake)
		
		set(GLEW_LIBRARY glew_s)
		
		if(BUILD_SHARED_LIBS)
			add_definitions(-DGLEW_BUILD)
		else()
			add_definitions(-DGLEW_STATIC)
		endif()
		
		set(GLEW_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/${GLEW_SOURCE_NAME}/include)
	endif()
endif()

if(NOT ${FREETYPE_FOUND})
	if(NOT MACE_DOWNLOAD_DEPENDENCIES)
		message(FATAL_ERROR "Freetype not found!")
	else()
		download_library(${FREETYPE_SOURCE_NAME} ${FREETYPE_URL})
		add_subdirectory(${PROJECT_SOURCE_DIR}/${FREETYPE_SOURCE_NAME})
		
		set(FREETYPE_LIBRARY freetype)
		
		set(FREETYPE_INCLUDE_DIR_ft2build ${PROJECT_SOURCE_DIR}/${FREETYPE_SOURCE_NAME}/include)
		set(FREETYPE_INCLUDE_DIR_freetype2 ${FREETYPE_INCLUDE_DIR_ft2build})
	endif()
endif()

if(NOT ${OPENAL_FOUND})
	if(NOT MACE_DOWNLOAD_DEPENDENCIES)
		message(FATAL_ERROR "OpenAL not found!")
	else()
		download_library(${OPENAL_SOURCE_NAME} ${OPENAL_URL})
		add_subdirectory(${PROJECT_SOURCE_DIR}/${OPENAL_SOURCE_NAME})
		
		if(WIN32)
			set(OPENAL_LIBRARY OpenAL32)
		else()
			set(OPENAL_LIBRARY openal)
		endif()
		
		set(OPENAL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/${OPENAL_SOURCE_NAME}/include)
	endif()
endif()

if(MACE_TESTS OR MACE_DEMOS)
	set_target_properties(MACE PROPERTIES FOLDER "MC")
endif()

target_include_directories(MACE PUBLIC ${GLFW_INCLUDE_DIR})
target_include_directories(MACE PUBLIC ${GLEW_INCLUDE_DIR})
target_include_directories(MACE PUBLIC ${FREETYPE_INCLUDE_DIR_ft2build})
target_include_directories(MACE PUBLIC ${FREETYPE_INCLUDE_DIR_freetype2})
target_include_directories(MACE PUBLIC ${OPENAL_INCLUDE_DIR})
target_include_directories(MACE PUBLIC ${MACE_INCLUDES})

target_link_libraries(MACE LINK_PUBLIC ${FREETYPE_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${GLFW_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${OPENAL_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${OPENGL_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${GLEW_LIBRARY})

if(UNIX AND NOT APPLE)#glfw requires this
	find_package(X11 REQUIRED)
	target_link_libraries(MACE LINK_PUBLIC -ldl -lm ${X11_LIBRARIES} )
	
	set(MACE_X11 1)
endif()

#For the configure file
target_include_directories(MACE PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
