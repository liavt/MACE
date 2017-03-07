#The MIT License (MIT)
#
#Copyright (c) 2016 Liav Turkia
#
#Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
if(__MACE_DOWNLOAD_LIBRARY)
	return()
endif()

set(__MACE_DOWNLOAD_LIBRARY YES)

function(download_library name url)
	if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${name}")
		string(REGEX MATCH "tar\\.gz|tgz|zip" FILE_EXTENSION "${url}")
	
		set(TARBELL_NAME "${name}.${FILE_EXTENSION}")
	
		if(NOT EXISTS "${PROJECT_SOURCE_DIR}/${name}.tar.gz")
			message(STATUS "Downloading ${PROJECT_SOURCE_DIR}/${TARBELL_NAME} from ${url}")
			
			
			file(DOWNLOAD "${url}" "${PROJECT_SOURCE_DIR}/${TARBELL_NAME}" TIMEOUT 30 STATUS DOWNLOAD_RESULT)
			if(NOT "${DOWNLOAD_RESULT}" MATCHES "0")
				message(FATAL_ERROR "Failed to download from ${url} with error ${DOWNLOAD_RESULT}. Are you sure you are connected to the internet?")
			endif()
		endif()
		
		file(MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/${name})
				
		message(STATUS "Extracting ${TARBELL_NAME}...")

		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${TARBELL_NAME} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} RESULT_VARIABLE UNTAR_RESULT)
				
		if(NOT "${UNTAR_RESULT}" MATCHES "0")
			message(FATAL_ERROR "Failed to untar ${TARBELL_NAME} with result ${UNTAR_RESULT}")
		endif()
		
		message(STATUS "Downloaded ${name} to source directory.")
	else()
		message(STATUS "Found ${name} in source directory")
	endif()
endfunction()
