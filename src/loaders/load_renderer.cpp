#include <string>
#include <tinyxml2.h>
#include "renderer/renderer.h"
#include "integrator/path_integrator.h"
#include "integrator/whitted_integrator.h"
#include "integrator/bidir_path_integrator.h"
#include "loaders/load_renderer.h"

std::unique_ptr<SurfaceIntegrator> load_surface_integrator(tinyxml2::XMLElement *elem){
	tinyxml2::XMLElement *r = elem->FirstChildElement("renderer");
	if (!r){
		return std::make_unique<PathIntegrator>(3, 8);
	}
	std::string type = r->Attribute("type");
	int max_depth = r->IntAttribute("max_depth");
	if (type == "whitted"){
		return std::make_unique<WhittedIntegrator>(max_depth);
	}
	int min_depth = r->IntAttribute("min_depth");
	if (type == "path"){
		return std::make_unique<PathIntegrator>(min_depth, max_depth);
	}
	else if (type == "bidir"){
		return std::make_unique<BidirPathIntegrator>(min_depth, max_depth);
	}
	std::cout << "Load renderer error: Unrecognized surface integrator " << type << std::endl;
	return std::make_unique<PathIntegrator>(3, 8);
}

