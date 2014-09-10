#include <iostream>
#include <cmath>
#include "lights/light.h"
#include "material/blinn_phong.h"

BlinnPhong::BlinnPhong(const Colorf &diffuse, const Colorf &specular, float gloss)
	: diffuse(diffuse), specular(specular), gloss(gloss)
{}
Colorf BlinnPhong::shade(const Ray &r, const HitInfo &hitinfo, const std::vector<Light*> &lights) const {
	Colorf illum;
	for (auto light : lights){
		if (light->type() == LIGHT::AMBIENT){
			illum += diffuse * light->illuminate(hitinfo.point);
		}
		else {
			Vector l = -light->direction(hitinfo.point).normalized();
			Vector v = -r.d.normalized();
			Vector h = (l + v).normalized();
			//Normal may not be normalized due to translation into world space
			Normal n = hitinfo.normal.normalized();
			float dif = std::max(l.dot(n), 0.f);
			if (dif == 0.f){
				continue;
			}
			float spec = std::pow(std::max(n.dot(h), 0.f), gloss);
			illum += diffuse * dif * light->illuminate(hitinfo.point)
				+ specular * spec * light->illuminate(hitinfo.point);
		}
	}
	illum.normalize();
	return illum;
}

