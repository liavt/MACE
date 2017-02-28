file(GLOB_RECURSE DEMOS_SRC "*.cpp")
foreach(FILE ${DEMOS_SRC})
	get_filename_component(FILE_NAME ${FILE} NAME_WE)
	
	set(PROJECT_NAME MACE-${FILE_NAME})
	
	add_executable(${PROJECT_NAME} ${FILE})

	target_link_libraries(${PROJECT_NAME} LINK_PUBLIC MACE)
	
	target_compile_definitions(${PROJECT_NAME} PUBLIC MACE_DEMO_ASSETS="${PROJECT_SOURCE_DIR}/demos/assets/")
	
	set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "MC/Demos")
endforeach()