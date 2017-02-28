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

file(GLOB_RECURSE TESTS_SRC "*.cpp")
add_executable(MACE-Testing ${TESTS_SRC})
	
target_link_libraries(MACE-Testing LINK_PUBLIC MACE)
target_include_directories(MACE-Testing PUBLIC ${MACE_INCLUDES})

set_target_properties(MACE-Testing PROPERTIES FOLDER "MC")