#ifndef LOAD_SCENE_H
#define LOAD_SCENE_H

#include <string>
#include <tinyxml2.h>
#include "linalg/vector.h"

/*
 * Load a scene as described by the XML document and return it
 * TODO: Write scene class, and return it instead of void
 */
void load_scene(const std::string &file);
/*
 * Read the x,y,z attributes of the XMLElement and return it
 */
Vector read_vector(tinyxml2::XMLElement *elem);
/*
 * Read the value float attribute of the XMLElement and return it
 */
float read_value(tinyxml2::XMLElement *elem);

#endif

