#include <cstdlib>
#include <iostream>
#include <string>
#include <array>
#include <tinyxml2.h>
#include "linalg/util.h"
#include "linalg/vector.h"
#include "linalg/point.h"
#include "linalg/transform.h"
#include "render/camera.h"
#include "render/render_target.h"
#include "geometry/sphere.h"
#include "geometry/box.h"
#include "geometry/plane.h"
#include "load_scene.h"
#include "scene.h"

/*
 * Load the camera information from the element and return it
 * also passes back x & y image resolution through xres & yres
 */
Camera load_camera(tinyxml2::XMLElement *elem, int &w, int &h);
/*
 * Load object nodes in the XMLElement as children of the
 * passed node. Any geometry needed will be loaded from
 * the scene's geometry cache or added if it's missing
 */
void load_node(tinyxml2::XMLElement *elem, Node &node, Scene &scene);
/*
 * Read the x,y,z attributes of the XMLElement and return it
 */
void read_vector(tinyxml2::XMLElement *elem, Vector &v);
/*
 * Read the x,y,z attributes of the XMLElement and return it
 */
void read_point(tinyxml2::XMLElement *elem, Point &p);
/*
 * Read the value float attribute of the XMLElement and return it
 * optionally passing the attribute name to read from. Default is value
 */
void read_float(tinyxml2::XMLElement *elem, float &f, const std::string &attrib = "value");

Scene load_scene(const std::string &file){
	using namespace tinyxml2;
	XMLDocument doc;
	XMLError err = doc.LoadFile(file.c_str());
	if (err != XML_SUCCESS){
		std::cerr << "load_scene Error: failed to open scene " << file << std::endl;
		std::exit(1);
	}
	XMLElement *xml = doc.FirstChildElement("xml");
	if (!xml){
		std::cerr << "load_scene Error: missing <xml> tag\n";
		std::exit(1);
	}
	XMLElement *scene_node = xml->FirstChildElement("scene");
	if (!scene_node){
		std::cerr << "load_scene Error: no scene found\n";
		std::exit(1);
	}
	XMLElement *cam = xml->FirstChildElement("camera");
	if (!cam){
		std::cerr << "load_scene Error: no camera found\n";
		std::exit(1);
	}
	int w = 0, h = 0;
	Camera camera = load_camera(cam, w, h);
	RenderTarget render_target{static_cast<size_t>(w), static_cast<size_t>(h)};
	Scene scene{std::move(camera), std::move(render_target)};
	load_node(scene_node, scene.get_root(), scene);
	return scene;
}
Camera load_camera(tinyxml2::XMLElement *elem, int &w, int &h){
	using namespace tinyxml2;
	Point pos, target;
	Vector up;
	float fov;
	for (XMLNode *c = elem->FirstChild(); c; c = c->NextSibling()){
		if (c->Value() == std::string{"position"}){
			read_point(c->ToElement(), pos);
		}
		else if (c->Value() == std::string{"target"}){
			read_point(c->ToElement(), target);
		}
		else if (c->Value() == std::string{"up"}){
			read_vector(c->ToElement(), up);
		}
		else if (c->Value() == std::string{"fov"}){
			read_float(c->ToElement(), fov);
		}
		else if (c->Value() == std::string{"width"}){
			c->ToElement()->QueryIntAttribute("value", &w);
		}
		else if (c->Value() == std::string{"height"}){
			c->ToElement()->QueryIntAttribute("value", &h);
		}
	}
	//Compute x & y dimensions of image plane in screen space, in this
	//space the image size is normalized so that the shorter axis has
	//a half length of 1, while the longer has a half length of aspect ratio
	float aspect_ratio = static_cast<float>(w) / h;
	float screen[2];
	if (aspect_ratio > 1){
		screen[0] = aspect_ratio;
		screen[1] = 1;
	}
	else {
		screen[0] = 1;
		screen[1] = 1 / aspect_ratio;
	}
	return Camera{Transform::look_at(pos, target, up),
		std::array<float, 4>{-screen[0], screen[0], -screen[1], screen[1]},
		fov, w, h};
}
void load_node(tinyxml2::XMLElement *elem, Node &node, Scene &scene){
	using namespace tinyxml2;
	auto &children = node.get_children();
	for (XMLNode *c = elem->FirstChild(); c; c = c->NextSibling()){
		if (c->Value() == std::string{"object"}){
			XMLElement *e = c->ToElement();
			std::string name = e->Attribute("name");
			const char *t = e->Attribute("type");
			std::cout << "Object with name " << name << " found\n";
			Geometry *geom = nullptr;
			if (t){
				std::string type = t;
				std::cout << "Geometry type: " << type << std::endl;
				//Check if the geometry is in our cache, if not load it
				auto &gcache = scene.get_geom_cache();
				if (!gcache.get(type)){
					if (type == "sphere"){
						gcache.add(type, std::unique_ptr<Geometry>(new Sphere{}));
					}
					else if (type == "box"){
						gcache.add(type, std::unique_ptr<Geometry>(new Box{}));
					}
					else if (type == "plane"){
						gcache.add(type, std::unique_ptr<Geometry>(new Plane{}));
					}
				}
				geom = gcache.get(type);
			}
			//Push the new child on and assign its geometry, the transform will
			//be setup in further iterations when we read the scale/translate elements
			children.push_back(std::make_shared<Node>(geom, Transform{}, name));
			load_node(e, *children.back(), scene);
		}
		else if (c->Value() == std::string{"scale"}){
			//Query both uniform and non-uniform scaling possibilities
			float f = 1;
			Vector v{1, 1, 1};
			read_float(c->ToElement(), f);
			read_vector(c->ToElement(), v);
			v *= f;
			std::cout << "Scaling of " << v << " applied\n";
			auto &transform = node.get_transform();
			transform = Transform::scale(v.x, v.y, v.z) * transform;
		}
		else if (c->Value() == std::string{"translate"}){
			Vector v;
			read_vector(c->ToElement(), v);
			std::cout << "Translation of " << v << " applied\n";
			auto &transform = node.get_transform();
			transform = Transform::translate(v) * transform;
		}
		else if (c->Value() == std::string{"rotate"}){
			Vector v;
			float d = 0;
			read_vector(c->ToElement(), v);
			read_float(c->ToElement(), d, "angle");
			std::cout << "Rotation of " << d << " deg about " << v << " applied\n";
			auto &transform = node.get_transform();
			transform = Transform::rotate(v, d) * transform;
		}
	}
}
void read_vector(tinyxml2::XMLElement *elem, Vector &v){
	elem->QueryFloatAttribute("x", &v.x);
	elem->QueryFloatAttribute("y", &v.y);
	elem->QueryFloatAttribute("z", &v.z);
}
void read_point(tinyxml2::XMLElement *elem, Point &p){
	elem->QueryFloatAttribute("x", &p.x);
	elem->QueryFloatAttribute("y", &p.y);
	elem->QueryFloatAttribute("z", &p.z);
}
void read_float(tinyxml2::XMLElement *elem, float &f, const std::string &attrib){
	elem->QueryFloatAttribute(attrib.c_str(), &f);
}

