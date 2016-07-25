CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-Audio...")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Audio/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

if(${MACE_STATIC_LIBRARY})
	add_library(MC-Audio STATIC ${HEADERS} ${MCT_SRC})
else()
	add_library(MC-Audio SHARED ${HEADERS} ${MCT_SRC})
endif()
target_link_libraries(MC-Audio LINK_PUBLIC MC-System)
target_include_directories (MC-Audio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})