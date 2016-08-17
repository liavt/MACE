CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-System...")

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-System/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

if(${MACE_STATIC_LIBRARY})
	add_library(MC-System STATIC ${HEADERS} ${MCT_SRC})
else()
	add_library(MC-System SHARED ${HEADERS} ${MCT_SRC})
endif()

target_include_directories(MC-System PUBLIC ${MACE_INCLUDES})
target_link_libraries(MC-System LINK_PUBLIC ${GLFW_LIBRARY_NAME})