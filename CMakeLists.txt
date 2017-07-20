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
		set(CMAKE_POLICY_DEFAULT_CMP0017 NEW) 
	endif()
	
	if(POLICY CMP0025)
		cmake_policy(SET CMP0025 OLD)
		set(CMAKE_POLICY_DEFAULT_CMP0025 NEW) 
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
include(ExternalProject)
include(ParseAndAddCatchTests)

option(MACE_TESTS "Whether to build the unit tests under /tests." OFF)
option(MACE_DEMOS "Whether to build the demo programs under /demos" OFF)
option(MACE_ALL_WARNINGS "Whether to include all warnings in compilation" OFF)
option(MACE_SWIG "Whether to generage cross-language binaries with SWIG" OFF)
set(MACE_SWIG_LANGUAGE "" CACHE STRING "What language to generate binaries for using SWIG. MACE_SWIG must be enabled for this to work.")
option(MACE_DOWNLOAD_DEPENDENCIES "Whether to download and extract dependencies that are not found. If turned off, will instead error." ON)
mark_as_advanced(MACE_SWIG)
mark_as_advanced(MACE_SWIG_LANGUAGE)
mark_as_advanced(MACE_DOWNLOAD_DEPENDENCIES)

#for glfw
set(LIB_SUFFIX ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

set(MACE_MSVC_FLAGS /W4 /w44640 /w14242 /w14254 /w14265 /w14287 /w14287 /w14296 /w14311 /w14545 /w14546 /w14547 /w14546 /w14549 /w14555 /w14619 /w14640 /w14826)
set(MACE_COMPILER_FLAGS -Wall -Wnon-virtual-dtor -pedantic -Wmisleading-indentation)

#This defines where all of our includes are located
set(MACE_INCLUDES "${PROJECT_SOURCE_DIR}/include")

#get version number
file(READ "${PROJECT_SOURCE_DIR}/VERSION" MACE_VERSION)

string(REPLACE "." ";" MACE_VERSION_COMPONENTS ${MACE_VERSION})
list(GET MACE_VERSION_COMPONENTS 0 MACE_VERSION_MAJOR)
list(GET MACE_VERSION_COMPONENTS 1 MACE_VERSION_MINOR)
list(GET MACE_VERSION_COMPONENTS 2 MACE_VERSION_PATCH)

set(GLFW_SOURCE_VERSION "3.2.1")
set(GLFW_SOURCE_NAME "glfw-${GLFW_SOURCE_VERSION}")
set(GLFW_URL "https://github.com/glfw/glfw/archive/${GLFW_SOURCE_VERSION}.tar.gz")

set(OPENAL_SOURCE_VERSION "1.18.0")
set(OPENAL_SOURCE_NAME "openal-soft-openal-soft-${OPENAL_SOURCE_VERSION}")
set(OPENAL_URL "https://github.com/kcat/openal-soft/archive/openal-soft-${OPENAL_SOURCE_VERSION}.tar.gz")

set(GLEW_SOURCE_VERSION "2.0.0")
set(GLEW_SOURCE_NAME "glew-${GLEW_SOURCE_VERSION}")
set(GLEW_URL "https://sourceforge.net/projects/glew/files/glew/${GLEW_SOURCE_VERSION}/${GLEW_SOURCE_NAME}.tgz/download")

set(FREETYPE_SOURCE_VERSION "2.8")
set(FREETYPE_SOURCE_NAME "freetype-${FREETYPE_SOURCE_VERSION}")
set(FREETYPE_URL "https://download.savannah.gnu.org/releases/freetype/${FREETYPE_SOURCE_NAME}.tar.gz")

set(MACE_OUTPUT_INCLUDE_DIR "${CMAKE_BINARY_DIR}/include")

set(CATCH_URL "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp")
#This is where Catch should be located in
set(CATCH_DOWNLOAD_PATH "${MACE_OUTPUT_INCLUDE_DIR}")

#stb_image is a single header only image loading library.
set(STB_IMAGE_URL "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h")
set(STB_IMAGE_DOWNLOAD_PATH "${MACE_OUTPUT_INCLUDE_DIR}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(OpenGL REQUIRED)
find_package(GLEW ${GLEW_SOURCE_VERSION})
find_package(GLFW ${GLFW_SOURCE_VERSION})
find_package(Freetype ${FREETYPE_SOURCE_VERSION})
find_package(OpenAL)

#finding header-only libraries

find_path(STB_IMAGE_INCLUDE_DIR "stb_image.h" HINTS "${STB_IMAGE_DOWNLOAD_PATH}" PATH_SUFFIXES stb stb_image)
find_package_handle_standard_args(stb_image DEFAULT_MSG STB_IMAGE_INCLUDE_DIR)
mark_as_advanced(STB_IMAGE_INCLUDE_DIR)

find_path(CATCH_INCLUDE_DIR "Catch.hpp" HINTS "${CATCH_INCLUDE_DIR}" "${CATCH_DOWNLOAD_PATH}" PATH_SUFFIXES catch single_include)
find_package_handle_standard_args(Catch DEFAULT_MSG CATCH_INCLUDE_DIR)
mark_as_advanced(CATCH_INCLUDE_DIR)

if(MACE_SWIG)
	find_package(SWIG REQUIRED)
	include(${SWIG_USE_FILE})
	
	set_source_files_properties(MACE.swig PROPERTIES CPLUSPLUS ON)
	set_source_files_properties(MACE.swig PROPERTIES SWIG_FLAGS "-includeall")
	
	if(NOT MACE_SWIG_LANGUAGE)
		message(SEND_ERROR "Must define a language for SWIG to compile to!")
	endif()
elseif(MACE_SWIG_LANGUAGE)
	message(WARNING "MACE_SWIG_LANGUAGE has no effect if MACE_SWIG isnt enabled")
endif()

if(NOT stb_image_FOUND)
	if (NOT EXISTS "${STB_IMAGE_DOWNLOAD_PATH}/stb_image.h")
		if(${MACE_DOWNLOAD_DEPENDENCIES})
			#stb_image is one header file, so its easy to fix. Just download it!
			message(STATUS "Downloading STB_IMAGE..")
			file(DOWNLOAD "${STB_IMAGE_URL}" "${STB_IMAGE_DOWNLOAD_PATH}/stb_image.h")
			message(STATUS "Done downloading STB_IMAGE.")
		else()
			message(FATAL_ERROR "STB_IMAGE is not  found!")
		endif()
	else()
		message(STATUS "STB_IMAGE found.")
	endif()
	set(STB_IMAGE_INCLUDE_DIR ${STB_IMAGE_DOWNLOAD_PATH})
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

if(MACE_SWIG)
	swig_add_module("MACE-${MACE_SWIG_LANGUAGE}" ${MACE_SWIG_LANGUAGE} MACE.swig ${HEADERS} ${SRC})
	swig_link_libraries("MACE-${MACE_SWIG_LANGUAGE}" MACE)
	
	if(TARGET "MACE-${MACE_SWIG_LANGUAGE}")
		set_target_properties("MACE-${MACE_SWIG_LANGUAGE}" PROPERTIES FOLDER "MC")
	endif()
endif()

if(${MACE_TESTS})
	if(NOT Catch_FOUND)
		if(NOT EXISTS ${CATCH_DOWNLOAD_PATH}/Catch.hpp)
			if(${MACE_DOWNLOAD_DEPENDENCIES})
				#Catch is one header file, so its easy to fix. Just download it!
				message(STATUS "Downloading Catch..")
				file(DOWNLOAD "${CATCH_URL}" "${CATCH_DOWNLOAD_PATH}/Catch.hpp")
				set(CATCH_INCLUDE_DIR, "${CATCH_DOWNLOAD_PATH}/Catch.hpp")
				message(STATUS "Done downloading Catch.")
			else()
				message(FATAL_ERROR "Catch is not found!")
			endif()
		endif()
		set(CATCH_INCLUDE_DIR ${CATCH_DOWNLOAD_PATH})
	endif()

	file(GLOB_RECURSE TESTS_SRC "tests/*.cpp")
	add_executable(MACE-Testing ${TESTS_SRC})
			
	target_link_libraries(MACE-Testing LINK_PUBLIC MACE)
	target_include_directories(MACE-Testing PUBLIC ${MACE_INCLUDES})
	target_include_directories(MACE-Testing PUBLIC ${CATCH_INCLUDE_DIR})
	
	#to test runtime dll linking we compiled a source file twice: once as part as MACE-Testing and one as a shared library to test runtime linking
	add_library(MACE-DynamicLibraryTest SHARED "tests/Utility/DynamicLibraryTest.cpp")

	add_executable(MACE-ProcessTest "tests/Utility/ProcessTest.cpp")
	
	target_link_libraries(MACE-DynamicLibraryTest LINK_PUBLIC MACE)
	target_link_libraries(MACE-ProcessTest LINK_PUBLIC MACE)
	target_link_libraries(MACE-Testing LINK_PUBLIC MACE-DynamicLibraryTest)
	target_include_directories(MACE-DynamicLibraryTest PUBLIC ${MACE_INCLUDES})
	target_include_directories(MACE-ProcessTest PUBLIC ${MACE_INCLUDES})

	add_dependencies(MACE-Testing MACE-ProcessTest)
	
	target_compile_definitions(MACE-Testing PUBLIC MACE_DLL_TEST_OUTPUT="$<TARGET_FILE:MACE-DynamicLibraryTest>"
													MACE_PROC_TEST_OUTPUT="$<TARGET_FILE:MACE-ProcessTest>")
	target_compile_definitions(MACE-DynamicLibraryTest PRIVATE MACE_DLL_OUT_PASS="1")
	target_compile_definitions(MACE-ProcessTest PRIVATE MACE_PROC_OUT_PASS="1")

	set_target_properties(MACE-Testing PROPERTIES FOLDER "MC/Tests")
	set_target_properties(MACE-DynamicLibraryTest PROPERTIES FOLDER "MC/Tests")
	set_target_properties(MACE-ProcessTest PROPERTIES FOLDER "MC/Tests")
	
	enable_testing()
	ParseAndAddCatchTests(MACE-Testing)
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
if(COMPILER_SUPPORTS_CXX11)
	message(STATUS "Found C++ 11 support in ${CMAKE_CXX_COMPILER}")
	target_compile_options(MACE PUBLIC -std=c++11)
else()
	check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
	if(COMPILER_SUPPORTS_CXX0X)
		message(STATUS "Found C++ 0X support in ${CMAKE_CXX_COMPILER}")
		target_compile_options(MACE PUBLIC -std=c++0x)
	else()
		message(SEND_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no standard C++ 11 support. C++ 11 must be manually set on this compiler, or it doesn't support it.")
	endif()
endif()

if(MACE_ALL_WARNINGS)
	if(MSVC)
		foreach(MACE_MSVC_FLAG ${MACE_MSVC_FLAGS})
			check_cxx_compiler_flag("${MACE_MSVC_FLAG}" "COMPILER_SUPPORTS_${MACE_MSVC_FLAG}")
			if(COMPILER_SUPPORTS_${MACE_MSVC_FLAG})
				target_compile_options(MACE PRIVATE "$<$<CONFIG:DEBUG>:${MACE_MSVC_FLAG}>")
			endif()
		endforeach()
	else()
		foreach(MACE_COMPILER_FLAG ${MACE_COMPILER_FLAGS})
			check_cxx_compiler_flag("${MACE_COMPILER_FLAG}" "COMPILER_SUPPORTS_${MACE_COMPILER_FLAG}")
			if(COMPILER_SUPPORTS_${MACE_COMPILER_FLAG})
				target_compile_options(MACE PRIVATE "$<$<CONFIG:DEBUG>:${MACE_COMPILER_FLAG}>")
			endif()
		endforeach()
	endif()
endif()

if(MSVC)
	check_cxx_compiler_flag("/EHsc" COMPILER_SUPPORTS_EXCEPTION_HANDLING)
	if(COMPILER_SUPPORTS_EXCEPTION_HANDLING)
		target_compile_options(MACE PRIVATE "/EHsc")
	else()
		message(FATAL_ERROR "This version of Microsoft Visual Studio does not support exception handling.")
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
		
		set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
		set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
		set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
		
		add_subdirectory(${CMAKE_BINARY_DIR}/${GLFW_SOURCE_NAME} ${CMAKE_BINARY_DIR}/${GLFW_SOURCE_NAME})
		
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
		
		set(GLFW_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${GLFW_SOURCE_NAME}/include)
		set(GLFW_LIBRARY glfw)
	endif()
endif()

if(NOT ${GLEW_FOUND})
	if(NOT MACE_DOWNLOAD_DEPENDENCIES)
		message(FATAL_ERROR "GLEW not found!")
	else()
		download_library(${GLEW_SOURCE_NAME} ${GLEW_URL})
		add_subdirectory(${CMAKE_BINARY_DIR}/${GLEW_SOURCE_NAME}/build/cmake ${CMAKE_BINARY_DIR}/${GLEW_SOURCE_NAME})
		
		set(GLEW_LIBRARY glew_s)
		
		if(BUILD_SHARED_LIBS)
			add_definitions(-DGLEW_BUILD)
		else()
			add_definitions(-DGLEW_STATIC)
		endif()
		
		set(GLEW_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${GLEW_SOURCE_NAME}/include)
	endif()
endif()

if(NOT ${FREETYPE_FOUND})
	if(NOT MACE_DOWNLOAD_DEPENDENCIES)
		message(FATAL_ERROR "Freetype not found!")
	else()
		download_library(${FREETYPE_SOURCE_NAME} ${FREETYPE_URL})
		set(WITH_HarfBuzz OFF)
		add_subdirectory(${CMAKE_BINARY_DIR}/${FREETYPE_SOURCE_NAME} ${CMAKE_BINARY_DIR}/${FREETYPE_SOURCE_NAME}/build)
		
		set(FREETYPE_LIBRARY freetype)
		
		set(FREETYPE_INCLUDE_DIR_ft2build ${CMAKE_BINARY_DIR}/${FREETYPE_SOURCE_NAME}/include)
		set(FREETYPE_INCLUDE_DIR_freetype2 ${FREETYPE_INCLUDE_DIR_ft2build})
	endif()
endif()

if(NOT ${OPENAL_FOUND})
	if(NOT MACE_DOWNLOAD_DEPENDENCIES)
		message(FATAL_ERROR "OpenAL not found!")
	else()
		download_library(${OPENAL_SOURCE_NAME} ${OPENAL_URL})
		add_subdirectory(${CMAKE_BINARY_DIR}/${OPENAL_SOURCE_NAME} ${CMAKE_BINARY_DIR}/${OPENAL_SOURCE_NAME})
		
		if(WIN32)
			set(OPENAL_LIBRARY OpenAL)
		else()
			set(OPENAL_LIBRARY openal)
		endif()
		
		set(OPENAL_INCLUDE_DIR ${CMAKE_BINARY_DIR}/${OPENAL_SOURCE_NAME}/include)
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
target_include_directories(MACE PUBLIC ${STB_IMAGE_INCLUDE_DIR})

target_link_libraries(MACE LINK_PUBLIC ${FREETYPE_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${GLFW_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${OPENAL_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${OPENGL_LIBRARY})
target_link_libraries(MACE LINK_PUBLIC ${GLEW_LIBRARY})

if(UNIX AND NOT APPLE)#glfw requires this
	find_package(X11 REQUIRED)
	target_link_libraries(MACE LINK_PUBLIC -ldl -lm ${X11_LIBRARIES} )
endif()

find_package(OpenCV)

if(${OpenCV_FOUND})
	target_compile_definitions(MACE PUBLIC MACE_OPENCV=1)
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

configure_file("${PROJECT_SOURCE_DIR}/Configure.h.in" "${MACE_OUTPUT_INCLUDE_DIR}/MACE/Configure.h" ESCAPE_QUOTES @ONLY)

#For the configure file
target_include_directories(MACE PUBLIC "${MACE_OUTPUT_INCLUDE_DIR}")