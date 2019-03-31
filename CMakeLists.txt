cmake_minimum_required(VERSION 3.12)
project(iwatch)
set(PROJECT_VERSION "0.0.4")

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if (WIN32)
	if (NOT DEFINED OpenCV_DIR)
	    set(OpenCV_DIR "C:/workspace/third-party/opencv-3.4.1/build")
	endif()
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
# TODO: Add binary path to load opencv's dll 

find_package(Dlib REQUIRED)
include_directories(${Dlib_INCLUDE_DIRS})
find_package(OpenPose REQUIRED)
include_directories(${OpenPose_INCLUDE_DIRS})

include_directories(include)

add_subdirectory(src)
add_subdirectory(examples)
