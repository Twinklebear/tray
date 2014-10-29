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
set(RAY_LIBS loaders lights integrator renderer linalg film geometry samplers
	material accelerators filters textures)

add_executable(ray_tracer main.cpp mesh_preprocess.cpp driver.cpp block_queue.cpp args.cpp scene.cpp
	${tinyxml2_DIR}/tinyxml2.cpp)

# Need to link libm on Unix
if (NOT WIN32)
	find_library(M_LIB m)
	set(LIBS ${RAY_LIBS} ${M_LIB})
else ()
	set(LIBS ${RAY_LIBS})
endif()

# If we're buidling the previewer we need SDL2 and OpenGL
if (BUILD_PREVIEWER)
	set(LIBS ${LIBS} previewer ${SDL2_LIBRARY} ${OPENGL_LIBRARIES})
endif()

target_link_libraries(ray_tracer ${LIBS} ${CMAKE_THREAD_LIBS_INIT})
install(TARGETS ray_tracer DESTINATION ${CS6620_INSTALL_DIR})

