#include <memory>
#include <string>
#include <tinyxml2.h>
#include "linalg/transform.h"
#include "lights/ambient_light.h"
#include "lights/direct_light.h"
#include "lights/pbr_point_light.h"
#include "lights/area_light.h"
#include "loaders/load_scene.h"
#include "loaders/load_light.h"

/*
 * Load the AmbientLight properties and return the light
 * elem should be the root of the ambient light being loaded
 */
static std::unique_ptr<Light> load_ambientl(tinyxml2::XMLElement *elem);
/*
 * Load the DirectLight properties and return the light
 * elem should be the root of the direct light being loaded
 */
static std::unique_ptr<Light> load_directl(tinyxml2::XMLElement *elem);
/*
 * Load the PointLight properties and return the light
 * elem should be the root of the direct light being loaded
 */
static std::unique_ptr<PBRLight> load_pointl(tinyxml2::XMLElement *elem);
/*
 * Load the AreaLight properties and return the light
 */
static std::unique_ptr<AreaLight> load_area_light(tinyxml2::XMLElement *elem);

void load_lights(tinyxml2::XMLElement *elem, PBRLightCache &cache){
	using namespace tinyxml2;
	using namespace std::literals;
	for (XMLNode *n = elem; n; n = n->NextSibling()){
		if (n->Value() == "light"s){
			XMLElement *l = n->ToElement();
			std::string name = l->Attribute("name");
			std::cout << "Loading light: " << name << std::endl;
			std::unique_ptr<PBRLight> light;
			std::string type = l->Attribute("type");
			/*
			if (type == "ambient"){
				light = load_ambientl(l);
			}
			else if (type == "direct"){
				light = load_directl(l);
			}
			*/
			if (type == "point"){
				light = load_pointl(l);
				cache.add(name, std::move(light));
			}
			else if (type == "area"){
				light = load_area_light(l);
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
std::unique_ptr<Light> load_ambientl(tinyxml2::XMLElement *elem){
	Colorf color{1, 1, 1};
	read_color(elem->FirstChildElement("intensity"), color);
	return std::make_unique<AmbientLight>(color);
}
std::unique_ptr<Light> load_directl(tinyxml2::XMLElement *elem){
	Colorf color{1, 1, 1};
	Vector dir{0, 0, 0};
	read_color(elem->FirstChildElement("intensity"), color);
	read_vector(elem->FirstChildElement("direction"), dir);
	return std::make_unique<DirectLight>(color, dir);
}
std::unique_ptr<PBRLight> load_pointl(tinyxml2::XMLElement *elem){
	Colorf color{1, 1, 1};
	Vector pos{0, 0, 0};
	read_color(elem->FirstChildElement("intensity"), color);
	read_vector(elem->FirstChildElement("position"), pos);
	return std::make_unique<PBRPointLight>(Transform::translate(pos), color);
}
std::unique_ptr<AreaLight> load_area_light(tinyxml2::XMLElement *elem){
	Colorf color{1, 1, 1};
	Vector pos{0, 0, 0};
	float radius = 1;
	read_color(elem->FirstChildElement("intensity"), color);
	read_vector(elem->FirstChildElement("position"), pos);
	read_float(elem->FirstChildElement("size"), radius);
	return std::make_unique<AreaLight>(Transform::translate(pos), color, radius);
}

