#ifndef LOAD_FILTER_H
#define LOAD_FILTER_H

#include <memory>
#include <tinyxml2.h>
#include "filters/filter.h"

/*
 * Load the filter for the scene from the <config> tag
 * XMLElement passed should be the element for the <config> tag
 */
std::unique_ptr<Filter> load_filter(tinyxml2::XMLElement *elem);

#endif

