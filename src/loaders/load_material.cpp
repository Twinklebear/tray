#include <memory>
#include <string>
#include <tinyxml2.h>
#include "material/matte_material.h"
#include "material/plastic_material.h"
#include "material/translucent_material.h"
#include "material/metal_material.h"
#include "loaders/load_scene.h"
#include "loaders/load_material.h"
#include "loaders/load_texture.h"

/*
 * Load the matte material properties and return the material
 * elem should be root of the material being loaded
 */
static std::unique_ptr<PBRMaterial> load_matte(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the plastic material properties and return the material
 * elem should be the root of the material being loaded
 */
static std::unique_ptr<PBRMaterial> load_plastic(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the translucent material properties and return the material
 * elem should be root of the material being loaded
 */
static std::unique_ptr<PBRMaterial> load_translucent(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);
/*
 * Load the metal material properties and return the material
 * elem should be root of the material being loaded
 */
static std::unique_ptr<PBRMaterial> load_metal(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file);

void load_materials(tinyxml2::XMLElement *elem, PBRMaterialCache &cache, TextureCache &tcache,
	const std::string &file)
{
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
			std::unique_ptr<PBRMaterial> material;
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
			cache.add(name, std::move(material));
		}
		else {
			//The materials are all passed in a block, so once
			//we hit something not a material we're done loading
			return;
		}
	}
}
std::unique_ptr<PBRMaterial> load_matte(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file){
	Texture *tex = nullptr;
	if (elem->FirstChildElement("diffuse")){
		tex = load_texture(elem->FirstChildElement("diffuse"), elem->Attribute("name"), tcache, file);
	}
	if (tex == nullptr){
		std::cout << "Scene error: matte materials require a diffuse attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<MatteMaterial>(tex, 0);
}
std::unique_ptr<PBRMaterial> load_plastic(tinyxml2::XMLElement *elem, TextureCache &tcache,
	const std::string &file)
{
	using namespace tinyxml2;
	Texture *diff = nullptr, *spec = nullptr;
	float rough = 1;
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
		read_float(e, rough);
	}
	if (diff == nullptr || spec == nullptr){
		std::cout << "Scene error: plastic materials require a diffuse and specular attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<PlasticMaterial>(diff, spec, rough);
}
std::unique_ptr<PBRMaterial> load_translucent(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file){
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
		std::cout << "Scene error: plastic materials require a diffuse, specular, reflection"
			<< " and transmission attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<TranslucentMaterial>(diff, spec, refl, trans, rough, ior);
}
std::unique_ptr<PBRMaterial> load_metal(tinyxml2::XMLElement *elem, TextureCache &tcache, const std::string &file){
	using namespace tinyxml2;
	Texture *ior = nullptr, *absorp_coef = nullptr;
	float rough = 1;
	std::string name = elem->Attribute("name");
	XMLElement *e = elem->FirstChildElement("ior");
	if (e){
		ior = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("absorption");
	if (e){
		absorp_coef = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("roughness");
	if (e){
		read_float(e, rough);
	}
	if (ior == nullptr || absorp_coef == nullptr){
		std::cout << "Scene error: metal materials require an ior and absorption attribute" << std::endl;
		std::exit(1);
	}
	return std::make_unique<MetalMaterial>(ior, absorp_coef, rough);
}

