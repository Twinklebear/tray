#include <memory>
#include <string>
#include <tinyxml2.h>
#include "material/matte_material.h"
#include "material/plastic_material.h"
#include "material/translucent_material.h"
#include "material/metal_material.h"
#include "material/specular_metal_material.h"
#include "material/merl_material.h"
#include "material/glass_material.h"
#include "material/mix_material.h"
#include "loaders/load_scene.h"
#include "loaders/load_material.h"
#include "loaders/load_texture.h"

/*
 * Load the matte material properties and return the material
 * elem should be root of the material being loaded
 */
static std::unique_ptr<Material> load_matte(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the plastic material properties and return the material
 * elem should be the root of the material being loaded
 */
static std::unique_ptr<Material> load_plastic(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the translucent material properties and return the material
 * elem should be root of the material being loaded
 */
static std::unique_ptr<Material> load_translucent(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the metal material properties and return the material
 * elem should be root of the material being loaded
 */
static std::unique_ptr<Material> load_metal(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the specular metal material properties and return the material
 */
static std::unique_ptr<Material> load_specular_metal(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the MERL measured material properties and return the material
 * elem should be root of the material being loaded
 */
static std::unique_ptr<Material> load_merl(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the Glass material properties and return the material
 * elem should be root of the material being loaded
 */
static std::unique_ptr<Material> load_glass(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the Mix material properties and return the material
 * elem should be root of the material being loaded
 */
static std::unique_ptr<Material> load_mix(tinyxml2::XMLElement *elem, MaterialCache &mcache, TextureCache &tcache, const std::string &file);

void load_materials(tinyxml2::XMLElement *elem, MaterialCache &cache, TextureCache &tcache, const std::string &file){
	using namespace tinyxml2;
	using namespace std::literals;
	for (XMLNode *n = elem; n; n = n->NextSibling()){
		if (n->Value() == "material"s){
			XMLElement *m = n->ToElement();
			std::string name = m->Attribute("name");
			if (cache.get(name)){
				continue;
			}
			std::cout << "Loading material: " << name << std::endl;
			std::unique_ptr<Material> material = nullptr;
			std::string type = m->Attribute("type");
			if (type == "matte"){
				material = load_matte(m, tcache, file);
			}
			else if (type == "plastic"){
				material = load_plastic(m, tcache, file);
			}
			else if (type == "translucent"){
				material = load_translucent(m, tcache, file);
			}
			else if (type == "metal"){
				material = load_metal(m, tcache, file);
			}
			else if (type == "specular_metal"){
				material = load_specular_metal(m, tcache, file);
			}
			else if (type == "merl"){
				material = load_merl(m, tcache, file);
			}
			else if (type == "glass"){
				material = load_glass(m, tcache, file);
			}
			else if (type == "mix"){
				material = load_mix(m, cache, tcache, file);
			}
			if (material != nullptr){
				cache.add(name, std::move(material));
			}
		}
	}
}
std::unique_ptr<Material> load_matte(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file){
	using namespace tinyxml2;
	Texture *tex = nullptr;
	float rough = 0;
	XMLElement *e = elem->FirstChildElement("diffuse");
	if (e){
		tex = load_texture(e, elem->Attribute("name"), tcache, file);
	}
	e = elem->FirstChildElement("roughness");
	if (e){
		read_float(e, rough);
	}
	if (tex == nullptr){
		std::cout << "Scene error: matte materials require a diffuse attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<MatteMaterial>(tex, rough);
}
std::unique_ptr<Material> load_plastic(tinyxml2::XMLElement *elem, TextureCache &tcache,
	const std::string &file)
{
	using namespace tinyxml2;
	Texture *diff = nullptr, *spec = nullptr;
	float rough_x = 1, rough_y = -1;
	std::string name = elem->Attribute("name");
	XMLElement *e = elem->FirstChildElement("diffuse");
	if (e){
		diff = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("specular");
	if (e){
		spec = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("roughness");
	if (e){
		if (e->Attribute("x") && e->Attribute("y")){
			read_float(e, rough_x, "x");
			read_float(e, rough_y, "y");
		}
		else {
			read_float(e, rough_x);
		}
	}
	if (diff == nullptr || spec == nullptr){
		std::cout << "Scene error: plastic materials require a diffuse and specular attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<PlasticMaterial>(diff, spec, rough_x, rough_y);
}
std::unique_ptr<Material> load_translucent(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file){
	using namespace tinyxml2;
	Texture *diff = nullptr, *spec = nullptr, *refl = nullptr, *trans = nullptr;
	float rough = 1, ior = 1;
	std::string name = elem->Attribute("name");
	XMLElement *e = elem->FirstChildElement("diffuse");
	if (e){
		diff = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("specular");
	if (e){
		spec = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("reflection");
	if (e){
		refl = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("transmission");
	if (e){
		trans = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("roughness");
	if (e){
		read_float(e, rough);
	}
	e = elem->FirstChildElement("ior");
	if (e){
		read_float(e, ior);
	}
	if (diff == nullptr || spec == nullptr || refl == nullptr || trans == nullptr){
		std::cout << "Scene error: translucent materials require a diffuse, specular, reflection"
			<< " and transmission attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<TranslucentMaterial>(diff, spec, refl, trans, rough, ior);
}
std::unique_ptr<Material> load_metal(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file){
	using namespace tinyxml2;
	Texture *ior = nullptr, *absorp_coef = nullptr;
	float rough_x = 1, rough_y = -1;
	std::string name = elem->Attribute("name");
	XMLElement *e = elem->FirstChildElement("ior");
	if (e){
		if (e->Attribute("spd")){
			ior = load_spd(e, name, tcache, file);
		}
		else {
			ior = load_texture(e, name, tcache, file);
		}
	}
	e = elem->FirstChildElement("absorption");
	if (e){
		if (e->Attribute("spd")){
			absorp_coef = load_spd(e, name, tcache, file);
		}
		else {
			absorp_coef = load_texture(e, name, tcache, file);
		}
	}
	e = elem->FirstChildElement("roughness");
	if (e){
		if (e->Attribute("x") && e->Attribute("y")){
			read_float(e, rough_x, "x");
			read_float(e, rough_y, "y");
		}
		else {
			read_float(e, rough_x);
		}
	}
	if (ior == nullptr || absorp_coef == nullptr){
		std::cout << "Scene error: metal materials require an ior and absorption attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<MetalMaterial>(ior, absorp_coef, rough_x, rough_y);
}
std::unique_ptr<Material> load_specular_metal(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file){
	using namespace tinyxml2;
	Texture *ior = nullptr, *absorp_coef = nullptr;
	std::string name = elem->Attribute("name");
	XMLElement *e = elem->FirstChildElement("ior");
	if (e){
		if (e->Attribute("spd")){
			ior = load_spd(e, name, tcache, file);
		}
		else {
			ior = load_texture(e, name, tcache, file);
		}
	}
	e = elem->FirstChildElement("absorption");
	if (e){
		if (e->Attribute("spd")){
			absorp_coef = load_spd(e, name, tcache, file);
		}
		else {
			absorp_coef = load_texture(e, name, tcache, file);
		}
	}
	if (ior == nullptr || absorp_coef == nullptr){
		std::cout << "Scene error: specular metal materials require an ior and absorption attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<SpecularMetalMaterial>(ior, absorp_coef);
}
std::unique_ptr<Material> load_merl(tinyxml2::XMLElement *elem, TextureCache&, const std::string &file){
	using namespace tinyxml2;
	if (!elem->Attribute("file")){
		std::cout << "Scene error: MERL materials require a file attribute to load measured data from" << std::endl;
		std::exit(1);
	}
	std::string brdf_file = elem->Attribute("file");
	brdf_file = file.substr(0, file.rfind(PATH_SEP) + 1) + brdf_file;
	return std::make_unique<MerlMaterial>(brdf_file);
}
std::unique_ptr<Material> load_glass(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file){
	using namespace tinyxml2;
	Texture *refl = nullptr, *trans = nullptr;
	float ior = 1;
	std::string name = elem->Attribute("name");
	XMLElement *e = elem->FirstChildElement("reflection");
	if (e){
		refl = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("transmission");
	if (e){
		trans = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("ior");
	if (e){
		read_float(e, ior);
	}
	if (refl == nullptr || trans == nullptr){
		std::cout << "Scene error: glass materials require a reflection and transmission attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<GlassMaterial>(refl, trans, ior);
}
std::unique_ptr<Material> load_mix(tinyxml2::XMLElement *elem, MaterialCache &mcache, TextureCache &tcache, const std::string &file){
	using namespace tinyxml2;
	Texture *scale = nullptr;
	const Material *mat_a = nullptr, *mat_b = nullptr;
	std::string name = elem->Attribute("name");
	XMLElement *e = elem->FirstChildElement("material");
	if (e){
		mat_a = mcache.get(e->Attribute("name"));
	}
	e = e->NextSiblingElement("material");
	if (e){
		mat_b = mcache.get(e->Attribute("name"));
	}
	e = elem->FirstChildElement("scale");
	if (e){
		scale = load_texture(e, name, tcache, file);
	}
	if (mat_a == nullptr || mat_b == nullptr){
		std::cout << "Scene error: mix materials require two material attributes which must be materials "
			<< "declared prior to the mix material" << std::endl;
		std::exit(1);
	}
	if (scale == nullptr){
		std::cout << "Scene error: mix materials require a scale color or texture" << std::endl;
		std::exit(1);
	}
	return std::make_unique<MixMaterial>(mat_a, mat_b, scale);
}

