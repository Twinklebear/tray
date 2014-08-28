add_subdirectory(linalg)
add_subdirectory(render)
add_subdirectory(geometry)
set(RAY_LIBS linalg render geometry)

add_executable(proj1 main.cpp args.cpp scene.cpp load_scene.cpp ${tinyxml2_SRC})

# Need to link libm on Unix
if (NOT WIN32)
	find_library(M_LIB m)
	set(LIBS ${RAY_LIBS} ${M_LIB})
else ()
	set(LIBS ${RAY_LIBS})
endif()

# If we're buidling the previewer we need SDL2 and OpenGL
if (BUILD_PREVIEWER)
	set(LIBS ${LIBS} ${SDL2_LIBRARY} ${OPENGL_LIBRARIES})
endif()

target_link_libraries(proj1 ${LIBS})
install(TARGETS proj1 DESTINATION ${CS6620_Prj1_INSTALL_DIR})

