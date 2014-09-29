#include <memory>
#include <string>
#include <regex>
#include <tinyxml2.h>
#include "textures/constant_texture.h"
#include "textures/image_texture.h"
#include "textures/uv_texture.h"
#include "textures/checkerboard_texture.h"
#include "textures/uv_mapping.h"
#include "loaders/load_scene.h"
#include "loaders/load_texture.h"

Texture* load_texture(tinyxml2::XMLElement *elem, const std::string &mat_name,
	TextureCache &cache, const std::string &file)
{
	using namespace tinyxml2;
	std::string name;
	if (elem->Attribute("texture")){
		name = elem->Attribute("texture");
	}
	else {
		name = "__" + mat_name + "_" + elem->Value() + "_tex";
	}
	Texture *tex = cache.get(name);
	if (tex){
		std::cout << "Using cached texture, name: " << name << std::endl;
		return tex;
	}
	//If it's a generated texture we're just setting a constant color
	//for a material
	if (name.substr(0, 2) == "__"){
		Colorf color{1, 1, 1};
		read_color(elem, color);
		cache.add(name, std::make_unique<ConstantTexture>(color));
		tex = cache.get(name);
	}
	//We need to load a texture file or procedural texture
	else {
		//Read any scaling and translation being applied to the mapping
		Vector scale{1}, translate;
		XMLElement *c = elem->FirstChildElement("scale");
		if (c){
			read_vector(c, scale);
			//The scale specified in the scene files is 2 / scale of what we
			//want to happen to the texture, very odd.
			scale = Vector{2} / scale;
		}
		c = elem->FirstChildElement("translate");
		if (c){
			read_vector(c, translate);
		}

		std::regex match_file{".*\\.[a-zA-Z]{3}$"};
		std::smatch match;
		if (std::regex_match(name, match, match_file)){
			std::string tex_file = file.substr(0, file.rfind(PATH_SEP) + 1) + name;
			//TODO: read the scale & translate
			cache.add(name, std::make_unique<ImageTexture>(tex_file,
				std::make_unique<UVMapping>(scale, translate)));
			tex = cache.get(name);
		}
		else if (name == "uv"){
			name = "__" + mat_name + "_" + name + "_tex";
			cache.add(name, std::make_unique<UVTexture>(std::make_unique<UVMapping>(scale, translate)));
			tex = cache.get(name);
		}
		else if (name == "checkerboard"){
			Colorf a, b{1};
			//My checkerboard colors are flipped compared to what Cem's expect
			c = elem->FirstChildElement("color1");
			if (c){
				read_color(c, b);
			}
			c = elem->FirstChildElement("color2");
			if (c){
				read_color(c, a);
			}
			name = "__" + mat_name + "_" + name + "_tex";
			cache.add(name, std::make_unique<CheckerboardTexture>(a, b, std::make_unique<UVMapping>(scale, translate)));
			tex = cache.get(name);

			std::cout << "Texture " << name
				<< " scale: " << scale << ", translate: " << translate
				<< std::endl;
		}
		else {
			std::cout << "Procedural texture " << name << " not implemented\n";
			cache.add(name, std::make_unique<ConstantTexture>(Colorf{0, 0, 0}));
			tex = cache.get(name);
		}
	}
	std::cout << "name: " << name << std::endl;
	return tex;
}

