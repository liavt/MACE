CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-Window")

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Window/*.h" "${MACE_INCLUDES}/MC-Window/*.inl")
file(GLOB_RECURSE MCT_SRC *.cpp)

if(${MACE_STATIC_LIBRARY})
	add_library(MC-Window STATIC ${HEADERS} ${MCT_SRC})
else()
	add_library(MC-Window SHARED ${HEADERS} ${MCT_SRC})
endif()

target_include_directories(MC-Window PUBLIC ${MACE_INCLUDES})

target_include_directories (MC-Window PUBLIC ${GLEW_INCLUDE_DIR})
target_link_libraries(MC-Window LINK_PUBLIC "${GLEW_LIBRARIES}")

target_include_directories(MC-Window PUBLIC ${GLFW_INCLUDE_DIR})
target_link_libraries(MC-Window LINK_PUBLIC "${GLFW_LIBRARIES}")

target_link_libraries(MC-Window LINK_PUBLIC MC-System)