CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-Graphics")


file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Graphics/*.h" "${MACE_INCLUDES}/MC-Graphics/*.inl")
file(GLOB_RECURSE MCT_SRC *.cpp)

if(${MACE_STATIC_LIBRARY})
	add_library(MC-Graphics STATIC ${HEADERS} ${MCT_SRC})
else()
	add_library(MC-Graphics SHARED ${HEADERS} ${MCT_SRC})
endif()

target_include_directories(MC-Graphics PUBLIC ${MACE_INCLUDES})
target_include_directories (MC-Graphics PUBLIC ${GLEW_INCLUDE_DIR})
if(GLEW_FOUND)
	target_link_libraries(MC-Graphics LINK_PUBLIC "${GLEW_LIBRARIES}")
else()
	if(${MACE_STATIC_LIBRARY}})
		target_link_libraries(MC-Graphics LINK_PUBLIC glew_s)
	else()
		target_link_libraries(MC-Graphics LINK_PUBLIC glew)
	endif()
endif()

target_link_libraries(MC-Graphics LINK_PUBLIC MC-System)
target_link_libraries(MC-Graphics LINK_PUBLIC MC-Window)