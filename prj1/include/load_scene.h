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

#endif

