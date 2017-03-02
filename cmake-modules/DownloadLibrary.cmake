if(__MACE_DOWNLOAD_LIBRARY)
	return()
endif()

set(__MACE_DOWNLOAD_LIBRARY YES)

function(download_library name url)
	if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}")
		if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${name}.tar.gz")
			message(STATUS "Downloading ${CMAKE_CURRENT_SOURCE_DIR}/${name} from ${url}")
			file(DOWNLOAD "${url}" ${PROJECT_SOURCE_DIR}/${name}.tar.gz TIMEOUT 10)
		endif()
		
		file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/${name})
		
		if(EXISTS ${name}.tar.gz)
			set(TARBELL_NAME ${name}.tar.gz)
		else()
			set(TARBELL_NAME ${name}.tgz)
		endif()

		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${TARBELL_NAME} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	endif()
	
	message(STATUS "Found ${name} in source directory")
endfunction()
