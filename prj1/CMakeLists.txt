cmake_minimum_required(VERSION 2.6)
project(CS6620_Prj1)

# Bump up warning levels appropriately for each compiler
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	# TODO: debug & release configurations
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -ggdb -std=c++11")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Zi")
	endif()
endif()

# Use the math libs M_PI & etc
add_definitions(-D_USE_MATH_DEFINES)
set(CS6620_Prj1_INSTALL_DIR "${CS6620-Prj1_SOURCE_DIR}/bin")
set(tinyxml2_SRC "${CS6620-Prj1_SOURCE_DIR}/tinyxml2/tinyxml2.cpp")
include_directories(tinyxml2 include)
add_subdirectory(src)

