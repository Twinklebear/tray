add_library(material blinn_phong.cpp flat_material.cpp bxdf.cpp fresnel.cpp specular_reflection.cpp
	specular_transmission.cpp lambertian.cpp bsdf.cpp matte_material.cpp oren_nayer.cpp plastic_material.cpp
	torrance_sparrow.cpp microfacet_distribution.cpp blinn_distribution.cpp anisotropic_distribution.cpp
	btdf_adapter.cpp translucent_material.cpp)

