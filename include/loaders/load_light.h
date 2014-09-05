#ifndef LOAD_LIGHT_H
#define LOAD_LIGHT_H

#include <tinyxml2.h>
#include "lights/light.h"

/*
 * Load all the light information into the light cache.
 * elem should be the first light XML element in the file
 */
void load_lights(tinyxml2::XMLElement *elem, LightCache &cache);

#endif

