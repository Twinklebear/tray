#include <memory>
#include <string>
#include <tinyxml2.h>
#include "material/blinn_phong.h"
#include "material/flat_material.h"
#include "textures/constant_texture.h"
#include "loaders/load_scene.h"
#include "loaders/load_material.h"
#include "loaders/load_texture.h"

/*
 * Load the FlatMaterial properties and return the material
 * elem should be root of the flat material being loaded
 */
static std::unique_ptr<Material> load_flatmat(tinyxml2::XMLElement *elem, TextureCache &tcache,
	const std::string &file);
/*
 * Load the BlinnPhong material properties and return the material
 * elem should be the root of the blinn material being loaded
 */
static std::unique_ptr<Material> load_blinnphong(tinyxml2::XMLElement *elem, TextureCache &tcache,
	const std::string &file);

void load_materials(tinyxml2::XMLElement *elem, MaterialCache &cache, TextureCache &tcache,
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
			std::unique_ptr<Material> material;
			std::string type = m->Attribute("type");
			if (type == "blinn"){
				material = load_blinnphong(m, tcache, file);
			}
			else if (type == "flat"){
				material = load_flatmat(m, tcache, file);
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
std::unique_ptr<Material> load_flatmat(tinyxml2::XMLElement *elem, TextureCache &tcache,
	const std::string &file)
{
	Texture *tex = load_texture(elem->FirstChildElement("color"), elem->Attribute("name"), tcache, file);
	return std::make_unique<FlatMaterial>(tex);
}
std::unique_ptr<Material> load_blinnphong(tinyxml2::XMLElement *elem, TextureCache &tcache,
	const std::string &file)
{
	using namespace tinyxml2;
	Texture *diff = nullptr, *spec = nullptr, *refl = nullptr,
		*refrc = nullptr, *absorp = nullptr;
	float gloss = 1, refr_index = -1;
	std::string name = elem->Attribute("name");
	XMLElement *e = elem->FirstChildElement("diffuse");
	if (e){
		diff = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("specular");
	if (e){
		spec = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("glossiness");
	if (e){
		read_float(e, gloss);
	}
	e = elem->FirstChildElement("reflection");
	if (e){
		refl = load_texture(e, name, tcache, file);
	}
	e = elem->FirstChildElement("refraction");
	if (e){
		refrc = load_texture(e, name, tcache, file);
		read_float(e, refr_index, "index");
	}
	e = elem->FirstChildElement("absorption");
	if (e){
		absorp = load_texture(e, name, tcache, file);
	}
	return std::make_unique<BlinnPhong>(diff, spec, gloss,
		refl, refrc, absorp, refr_index);
}

