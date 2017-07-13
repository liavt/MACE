#The MIT License (MIT)
#
#Copyright (c) 2016 Liav Turkia
#
#Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
#The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

# - Try to find MACE
# Once done this will define
#  MACE_FOUND - System has MACE
#  MACE_INCLUDE_DIRS - The MACE include directories
#  MACE_LIBRARIES - The libraries needed to use MACE

find_path(MACE_INCLUDE_DIR MACE/MACE.h
		PATH_SUFFIXES
			MC
			MACE
		HINTS
			"${MACE_LOCATION}/include"
			"$ENV{MACE_LOCATION}/include"
		PATHS
			"$ENV{PROGRAMFILES}/MACE/include"
			"$ENV{PROGRAMFILES}"
			"${MACE_INCLUDE_DIR}"
			/usr/include/
			/usr/local/include/
		DOC 
			"The directory where MACE/MACE.h resides"
)

find_library( MACE_LIBRARY 
	NAMES
		MACE
		libMACE
		MACE_s
		libMACE_s
	HINTS
		"${MACE_LOCATION}/lib"
		"${MACE_LOCATION}/lib/x64"
		"${MACE_LOCATION}/lib/x32"
		"${MACE_LOCATION}/lib/${CMAKE_BUILD_TYPE}"
		"$ENV{MACE_LOCATION}/lib"
		"$ENV{MACE_LOCATION}/lib/x64"
		"$ENV{MACE_LOCATION}/lib/${CMAKE_BUILD_TYPE}"
	PATHS
		"${MACE_LIBRARY_DIR}"
		/usr/lib
		/usr/local/lib
		"$ENV{PROGRAMFILES}/MACE/lib"
		"$ENV{PROGRAMFILES}/MACE/lib/${CMAKE_BUILD_TYPE}"
		"$ENV{PROGRAMFILES}/MACE/build/lib"
		"$ENV{PROGRAMFILES}/MACE/build/lib/${CMAKE_BUILD_TYPE}"
	DOC 
		"The MACE library"
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set MACE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(MACE  DEFAULT_MSG
                                  MACE_LIBRARY MACE_INCLUDE_DIR)

mark_as_advanced(MACE_INCLUDE_DIR MACE_LIBRARY)

#if MACE ever switches to a modular system, users of FindMACE don't have to change code,
#because its MACE_LIBRARIES (plural) and MACE_INCLUDE_DIRS (plural)
set(MACE_LIBRARIES ${MACE_LIBRARY} )
set(MACE_INCLUDE_DIRS ${MACE_INCLUDE_DIR} )