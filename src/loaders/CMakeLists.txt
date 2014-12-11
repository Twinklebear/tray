add_library(loaders load_scene.cpp load_material.cpp load_light.cpp load_filter.cpp load_sampler.cpp
	load_texture.cpp load_renderer.cpp load_volume.cpp ${tinyxml2_DIR}/tinyxml2.cpp)

