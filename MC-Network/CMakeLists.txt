CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-Network...")

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Network/*.h" "${MACE_INCLUDES}/MC-Network/*.inl")
file(GLOB_RECURSE MCT_SRC *.cpp)

if(${MACE_STATIC_LIBRARY})
	add_library(MC-Network STATIC ${HEADERS} ${MCT_SRC})
else()
	add_library(MC-Network SHARED ${HEADERS} ${MCT_SRC})
endif()

target_link_libraries(MC-Network LINK_PUBLIC MC-System)
target_include_directories (MC-Network PUBLIC ${MACE_INCLUDES})
