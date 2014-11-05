#ifndef LOAD_MATERIAL_H
#define LOAD_MATERIAL_H

#include <tinyxml2.h>
#include "material/pbr_material.h"
#include "textures/texture.h"

/*
 * Load all the material information into the material cache. elem should
 * be the first material XML element in the file
 * We do this in a pre-pass to avoid having to store the material
 * names on the nodes and do a post-load pass to set up the node
 * materials like is done in Cem's loading code, due to how the XML
 * file is layed out with materials coming after objects
 * Loaded textures for the materials will be placed in the texture cache
 */
void load_materials(tinyxml2::XMLElement *elem, PBRMaterialCache &cache, TextureCache &tcache,
	const std::string &file);

#endif

