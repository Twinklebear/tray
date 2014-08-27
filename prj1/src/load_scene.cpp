#include <iostream>
#include <string>
#include <tinyxml2.h>
#include "linalg/vector.h"
#include "load_scene.h"

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
	load_node(scene);
}
void load_node(tinyxml2::XMLElement *elem){
	using namespace tinyxml2;
	//auto &children = node.get_children();
	for (XMLNode *c = elem->FirstChild(); c; c = c->NextSibling()){
		if (c->Value() == std::string{"object"}){
			XMLElement *e = c->ToElement();
			std::string name = e->Attribute("name");
			const char *t = e->Attribute("type");
			std::cout << "Object with name " << name << " found\n";
			if (t){
				std::string type = t;
				std::cout << "type: " << type << std::endl;
			}
			//This would really be the child node
			load_node(e);
		}
		else if (c->Value() == std::string{"scale"}){
			//Query both uniform and non-uniform scaling possibilities
			float f = 1;
			Vector v{1, 1, 1};
			read_float(c->ToElement(), f);
			read_vector(c->ToElement(), v);
			v *= f;
			std::cout << "Scaling of " << v << " applied\n";
		}
		else if (c->Value() == std::string{"translate"}){
			Vector v;
			read_vector(c->ToElement(), v);
			std::cout << "Translation of " << v << " applied\n";
		}
	}
}
void read_vector(tinyxml2::XMLElement *elem, Vector &v){
	elem->QueryFloatAttribute("x", &v.x);
	elem->QueryFloatAttribute("y", &v.y);
	elem->QueryFloatAttribute("z", &v.z);
}
void read_float(tinyxml2::XMLElement *elem, float &f){
	elem->QueryFloatAttribute("value", &f);
}

