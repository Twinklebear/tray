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
#include "loaders/load_material.h"
#include "loaders/load_light.h"
#include "loaders/load_scene.h"
#include "scene.h"

/*
 * Load the camera information from the element and return it
 * also passes back x & y image resolution through xres & yres
 */
static Camera load_camera(tinyxml2::XMLElement *elem, int &w, int &h);
/*
 * Load object nodes in the XMLElement as children of the
 * passed node. Any geometry needed will be loaded from
 * the scene's geometry cache or added if it's missing
 */
static void load_node(tinyxml2::XMLElement *elem, Node &node, Scene &scene);
/*
 * Get the geometry for the type, either return it from the cache
 * or load the geometry into the cache and return it
 * Returns nullptr if no valid geometry can be loaded
 */
static Geometry* get_geometry(const std::string &type, Scene &scene);

Scene load_scene(const std::string &file, int depth){
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
	Scene scene{std::move(camera), std::move(render_target), depth};
	
	//Run a pre-pass to load the materials so they're available when loading the objects
	XMLElement *mats = scene_node->FirstChildElement("material");
	if (mats){
		load_materials(mats, scene.get_mat_cache());
	}
	XMLElement *lights = scene_node->FirstChildElement("light");
	if (lights){
		load_lights(lights, scene.get_light_cache());
	}
	load_node(scene_node, scene.get_root(), scene);
	return scene;
}
Camera load_camera(tinyxml2::XMLElement *elem, int &w, int &h){
	using namespace tinyxml2;
	Point pos, target;
	Vector up;
	float fov;
	for (XMLNode *c = elem->FirstChild(); c; c = c->NextSibling()){
		std::string val = c->Value();
		if (val == "position"){
			read_point(c->ToElement(), pos);
		}
		else if (val == "target"){
			read_point(c->ToElement(), target);
		}
		else if (val == "up"){
			read_vector(c->ToElement(), up);
		}
		else if (val == "fov"){
			read_float(c->ToElement(), fov);
		}
		else if (val == "width"){
			c->ToElement()->QueryIntAttribute("value", &w);
		}
		else if (val == "height"){
			c->ToElement()->QueryIntAttribute("value", &h);
		}
	}
	return Camera{Transform::look_at(pos, target, up), fov, w, h};
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
				geom = get_geometry(type, scene);
			}
			const char *m = e->Attribute("material");
			Material *mat = nullptr;
			if (m){
				std::string mat_name = m;
				std::cout << "Material name: " << mat_name << std::endl;
				mat = scene.get_mat_cache().get(mat_name);
			}
			if (!mat){
				std::cerr << "Warning: material " << m << " could not be found\n";
			}
			//Push the new child on and assign its geometry, the transform will
			//be setup in further iterations when we read the scale/translate elements
			children.push_back(std::make_shared<Node>(geom, mat, Transform{}, name));
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
			auto &inv_transform = node.get_inv_transform();
			Transform t = Transform::scale(v.x, v.y, v.z);
			transform = t * transform;
			inv_transform = inv_transform * t.inverse();

		}
		else if (c->Value() == std::string{"translate"}){
			Vector v;
			read_vector(c->ToElement(), v);
			std::cout << "Translation of " << v << " applied\n";
			auto &transform = node.get_transform();
			auto &inv_transform = node.get_inv_transform();
			Transform t = Transform::translate(v);
			transform = t * transform;
			inv_transform = inv_transform * t.inverse();

		}
		else if (c->Value() == std::string{"rotate"}){
			Vector v;
			float d = 0;
			read_vector(c->ToElement(), v);
			read_float(c->ToElement(), d, "angle");
			std::cout << "Rotation of " << d << " deg about " << v << " applied\n";
			auto &transform = node.get_transform();
			auto &inv_transform = node.get_inv_transform();
			Transform t = Transform::rotate(v, d);
			transform = t * transform;
			inv_transform = inv_transform * t.inverse();
		}
	}
}
Geometry* get_geometry(const std::string &type, Scene &scene){
	//Check if the geometry is in our cache, if not load it
	auto &cache = scene.get_geom_cache();
	if (!cache.get(type)){
		if (type == "sphere"){
			cache.add(type, std::unique_ptr<Geometry>(new Sphere{}));
		}
		else if (type == "box"){
			cache.add(type, std::unique_ptr<Geometry>(new Box{}));
		}
		else if (type == "plane"){
			cache.add(type, std::unique_ptr<Geometry>(new Plane{}));
		}
		else {
			return nullptr;
		}
	}
	return cache.get(type);
}
void read_vector(tinyxml2::XMLElement *elem, Vector &v){
	elem->QueryFloatAttribute("x", &v.x);
	elem->QueryFloatAttribute("y", &v.y);
	elem->QueryFloatAttribute("z", &v.z);
}
void read_color(tinyxml2::XMLElement *elem, Colorf &c){
	elem->QueryFloatAttribute("r", &c.r);
	elem->QueryFloatAttribute("g", &c.g);
	elem->QueryFloatAttribute("b", &c.b);
	float s = 1;
	read_float(elem, s);
	c *= s;
}
void read_point(tinyxml2::XMLElement *elem, Point &p){
	elem->QueryFloatAttribute("x", &p.x);
	elem->QueryFloatAttribute("y", &p.y);
	elem->QueryFloatAttribute("z", &p.z);
}
void read_float(tinyxml2::XMLElement *elem, float &f, const std::string &attrib){
	elem->QueryFloatAttribute(attrib.c_str(), &f);
}

