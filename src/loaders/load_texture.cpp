#include <regex>
#include <memory>
#include <string>
#include <tinyxml2.h>
#include "textures/constant_texture.h"
#include "loaders/load_scene.h"
#include "loaders/load_texture.h"

Texture* load_texture(tinyxml2::XMLElement *elem, const std::string &mat_name,
	TextureCache &cache)
{
	using namespace tinyxml2;
	static std::regex match_file{"\..{3}$"};
	std::string tex_name;
	if (elem->Attribute("texture")){
		tex_name = elem->Attribute("texture");
	}
	else {
		tex_name = "__" + mat_name + "_" + elem->Value() + "_tex";
	}
	std::cout << "tex_name: " << tex_name << std::endl;
	Texture *tex = cache.get(tex_name);
	if (!tex){
		//If it's a generated texture we're just setting a constant color
		//for a material
		if (tex_name.substr(0, 2) == "__"){
			Colorf color{1, 1, 1};
			read_color(elem, color);
			cache.add(tex_name, std::make_unique<ConstantTexture>(color));
			tex = cache.get(tex_name);
		}
		//We need to load a texture file or procedural texture
		else {
			std::cout << "Currently unsupported texture " << tex_name << std::endl;
		}
	}
	return tex;
}

