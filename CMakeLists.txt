cmake_minimum_required(VERSION 2.6)
project(CS6620)

# Bump up warning levels appropriately for each compiler
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -ggdb -DDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -mfpmath=sse -march=native -flto -fno-use-linker-plugin")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Zi")
	endif()
endif()

# Use the math libs M_PI & etc
add_definitions(-D_USE_MATH_DEFINES)
set(CS6620_INSTALL_DIR "${CS6620_SOURCE_DIR}/bin")
set(tinyxml2_SRC "${CS6620_SOURCE_DIR}/tinyxml2/tinyxml2.cpp")
include_directories(tinyxml2 include)

# If we're building the live previewer find SDL2
if (BUILD_PREVIEWER)
	add_definitions(-DBUILD_PREVIEWER)
	# Use our modified FindSDL2 modules for windows/etc
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CS6620_SOURCE_DIR}/cmake")
	find_package(SDL2 REQUIRED)
	find_package(OpenGL REQUIRED)
	include_directories(${SDL2_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} previewer)
	add_subdirectory(previewer)
endif()

find_package(Threads)
add_subdirectory(src)

