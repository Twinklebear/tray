cmake_minimum_required(VERSION 2.6)
project(CS6620)

# Bump up warning levels appropriately for each compiler
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++14")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -m64 -ggdb -DDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -m64 -O3 -march=native -flto -fno-use-linker-plugin -ffast-math -DNDEBUG")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
	if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
		string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /Zi")
	endif()
endif()

set(CS6620_INSTALL_DIR "${CS6620_SOURCE_DIR}/bin")

set(tinyxml2_DIR "${CS6620_SOURCE_DIR}/external/tinyxml2/")
file(DOWNLOAD "https://raw.githubusercontent.com/leethomason/tinyxml2/master/tinyxml2.cpp"
	"${tinyxml2_DIR}/tinyxml2.cpp")
file(DOWNLOAD "https://raw.githubusercontent.com/leethomason/tinyxml2/master/tinyxml2.h"
	"${tinyxml2_DIR}/tinyxml2.h")
# Use stb_image for wider range of image format support
set(stb_image_INCLUDE_DIR "${CS6620_SOURCE_DIR}/external/stb/")
file(DOWNLOAD "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
	"${stb_image_INCLUDE_DIR}/stb_image.h")
include_directories(include ${tinyxml2_DIR} ${stb_image_INCLUDE_DIR})

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

# If user has request trilinear texture filtering
if (TEX_TRILINEAR)
	add_definitions(-DTEX_TRILINEAR)
endif()

find_package(Threads)
add_subdirectory(src)

