#include <string>
#include <tinyxml2.h>
#include "renderer/renderer.h"
#include "integrator/path_integrator.h"
#include "integrator/whitted_integrator.h"
#include "integrator/bidir_path_integrator.h"
#include "integrator/photon_map_integrator.h"
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
	if (type == "photon"){
		int num_caustic = r->IntAttribute("num_caustic");
		int num_indirect = r->IntAttribute("num_indirect");
		int max_phot_depth = 6, query_size = 50, final_gather_samples = 32;
		float max_dist_sqr = 0.1, gather_angle = 10, max_radiance_dist = -1;
		r->QueryIntAttribute("max_phot_depth", &max_phot_depth);
		r->QueryIntAttribute("query_size", &query_size);
		r->QueryIntAttribute("final_gather_samples", &final_gather_samples);
		r->QueryFloatAttribute("max_dist_sqr", &max_dist_sqr);
		r->QueryFloatAttribute("gather_angle", &gather_angle);
		r->QueryFloatAttribute("max_radiance_dist", &max_radiance_dist);
		return std::make_unique<PhotonMapIntegrator>(num_caustic, num_indirect, max_depth,
			max_phot_depth, query_size, final_gather_samples, max_dist_sqr, gather_angle,
			max_radiance_dist);
	}

	int min_depth = r->IntAttribute("min_depth");
	if (type == "path"){
		return std::make_unique<PathIntegrator>(min_depth, max_depth);
	}
	if (type == "bidir"){
		return std::make_unique<BidirPathIntegrator>(min_depth, max_depth);
	}
	std::cout << "Load renderer error: Unrecognized surface integrator " << type << std::endl;
	return std::make_unique<PathIntegrator>(3, 8);
}

