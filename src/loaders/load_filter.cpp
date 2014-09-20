#include <iostream>
#include <memory>
#include <tinyxml2.h>
#include "filters/filter.h"
#include "filters/box_filter.h"
#include "filters/triangle_filter.h"
#include "filters/gaussian_filter.h"
#include "filters/mitchell_filter.h"
#include "filters/lanczos_sinc_filter.h"

std::unique_ptr<Filter> load_filter(tinyxml2::XMLElement *elem){
	tinyxml2::XMLElement *f = elem->FirstChildElement("filter");
	if (!f){
		return std::make_unique<BoxFilter>(0.5, 0.5);
	}
	std::string type = f->Attribute("type");
	float w = f->FloatAttribute("w");
	float h = f->FloatAttribute("h");
	if (type == "box"){
		std::cout << "Using BoxFilter\n";
		return std::make_unique<BoxFilter>(w, h);
	}
	if (type == "triangle"){
		std::cout << "Using TriangleFilter\n";
		return std::make_unique<TriangleFilter>(w, h);
	}
	if (type == "gaussian"){
		std::cout << "Using GaussianFilter\n";
		float alpha = f->FloatAttribute("alpha");
		return std::make_unique<GaussianFilter>(w, h, alpha);
	}
	if (type == "mitchell"){
		std::cout << "Using MitchellFilter\n";
		float b = f->FloatAttribute("b");
		float c = f->FloatAttribute("c");
		return std::make_unique<MitchellFilter>(w, h, b, c);
	}
	if (type == "lanczos"){
		std::cout << "Using LanczosSincFilter\n";
		float a = f->FloatAttribute("a");
		return std::make_unique<LanczosSincFilter>(w, h, a);
	}
	std::cout << "Error: unrecognized filter type, defaulting to BoxFilter{0.5, 0.5}\n";
	return std::make_unique<BoxFilter>(0.5, 0.5);
}

