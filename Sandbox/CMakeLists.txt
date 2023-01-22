cmake_minimum_required (VERSION 3.8)

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)

add_custom_target(
	AphroShaders
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PARENT_DIR}/Aphro/Assets/Shaders/Compiled ${CMAKE_CURRENT_BINARY_DIR}/Assets/Aphro/Shaders
	DEPENDS CompileAphroShaders

	COMMENT "Copying Aphro Shaders"
)

add_custom_target(
	AphroModels
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PARENT_DIR}/Aphro/Assets/Models ${CMAKE_CURRENT_BINARY_DIR}/Assets/Aphro/Models

	COMMENT "Copying Aphro Models"
)

add_executable (Sandbox main.cpp SimpleMovementController.h SimpleMovementController.cpp)
add_dependencies(Sandbox AphroShaders)
add_dependencies(Sandbox AphroModels)

target_link_libraries(Sandbox
		Aphro
)