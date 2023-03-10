cmake_minimum_required (VERSION 3.8)

# Remove trailing '\' and '/' at the end of the path, so can reliably compare
STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_SOURCE_DIR ${CMAKE_SOURCE_DIR})
STRING(REGEX REPLACE "[/\\]+$" "" CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# When compiling Aphro as stand alone library set to C++11,
# when library is part of other project, respect parent project settings.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	# Enforce c++11 standard.
	set (CMAKE_CXX_STANDARD 11)
endif()

# Notification when too low version of C++ is set
if(NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 11)
	message(FATAL_ERROR "ERROR: CMAKE_CXX_STANDARD is not set or it is too low. Minimum C++ v11 is required to compile \"Aphro\" library.")
endif()

set(MODULE_FILES
		Window/VulkanWindow.cpp
		Window/VulkanWindow.h
		Core/GameApp.h
		Core/Camera.h
		Core/Camera.cpp
		Core/GameObject.h
		Core/GameApp.cpp
		Rendering/Renderer.h
		Rendering/Renderer.cpp
		Rendering/Vulkan/VulkanDevice.h
		Rendering/Vulkan/VulkanDevice.cpp
		Rendering/Vulkan/VulkanPipeline.h
		Rendering/Vulkan/VulkanPipeline.cpp
		Rendering/Vulkan/VulkanSwapChain.h
		Rendering/Vulkan/VulkanSwapChain.cpp
		Rendering/Mesh.h
		Rendering/Mesh.cpp
		"Rendering/Systems/SimpleRenderPipeline.h"
		"Rendering/Systems/SimpleRenderPipeline.cpp"
		"Core/Utils.h"
		"Core/GameObject.cpp"
		"Rendering/Vulkan/VulkanBuffer.h"
		"Rendering/Vulkan/VulkanBuffer.cpp"
		"Rendering/FrameInfo.h"
		"Rendering/Vulkan/VulkanDescriptors.h"
		"Rendering/Vulkan/VulkanDescriptors.cpp"
		"Rendering/Systems/PointLightBillboard.cpp"
		"Rendering/Systems/PointLightBillboard.h")


add_library(Aphro STATIC ${MODULE_FILES})

target_link_libraries(Aphro ${Vulkan_LIBRARIES})
target_link_libraries(Aphro glfw)
target_link_libraries(Aphro glm)


# Make sure the compiler can find include files for our Aphro library
# when other libraries or executables link to Aphro
target_include_directories(Aphro PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# List files that install and/or package should provide.
# Each library should know what it want to distribute,
# which files are internal or intermediate and which are public library export.
install(TARGETS Aphro DESTINATION Aphro-bin)
install(FILES Window/VulkanWindow.h DESTINATION Aphro-include)