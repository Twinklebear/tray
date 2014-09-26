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
 */
Texture* load_texture(tinyxml2::XMLElement *elem, const std::string &mat_name,
	TextureCache &cache);

#endif

