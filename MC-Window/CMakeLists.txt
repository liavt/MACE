CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-Window")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Window/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

if(${MACE_STATIC_LIBRARY})
	add_library(MC-Window STATIC ${HEADERS} ${MCT_SRC})
	target_link_libraries(MC-Window LINK_PUBLIC SDL2-static)
else()
	add_library(MC-Window SHARED ${HEADERS} ${MCT_SRC})
	target_link_libraries(MC-Window LINK_PUBLIC SDL2)
endif()
target_link_libraries(MC-Window LINK_PUBLIC MC-System)
target_include_directories (MC-Window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})