# This script is used to locate the Dlib library.
#
# Note: 
#   - Variables must be set in configuration section before using the script.
#   - The script should work on Windows.
#
# This file will define the following variables:
#   - Dlib_FOUND: Whether the Dlib is found or not.
# 	- Dlib_LIBS: The list of libraries to link against.
# 	- Dlib_INCLUDE_DIRS: The Dlib include directories.

unset(Dlib_FOUND)
unset(Dlib_INCLUDE_DIRS)
unset(Dlib_LIBS)

# 1 Configurations
# Dlib root path

if (WIN32)
    if (NOT DEFINED Dlib_DIR)
        set(Dlib_DIR "C:/workspace/third-party/dlib-19.2")
    endif()
else()
    set(Dlib_DIR /usr/local)
endif()

# Dlib version information
set(Dlib_VERSION_MAJOR 19)
set(Dlib_VERSION_MINOR 16)
set(Dlib_VERSION ${Dlib_VERSION_MAJOR}.${Dlib_VERSION_MINOR})


# 2 Check OS
if (NOT WIN32 AND NOT UNIX)
    message(FATAL_ERROR "Unsupported operating system. Only Windows and Unix systems are supported.")
endif ()


# 3 Find include directory and opencv libraries
find_path(Dlib_INCLUDE_DIRS NAMES
        dlib/image_processing.h
        HINTS
        ${Dlib_DIR}/include
        NO_DEFAULT_PATH)

if (WIN32)
    find_library(Dlib_LIB_DEBUG NAMES dlib
            PATHS ${Dlib_DIR}/lib/Debug)
    find_library(Dlib_LIB_RELEASE NAMES dlib
            PATHS ${Dlib_DIR}/lib/Release)
    set(Dlib_LIBS debug ${Dlib_LIB_DEBUG} optimized ${Dlib_LIB_RELEASE})
elseif (UNIX)
    find_library(Dlib_LIBS NAMES dlib
            HINTS
            ${Dlib_DIR}/lib
            ${Dlib_DIR}/lib64
            NO_DEFAULT_PATH)
endif ()

if (Dlib_LIBS AND Dlib_INCLUDE_DIRS)
    set(Dlib_FOUND 1)
endif (Dlib_LIBS AND Dlib_INCLUDE_DIRS)
