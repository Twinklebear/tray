add_subdirectory(linalg)
add_subdirectory(film)
add_subdirectory(geometry)
add_subdirectory(samplers)
add_subdirectory(material)
add_subdirectory(loaders)
add_subdirectory(lights)
add_subdirectory(accelerators)
add_subdirectory(filters)
add_subdirectory(textures)
add_subdirectory(integrator)
add_subdirectory(renderer)
add_subdirectory(monte_carlo)
add_subdirectory(volume)
set(TRAY_LIBS loaders lights renderer integrator linalg film geometry volume
	samplers material accelerators filters textures monte_carlo)

add_executable(tray main.cpp mesh_preprocess.cpp driver.cpp block_queue.cpp args.cpp scene.cpp
	memory_pool.cpp)

# Need to link libm on Unix
if (NOT WIN32)
	find_library(M_LIB m)
	set(LIBS ${TRAY_LIBS} ${M_LIB})
else ()
	set(LIBS ${TRAY_LIBS})
endif()

# If we're buidling the previewer we need SDL2 and OpenGL
if (BUILD_PREVIEWER)
	set(LIBS ${LIBS} previewer ${SDL2_LIBRARY} ${OPENGL_LIBRARIES})
endif()

target_link_libraries(tray ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS tray DESTINATION ${TRAY_INSTALL_DIR})

