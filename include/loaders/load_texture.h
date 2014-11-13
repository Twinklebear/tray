#ifndef LOAD_TEXTURE_H
#define LOAD_TEXTURE_H

#include <string>
#include <tinyxml2.h>
#include "textures/texture.h"

/*
 * Load the texture for the XML element into the texture cache
 * and return a non-owning pointer to it. If the item is
 * already in the cache the existing entry will be returned
 * name specifies the material name, to be used for prefixing
 * generated texture names
 * Also takes the XML file name so we can construct paths to the textures
 */
Texture* load_texture(tinyxml2::XMLElement *elem, const std::string &mat_name,
	TextureCache &cache, const std::string &file);
/*
 * Load the texture color as a constant texture by computing the RGB color
 * from the sampled spectrum data in the spd file. spd files are from PBRT,
 * the ones supported (metals only) can be found at https://github.com/mmp/pbrt-v2/tree/master/scenes/spds/metals
 */
Texture* load_spd(tinyxml2::XMLElement *elem, const std::string &mat_name,
	TextureCache &cache, const std::string &file);

#endif

