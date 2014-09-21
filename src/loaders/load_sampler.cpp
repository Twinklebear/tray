#include <iostream>
#include <memory>
#include <string>
#include <tinyxml2.h>
#include "loaders/load_sampler.h"
#include "samplers/sampler.h"
#include "samplers/uniform_sampler.h"
#include "samplers/stratified_sampler.h"
#include "samplers/ld_sampler.h"

std::unique_ptr<Sampler> load_sampler(tinyxml2::XMLElement *elem, size_t w, size_t h){
	tinyxml2::XMLElement *s = elem->FirstChildElement("sampler");
	if (!s){
		return std::make_unique<UniformSampler>(0, w, 0, h);
	}
	std::string type = s->Attribute("type");
	if (type == "uniform"){
		std::cout << "Using UniformSampler\n";
		return std::make_unique<UniformSampler>(0, w, 0, h);
	}
	if (type == "stratified"){
		int spp = s->IntAttribute("spp");
		std::cout << "Using StratifiedSampler with " << spp * spp << " samples per pixel\n";
		return std::make_unique<StratifiedSampler>(0, w, 0, h, spp);
	}
	if (type == "lowdiscrepancy"){
		int spp = s->IntAttribute("spp");
		std::cout << "Using LDSampler with " << spp << " samples per pixel\n";
		return std::make_unique<LDSampler>(0, w, 0, h, spp);
	}
	std::cout << "Error: unrecognized sampler type, defaulting to UniformSampler\n";
	return std::make_unique<UniformSampler>(0, w, 0, h);
}

