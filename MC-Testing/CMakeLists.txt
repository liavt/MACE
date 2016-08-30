CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

message(STATUS "Adding MC-Testing...")

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-Testing/*.h")
file(GLOB_RECURSE MCT_SRC *.cpp)

add_executable(MC-Testing ${HEADERS} ${MCT_SRC})

target_include_directories(MC-Testing PUBLIC ${MACE_INCLUDES})

target_include_directories (MC-Testing PUBLIC ${GLEW_INCLUDE_DIR})
target_link_libraries(MC-Testing LINK_PUBLIC "${GLEW_LIBRARY}")

target_include_directories(MC-Testing PUBLIC ${GLFW_INCLUDE_DIR})
target_link_libraries(MC-Testing LINK_PUBLIC "${GLFW_LIBRARY}")
	
target_link_libraries(MC-Testing LINK_PUBLIC MC-System)
target_link_libraries(MC-Testing LINK_PUBLIC MC-Window)
target_link_libraries(MC-Testing LINK_PUBLIC MC-Graphics)
target_link_libraries(MC-Testing LINK_PUBLIC MC-Network)
target_link_libraries(MC-Testing LINK_PUBLIC MC-Audio)
target_link_libraries(MC-Testing LINK_PUBLIC ${GLFW_LIBRARY_NAME})
target_link_libraries(MC-Testing LINK_PUBLIC ${OPENGL_LIBRARY})
