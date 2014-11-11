add_library(material bxdf.cpp fresnel.cpp specular_reflection.cpp specular_transmission.cpp
	lambertian.cpp bsdf.cpp matte_material.cpp oren_nayer.cpp plastic_material.cpp
	torrance_sparrow.cpp microfacet_distribution.cpp blinn_distribution.cpp anisotropic_distribution.cpp
	btdf_adapter.cpp translucent_material.cpp metal_material.cpp merl_brdf.cpp merl_material.cpp glass_material.cpp)

