#include "material/blinn_phong.h"

BlinnPhong::BlinnPhong(const Point &diffuse, const Point &specular, float gloss)
	: diffuse(diffuse), specular(specular), gloss(gloss)
{}
Point BlinnPhong::shade(const Ray &ray) const {
	//Just for debugging
	return diffuse;
}

