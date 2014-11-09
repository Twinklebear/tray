#include <memory>
#include <string>
#include <tinyxml2.h>
#include "linalg/transform.h"
#include "lights/point_light.h"
#include "lights/area_light.h"
#include "loaders/load_scene.h"
#include "loaders/load_light.h"

/*
 * Load the DirectLight properties and return the light
 * elem should be the root of the direct light being loaded
 */
//static std::unique_ptr<Light> load_directl(tinyxml2::XMLElement *elem);
/*
 * Load the PointLight properties and return the light
 * elem should be the root of the direct light being loaded
 */
static std::unique_ptr<Light> load_point_light(tinyxml2::XMLElement *elem);

void load_lights(tinyxml2::XMLElement *elem, LightCache &cache){
	using namespace tinyxml2;
	using namespace std::literals;
	for (XMLNode *n = elem; n; n = n->NextSibling()){
		if (n->Value() == "light"s){
			XMLElement *l = n->ToElement();
			std::string name = l->Attribute("name");
			std::cout << "Loading light: " << name << std::endl;
			std::unique_ptr<Light> light = nullptr;
			std::string type = l->Attribute("type");
			if (type == "point"){
				light = load_point_light(l);
			}
			if (light != nullptr){
				cache.add(name, std::move(light));
			}
		}
		else {
			//The lights are all passed in a block, so once
			//we hit something not a light we're done loading
			return;
		}
	}
}
std::unique_ptr<Light> load_point_light(tinyxml2::XMLElement *elem){
	Colorf color{1, 1, 1};
	Vector pos{0, 0, 0};
	read_color(elem->FirstChildElement("intensity"), color);
	read_vector(elem->FirstChildElement("position"), pos);
	return std::make_unique<PointLight>(Transform::translate(pos), color);
}

