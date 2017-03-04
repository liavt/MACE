if(__MACE_DOWNLOAD_LIBRARY)
	return()
endif()

set(__MACE_DOWNLOAD_LIBRARY YES)

function(download_library name url)
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${name}")
		if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${name}.tar.gz")
			message(STATUS "Downloading ${PROJECT_SOURCE_DIR}/${name} from ${url}")
			file(DOWNLOAD "${url}" ${PROJECT_SOURCE_DIR}/${name}.tar.gz TIMEOUT 10)
		endif()
		
		file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/${name})
		
		if(EXISTS "${PROJECT_SOURCE_DIR}/${name}.tar.gz")
			set(TARBELL_NAME "${name}.tar.gz")
		elseif(EXISTS "${PROJECT_SOURCE_DIR}/${name}.tgz")
			set(TARBELL_NAME "${name}.tgz")
		else()
			message(FATAL_ERROR "Failed to download ${name} from ${url}. Are you connected to the internet?")
		endif()
		
		message(STATUS Extracting ${TARBELL_NAME}...)

		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${TARBELL_NAME} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} RESULT_VARIABLE UNTAR_RESULT)
				
		if(NOT "${UNTAR_RESULT}" MATCHES "0")
			message(FATAL_ERROR "Failed to untar ${TARBELL_NAME} with result ${UNTAR_RESULT}")
		endif()
		
		message(STATUS "Downloaded ${name} to source directory.")
	else()
		message(STATUS "Found ${name} in source directory")
	endif()
endfunction()
