#ifndef LOAD_RENDERER_H
#define LOAD_RENDERER_H

#include <memory>
#include <tinyxml2.h>
#include "renderer/renderer.h"
#include "integrator/surface_integrator.h"

/*
 * Load the renderer from the renderer child of the config element passed
 */
std::unique_ptr<SurfaceIntegrator> load_surface_integrator(tinyxml2::XMLElement *elem);

#endif

