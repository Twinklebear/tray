add_subdirectory(linalg)
add_subdirectory(render)
add_subdirectory(geometry)
add_subdirectory(samplers)
add_subdirectory(material)
set(RAY_LIBS linalg render geometry samplers material)

add_executable(ray_tracer main.cpp driver.cpp args.cpp scene.cpp load_scene.cpp ${tinyxml2_SRC})

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

