cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(MACE CXX)

if(COMMAND cmake_Policy)
	cmake_policy(PUSH)
	cmake_policy(VERSION 2.8.12)
	if(POLICY CMP0017)
		cmake_policy(SET CMP0017 NEW)
	endif()
endif()

message("MACE project added")

#On unix systems this line is requied to compile GLFW and GLEW
enable_language(C)

set(CMAKE_MODULE_PATH ${CMAKE_ROOT}/Modules "${PROJECT_SOURCE_DIR}/cmake-modules")

include(FindPackageHandleStandardArgs)
include(CheckCXXCompilerFlag)
include(DownloadLibrary)

option(MACE_UNIT_TESTING "Whether to build the unit tests under tests." OFF)
option(MACE_DEMOS "Whether to build the demo programs under demos" OFF)
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
set(OPENAL_SOURCE_NAME "openal-soft-${GLFW_SOURCE_VERSION}")
set(OPENAL_URL "https://github.com/kcat/openal-soft/archive/${OPENAL_SOURCE_NAME}.tar.gz")

set(GLEW_SOURCE_VERSION "2.0.0")
set(GLEW_SOURCE_NAME "glew-${GLEW_SOURCE_VERSION}")
set(GLEW_URL "https://kent.dl.sourceforge.net/project/glew/glew/${GLEW_SOURCE_VERSION}/${GLEW_SOURCE_NAME}.tgz")

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

if(${MACE_UNIT_TESTING})
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
					STRING (REGEX REPLACE "/RTC(su|[1su])" "" ${flag_var} "${${flag_var}}")
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
		download_library(${OpenAL_SOURCE_NAME} ${OPENAL_URL})
		add_subdirectory(${PROJECT_SOURCE_DIR}/${OPENAL_SOURCE_NAME})
		
		set(OPENAL_LIBRARY OpenAL)
		
		set(OPENAL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/${OPENAL_SOURCE_NAME}/include)
	endif()
endif()

target_include_directories(MACE PUBLIC ${GLFW_INCLUDE_DIR})
target_link_libraries(MACE LINK_PUBLIC ${GLFW_LIBRARY})

target_include_directories(MACE PUBLIC ${GLEW_INCLUDE_DIR})
target_link_libraries(MACE LINK_PUBLIC ${GLEW_LIBRARY})

target_include_directories(MACE PUBLIC ${FREETYPE_INCLUDE_DIR_ft2build})
target_include_directories(MACE PUBLIC ${FREETYPE_INCLUDE_DIR_freetype2})
target_link_libraries(MACE LINK_PUBLIC ${FREETYPE_LIBRARY})

target_include_directories(MACE PUBLIC ${OPENAL_INCLUDE_DIR})
target_link_libraries(MACE LINK_PUBLIC ${OPENAL_LIBRARY})

target_include_directories(MACE PUBLIC ${MACE_INCLUDES})

target_link_libraries(MACE LINK_PUBLIC ${OPENGL_LIBRARY})

set_target_properties(MACE PROPERTIES FOLDER "MC")

if(COMMAND cmake_policy)
	cmake_policy(POP)
endif()
