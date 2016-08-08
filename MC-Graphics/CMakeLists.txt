CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-Graphics")

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Graphics/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

if(${MACE_STATIC_LIBRARY})
	add_library(MC-Graphics STATIC ${HEADERS} ${MCT_SRC})
else()
	add_library(MC-Graphics SHARED ${HEADERS} ${MCT_SRC})
endif()

if(GLEW_FOUND)
	target_link_libraries(MC-Graphics LINK_PUBLIC "${GLEW_LIBRARIES}")
else()
	target_link_libraries(MC-Graphics LINK_PUBLIC glew)
endif()

target_link_libraries(MC-Graphics LINK_PUBLIC SDL2)
target_link_libraries(MC-Graphics LINK_PUBLIC MC-System)
target_link_libraries(MC-Graphics LINK_PUBLIC MC-Window)
target_include_directories (MC-Graphics PUBLIC ${GLEW_INCLUDE_DIR})
