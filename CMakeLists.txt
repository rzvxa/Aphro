cmake_minimum_required(VERSION 3.8)

project(Aphro)


set(CMAKE_CXX_STANDARD 17)


# The version number.
set(APHRO_VERSION_MAJOR 0)
set(APHRO_VERSION_MINOR 1)
set(APHRO_VERSION_PATCH 1)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED)
find_package( PythonInterp 3 REQUIRED )

add_custom_target(
 CompileAphroShaders
 COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Scripts/compile_shaders.py
 COMMENT "Compiling Aphro Shaders"
)

# Try extracting VulkanSDK path from ${Vulkan_INCLUDE_DIRS}
if (NOT ${Vulkan_INCLUDE_DIRS} STREQUAL "")
	set(VULKAN_PATH ${Vulkan_INCLUDE_DIRS})
	STRING(REGEX REPLACE "/Include" "" VULKAN_PATH ${VULKAN_PATH})
endif()
if(EXISTS "${VULKAN_PATH}")
	message(STATUS "Successfully located the Vulkan SDK: ${VULKAN_PATH}")
else()
	message("Error: Unable to locate Vulkan SDK")
	return()
endif()
include_directories(AFTER ${VULKAN_PATH}/Include)

# Include sub-projects.
add_subdirectory(Vendor/glfw)
add_subdirectory(Vendor/glm)
add_subdirectory(Vendor/tinyobjloader)
add_subdirectory(Aphro)

add_subdirectory(Sandbox)



# USE_FOLDERS group cmake generated projects into one (CMakePredefinedTargets) folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Below line set Sandbox as startup application in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)

# Add installer (it is called CPack) which brings all files together (.exe, .lib, .dll, and .h) and put into one file 
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${APHRO_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${APHRO_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${APHRO_VERSION_PATCH})
include(CPack)
