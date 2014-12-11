#ifndef LOAD_RENDERER_H
#define LOAD_RENDERER_H

#include <memory>
#include <tinyxml2.h>
#include "renderer/renderer.h"
#include "integrator/surface_integrator.h"
#include "integrator/volume_integrator.h"

/*
 * Load the surface integrator from the renderer child of the config element passed
 */
std::unique_ptr<SurfaceIntegrator> load_surface_integrator(tinyxml2::XMLElement *elem);
/*
 * Load the volume integrator from the vol_integrator child of the config element passed
 */
std::unique_ptr<VolumeIntegrator> load_volume_integrator(tinyxml2::XMLElement *elem);

#endif

