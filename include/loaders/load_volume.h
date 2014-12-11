#ifndef LOAD_VOLUMES_H
#define LOAD_VOLUMES_H

#include <string>
#include <tinyxml2.h>
#include "scene.h"
#include "volume/volume.h"
#include "volume/volume_node.h"

/*
 * Load the volume specified by the element passed, returns nullptr if loading failed
 */
Volume* load_volume(tinyxml2::XMLElement *elem, VolumeCache &cache, const std::string &scene_file);
/*
 * Load the volume node at the element
 * will also push the volume's transform onto the stack
 */
void load_volume_node(tinyxml2::XMLElement *elem, Scene &scene, std::stack<Transform> &transform_stack,
	const std::string &file);

#endif

