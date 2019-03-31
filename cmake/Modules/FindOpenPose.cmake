# This script is used to locate the OpenPose library.
#
# Note: 
#   - Variables must be set in configuration section before using the script.
#   - The script should work on Windows.
#
# This file will define the following variables:
#   - OpenPose_FOUND: Whether the OpenPose is found or not.
# 	- OpenPose_LIBS: The list of libraries to link against.
# 	- OpenPose_INCLUDE_DIRS: The OpenPose include directories.

unset(OpenPose_FOUND)
unset(OpenPose_INCLUDE_DIRS)
unset(OpenPose_LIBS)

# 1 Configurations
# OpenPose root path

if (WIN32)
    if (NOT DEFINED OpenPose_DIR)
        set(OpenPose_DIR "C:/workspace/third-party/openpose-1.4.0-win64-gpu-binaries")
    endif()
else()
    set(OpenPose_DIR /usr/local)
endif()

# 2 Check OS
if (NOT WIN32 AND NOT UNIX)
    message(FATAL_ERROR "Unsupported operating system. Only Windows and Unix systems are supported.")
endif ()


# 3 Find include directory and opencv libraries
find_path(OpenPose_INCLUDE_DIRS NAMES
        openpose/headers.hpp
        HINTS
        ${OpenPose_DIR}/include
        NO_DEFAULT_PATH)

if (WIN32)
    find_library(OpenPose_LIB_DEBUG NAMES openposed
            PATHS ${OpenPose_DIR}/lib)
    find_library(OpenPose_LIB_RELEASE NAMES openpose
            PATHS ${OpenPose_DIR}/lib)
    set(OpenPose_LIBS debug ${OpenPose_LIB_DEBUG} optimized ${OpenPose_LIB_RELEASE})
elseif (UNIX)
    find_library(OpenPose_LIBS NAMES openpose
            HINTS
            ${OpenPose_DIR}/lib
            ${OpenPose_DIR}/lib64
            NO_DEFAULT_PATH)
endif ()

if (OpenPose_LIBS AND OpenPose_INCLUDE_DIRS)
    set(OpenPose_FOUND 1)
endif (OpenPose_LIBS AND OpenPose_INCLUDE_DIRS)
