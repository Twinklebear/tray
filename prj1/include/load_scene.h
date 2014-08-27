#ifndef LOAD_SCENE_H
#define LOAD_SCENE_H

#include <string>
#include <tinyxml2.h>
#include "linalg/vector.h"
#include "geometry/geometry.h"

/*
 * Load a scene as described by the XML document and return it
 * TODO: Write scene class, and return it instead of void
 */
void load_scene(const std::string &file);
/*
 * Load object nodes in the XMLElement as children of the
 * passed node
 */
void load_node(tinyxml2::XMLElement *elem);
/*
 * Read the x,y,z attributes of the XMLElement and return it
 */
void read_vector(tinyxml2::XMLElement *elem, Vector &v);
/*
 * Read the value float attribute of the XMLElement and return it
 */
void read_float(tinyxml2::XMLElement *elem, float &f);

#endif

