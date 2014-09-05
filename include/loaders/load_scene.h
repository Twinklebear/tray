#ifndef LOAD_SCENE_H
#define LOAD_SCENE_H

#include <string>
#include <tinyxml2.h>
#include "linalg/vector.h"
#include "geometry/geometry.h"
#include "scene.h"

/*
 * Load a scene as described by the XML document and return it
 * Based off of Cem's load scene utility but migrated to TinyXML-2
 */
Scene load_scene(const std::string &file);
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

#endif

