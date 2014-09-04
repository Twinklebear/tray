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
#include "material/blinn_phong.h"
#include "material/flat_material.h"
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
 * Load all the material information into the material cache. elem should
 * be the first material XML element in the file
 * We do this in a pre-pass to avoid having to store the material
 * names on the nodes and do a post-load pass to set up the node
 * materials like is done in Cem's loading code, due to how the XML
 * file is layed out with materials coming after objects
 */
void load_materials(tinyxml2::XMLElement *elem, Scene &scene);
/*
 * Load the FlatMaterial properites and return the material
 * elem should be root of the flat material being loaded
 */
std::unique_ptr<Material> load_flatmat(tinyxml2::XMLElement *elem);
/*
 * Load the BlinnPhong material properties and return the material
 * elem should be the root of the blinn material being loaded
 */
std::unique_ptr<Material> load_blinnphong(tinyxml2::XMLElement *elem);
/*
 * Get the geometry for the type, either return it from the cache
 * or load the geometry into the cache and return it
 * Returns nullptr if no valid geometry can be loaded
 */
Geometry* get_geometry(const std::string &type, Scene &scene);
/*
 * Read the x,y,z attributes of the XMLElement and return it
 */
void read_vector(tinyxml2::XMLElement *elem, Vector &v);
/*
 * Read the r,g,b attributes of the XMLElement and return it
 */
void read_color(tinyxml2::XMLElement *elem, Colorf &c);
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
	
	//Run a pre-pass to load the materials so they're available when loading the objects
	XMLElement *mats = scene_node->FirstChildElement("material");
	if (mats){
		load_materials(mats, scene);
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
void load_materials(tinyxml2::XMLElement *elem, Scene &scene){
	using namespace tinyxml2;
	auto &cache = scene.get_mat_cache();
	for (XMLNode *n = elem; n; n = n->NextSibling()){
		if (n->Value() == std::string{"material"}){
			XMLElement *m = n->ToElement();
			std::string name = m->Attribute("name");
			std::cout << "loading material: " << name << std::endl;
			std::unique_ptr<Material> material;
			if (m->Attribute("type") == std::string{"blinn"}){
				material = load_blinnphong(m);
			}
			else if (m->Attribute("type") == std::string{"flat"}){
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
	std::cout << "FlatMaterial color: " << color << std::endl;
	return std::unique_ptr<Material>{new FlatMaterial{color}};
}
std::unique_ptr<Material> load_blinnphong(tinyxml2::XMLElement *elem){
	Colorf diff{1, 1, 1}, spec{1, 1, 1};
	float gloss = 1;
	read_color(elem->FirstChildElement("diffuse"), diff);
	read_color(elem->FirstChildElement("specular"), spec);
	read_float(elem->FirstChildElement("glossiness"), gloss);
	std::cout << "Blinn material diff: " << diff << ", spec: " << spec
		<< ", gloss: " << gloss << std::endl;
	return std::unique_ptr<Material>{new BlinnPhong{diff, spec, gloss}};
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
}
void read_point(tinyxml2::XMLElement *elem, Point &p){
	elem->QueryFloatAttribute("x", &p.x);
	elem->QueryFloatAttribute("y", &p.y);
	elem->QueryFloatAttribute("z", &p.z);
}
void read_float(tinyxml2::XMLElement *elem, float &f, const std::string &attrib){
	elem->QueryFloatAttribute(attrib.c_str(), &f);
}

