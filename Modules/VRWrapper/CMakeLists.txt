cmake_minimum_required(VERSION 3.10.1)
project(RayneVR)

include(../../CMake/LibraryTarget.cmake)
include(../../CMake/Rayne.cmake)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 5)
set(VERSION_PATCH 0)
set(VERSION_ABI 1)
set(VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

set(SOURCES
    RNVRWindow.cpp
    RNVRCompositorLayer.cpp
    RNVRCamera.cpp)

set(HEADERS
	RNVR.h
	RNVRApplication.h
	RNVRApplicationImpl.h
    RNVRWindow.h
    RNVRCompositorLayer.h
    RNVRStubWindow.h
    RNVRCamera.h
    RNVRTrackingState.h)

set(DEFINES RN_BUILD_VR)

set(RAYNE_LIBRARIES Rayne)
set(INCLUDE_DIRECTORIES "${Rayne_BINARY_DIR}/include")

if(APPLE)
	if(${RN_BUILD_METAL_MODULE})
		set(RAYNE_LIBRARIES ${RAYNE_LIBRARIES} RayneMetal)
		set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} "${CMAKE_CURRENT_BINARY_DIR}/../Metal/include/Sources")
	endif()
elseif(ANDROID OR UNIX)
	if(${RN_BUILD_VULKAN_MODULE})
		set(RAYNE_LIBRARIES ${RAYNE_LIBRARIES} RayneVulkan)
		set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} "${CMAKE_CURRENT_BINARY_DIR}/../Vulkan/include/Sources")
	endif()
else(WIN32)
	if(${RN_BUILD_D3D12_MODULE})
		set(RAYNE_LIBRARIES ${RAYNE_LIBRARIES} RayneD3D12)
		set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} "${CMAKE_CURRENT_BINARY_DIR}/../D3D12/include/Sources")
	endif()
	if(${RN_BUILD_VULKAN_MODULE})
		set(RAYNE_LIBRARIES ${RAYNE_LIBRARIES} RayneVulkan)
		set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES} "${CMAKE_CURRENT_BINARY_DIR}/../Vulkan/include/Sources")
	endif()
endif()

rayne_add_library(RayneVR "${SOURCES}" "${HEADERS}" "${RAYNE_LIBRARIES}" "${VERSION_STRING}" "${VERSION_ABI}")
rayne_set_module_output_directory(RayneVR)

target_include_directories(RayneVR SYSTEM PRIVATE "${INCLUDE_DIRECTORIES}")
target_compile_definitions(RayneVR PRIVATE "${DEFINES}")

rayne_install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../../Build/RayneVR/. DESTINATION lib/Rayne/RayneVR)
