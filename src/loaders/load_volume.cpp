#include <stack>
#include <memory>
#include <iostream>
#include <tinyxml2.h>
#include "scene.h"
#include "film/color.h"
#include "loaders/load_scene.h"
#include "volume/volume.h"
#include "volume/homogeneous_volume.h"
#include "volume/geometry_volume.h"
#include "volume/exponential_volume.h"
#include "volume/grid_volume.h"
#include "volume/volume_node.h"

Volume* load_volume(tinyxml2::XMLElement *elem, VolumeCache &cache, const std::string &scene_file){
	if (!elem->Attribute("name")){
		std::cout << "Scene error: Volumes require a name" << std::endl;
		return nullptr;
	}
	if (!elem->Attribute("type")){
		std::cout << "Scene error: Volumes require a type" << std::endl;
		return nullptr;
	}

	std::string name = elem->Attribute("name");
	std::string type = elem->Attribute("type");

	Volume *vol = cache.get(name);
	if (vol){
		return vol;
	}
	Colorf sig_a{1}, sig_s{1}, emit{1};
	float phase_asym;
	read_color(elem->FirstChildElement("absorption"), sig_a);
	read_color(elem->FirstChildElement("scattering"), sig_s);
	read_color(elem->FirstChildElement("emission"), emit);
	read_float(elem->FirstChildElement("phase_asymmetry"), phase_asym);

	if (type == "homogeneous"){
		Point min, max;
		read_point(elem->FirstChildElement("min"), min);
		read_point(elem->FirstChildElement("max"), max);
		return cache.add(name, std::make_unique<HomogeneousVolume>(sig_a, sig_s, emit, phase_asym, BBox{min, max}));
	}
	if (type == "exponential"){
		float a = 0, b = 0;
		Vector up;
		Point min, max;
		read_float(elem->FirstChildElement("a"), a);
		read_float(elem->FirstChildElement("b"), b);
		read_vector(elem->FirstChildElement("up"), up);
		read_point(elem->FirstChildElement("min"), min);
		read_point(elem->FirstChildElement("max"), max);
		return cache.add(name, std::make_unique<ExponentialVolume>(sig_a, sig_s, emit, phase_asym, BBox{min, max}, a, b, up));
	}
	if (type == "vol"){
		std::string file = scene_file.substr(0, scene_file.rfind(PATH_SEP) + 1) + elem->Attribute("file");
		float density_scale = 1;
		read_float(elem->FirstChildElement("density_scale"), density_scale);
		return cache.add(name, std::make_unique<GridVolume>(sig_a, sig_s, emit, phase_asym, file, density_scale));
	}
	std::cout << "Scene error: Unrecognized volume type " << type << std::endl;
	return nullptr;
}
void load_volume_node(tinyxml2::XMLElement *elem, Scene &scene, std::stack<Transform> &transform_stack, const std::string &file){
	if (!elem->Attribute("name")){
		std::cout << "Scene error: Volume nodes require a name" << std::endl;
		std::exit(1);
	}
	std::string name = elem->Attribute("name");
	Transform t;
	read_transform(elem, t);
	t = transform_stack.top() * t;
	Volume *vol = load_volume(elem->FirstChildElement("volume"), scene.get_volume_cache(), file);
	if (!vol){
		std::cout << "Scene error: could not load volume attached to volume node " << name << std::endl;
		std::exit(1);
	}
	if (scene.get_volume_root() == nullptr){
		scene.set_volume_root(std::make_unique<VolumeNode>(vol, t, name));
	}
	else {
		auto &children = scene.get_volume_root()->get_children();
		children.push_back(std::make_unique<VolumeNode>(vol, t, name));
	}
	transform_stack.push(t);
}

