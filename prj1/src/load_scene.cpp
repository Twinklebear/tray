#include <iostream>
#include <string>
#include <tinyxml2.h>
#include "linalg/vector.h"
#include "load_scene.h"

/*
 * Load a scene as described by the XML document and return it
 * TODO: Write scene class, and return it instead of void
 * TODO: throw on these errors?
 */
void load_scene(const std::string &file){
	using namespace tinyxml2;
	XMLDocument doc;
	XMLError err = doc.LoadFile(file.c_str());
	if (err != XML_SUCCESS){
		std::cerr << "load_scene Error: failed to open scene " << file << std::endl;
		return;
	}
	XMLElement *xml = doc.FirstChildElement("xml");
	if (!xml){
		std::cerr << "load_scene Error: missing <xml> tag\n";
		return;
	}
	XMLElement *scene = xml->FirstChildElement("scene");
	if (!scene){
		std::cerr << "load_scene Error: no scene found\n";
		return;
	}
	XMLElement *cam = xml->FirstChildElement("camera");
	if (!cam){
		std::cerr << "load_scene Error: no camera found\n";
		return;
	}
}
/*
 * Read the x,y,z attributes of the XMLElement and return it
 */
Vector read_vector(tinyxml2::XMLElement *elem){
	if (elem->Attribute("x") && elem->Attribute("y") && elem->Attribute("z")){
		return Vector{elem->FloatAttribute("x"), elem->FloatAttribute("y"),
			elem->FloatAttribute("z")};
	}
	std::cerr << "load_scene Error: malformed vector attribute in scene\n";
	return Vector{0, 0, 0};
}
/*
 * Read the value float attribute of the XMLElement and return it
 */
float read_value(tinyxml2::XMLElement *elem){
	if (elem->Attribute("value")){
		return elem->FloatAttribute("value");
	}
	std::cerr << "load_scene Error: malformed value attribute in scene\n";
	return 0;
}

