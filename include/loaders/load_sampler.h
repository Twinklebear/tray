#ifndef LOAD_SAMPLER_H
#define LOAD_SAMPLER_H

#include <memory>
#include <tinyxml2.h>
#include "samplers/sampler.h"

/*
 * Load sampler configuration from the config tag passed.
 * Sampler information is loaded from the <sampler> tag
 * also takes the image width and height to set the sampler to sample
 */
std::unique_ptr<Sampler> load_sampler(tinyxml2::XMLElement *elem, size_t w, size_t h);

#endif

