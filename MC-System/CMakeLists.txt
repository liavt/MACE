CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

message(STATUS "Building MC-System...")

if(NOT EXISTS ${SDL2_SOURCE_DIR})
	message(STATUS "SDL2 source not found. Fixing..")
	#First we check if the .tar.gz is downloaded. There may be chance it is already downloaded and not extracted
	if (NOT EXISTS "${SDL2_DOWNLOAD_PATH}")
		message(STATUS "Downloading SDL2 source...")
		file(DOWNLOAD "${SDL2_URL}" "${SDL2_DOWNLOAD_PATH}")
	else()
		message(STATUS "SDL2 source found in tar.gz")
	endif()

	message(STATUS "Extracting SDL2 source...")
	#Next we extract the .tar.gz. This works on Windows as well
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xvf ${SDL2_DOWNLOAD_PATH}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
	
	#If the extraction fails, go to here.
	if(NOT EXISTS ${SDL2_SOURCE_DIR})
		message(FATAL_ERROR "Failed to extract SDL2 source! Please retry or file a bug report.")
	else()
		message(STATUS "SDL2 extracted.")
	endif()
	
	#Remove the extracted tar. We dont need the original anymore
	file(REMOVE ${SDL2_DOWNLOAD_PATH})
	message(STATUS "Deleted temporary tar")
	message(STATUS "Done adding ${SDL2_SOURCE_NAME}")
else()
	message(STATUS "SDL2 found.")
endif()

#Just because we download the source doesnt mean we can just use the headers! We have to move them to our include directory.
if(NOT EXISTS ${SDL2_INCLUDES})
	message(STATUS "SDL2 includes not found. Fixing...")
	
	#Simply move the headers from the source into our directory
	file(COPY "${SDL2_SOURCE_NAME}/include/" DESTINATION "${MACE_INCLUDES}/SDL")
	
	message(STATUS "SDL2 Includes added!")
else()
	message(STATUS "Found SDL2 includes.")
endif()

if(NOT EXISTS "${SDL2_INCLUDES}/SDL_net.h")
	if(NOT EXISTS "${SDL_NET_SOURCE_DIR}")
		if(NOT EXISTS "${SDL_NET_DOWNLOAD_PATH}")
			message(STATUS "Download SDL_net...")

			file(DOWNLOAD "${SDL_NET_URL}" "${SDL_NET_DOWNLOAD_PATH}")
		else()
			message(STATUS "Found SDL_net source!")
		endif()
		
		message(STATUS "Extracting SDL_net source...")
		#Next we extract the .tar.gz. This works on Windows as well
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E tar xvf ${SDL_NET_DOWNLOAD_PATH}
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		)
		
		#If the extraction fails, go to here.
		if(NOT EXISTS ${SDL_NET_SOURCE_DIR})
			message(FATAL_ERROR "Failed to extract SDL_net source! Please retry or file a bug report.")
		else()
			message(STATUS "SDL_net extracted.")
		endif()
		
		#Remove the extracted tar. We dont need the original anymore
		file(REMOVE ${SDL_NET_DOWNLOAD_PATH})
	else()
		message(STATUS "Found SDL_net source")
	endif()
	
	message(STATUS "Moving SDL_net headers to include/SDL directory... ")
	file(COPY "${SDL_NET_SOURCE_DIR}/SDLnetsys.h" DESTINATION "${SDL2_INCLUDES}")
	file(COPY "${SDL_NET_SOURCE_DIR}/SDL_net.h" DESTINATION "${SDL2_INCLUDES}")

	message(STATUS "SDL_net Includes added!")
else()
	message(STATUS "Found SDL_net includes.")
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE HEADERS "${MACE_INCLUDES}/MC-System/*.h")
file(GLOB_RECURSE MCS_SRC *.cpp)

add_library(MC-System ${HEADERS} ${MCS_SRC})
target_include_directories (MC-System PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})