#include <memory>
#include <string>
#include <tinyxml2.h>
#include "material/blinn_phong.h"
#include "material/flat_material.h"
#include "loaders/load_scene.h"
#include "loaders/load_material.h"

/*
 * Load the FlatMaterial properties and return the material
 * elem should be root of the flat material being loaded
 */
static std::unique_ptr<Material> load_flatmat(tinyxml2::XMLElement *elem);
/*
 * Load the BlinnPhong material properties and return the material
 * elem should be the root of the blinn material being loaded
 */
static std::unique_ptr<Material> load_blinnphong(tinyxml2::XMLElement *elem);

void load_materials(tinyxml2::XMLElement *elem, MaterialCache &cache){
	using namespace tinyxml2;
	for (XMLNode *n = elem; n; n = n->NextSibling()){
		if (n->Value() == std::string{"material"}){
			XMLElement *m = n->ToElement();
			std::string name = m->Attribute("name");
			std::cout << "Loading material: " << name << std::endl;
			std::unique_ptr<Material> material;
			std::string type = m->Attribute("type");
			if (type == "blinn"){
				material = load_blinnphong(m);
			}
			else if (type == "flat"){
				material = load_flatmat(m);
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
std::unique_ptr<Material> load_flatmat(tinyxml2::XMLElement *elem){
	Colorf color{1, 1, 1};
	read_color(elem->FirstChildElement("color"), color);
	color.normalize();
	return std::make_unique<FlatMaterial>(color);
}
std::unique_ptr<Material> load_blinnphong(tinyxml2::XMLElement *elem){
	using namespace tinyxml2;
	Colorf diff{1, 1, 1}, spec{1, 1, 1}, refl{1, 1, 1},
		refrc{1, 1, 1}, absorp{1, 1, 1};
	float gloss = 1, refr_index = -1;
	XMLElement *e = elem->FirstChildElement("diffuse");
	if (e){
		read_color(e, diff);
	}
	e = elem->FirstChildElement("specular");
	if (e){
		read_color(e, spec);
	}
	e = elem->FirstChildElement("glossiness");
	if (e){
		read_float(e, gloss);
	}
	e = elem->FirstChildElement("reflection");
	if (e){
		read_color(e, refl);
	}
	else {
		refl = Colorf{0, 0, 0};
	}
	e = elem->FirstChildElement("refraction");
	if (e){
		read_color(e, refrc);
		read_float(e, refr_index, "index");
	}
	e = elem->FirstChildElement("absorption");
	if (e){
		read_color(e, absorp);
	}
	diff.normalize();
	spec.normalize();
	refl.normalize();
	refrc.normalize();
	absorp.normalize();
	return std::make_unique<BlinnPhong>(diff, spec, gloss,
		refl, refrc, absorp, refr_index);
}

