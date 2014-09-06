#include <cmath>
#include "lights/light.h"
#include "material/blinn_phong.h"

BlinnPhong::BlinnPhong(const Colorf &diffuse, const Colorf &specular, float gloss)
	: diffuse(diffuse), specular(specular), gloss(gloss)
{}
Colorf BlinnPhong::shade(const Ray &r, const HitInfo &hitinfo, const LightCache &lights) const {
	Colorf illum;
	for (const auto &lit : lights){
		const auto &light = lit.second;
		if (light->is_ambient()){
			illum += diffuse * light->illuminate(hitinfo.point);
		}
		else {
			Vector l = -light->direction(hitinfo.point);
			Vector v = -r.d.normalized();
			Vector h = (l + v).normalized();
			//Normal may not be normalized due to translation into world space
			Normal n = hitinfo.normal.normalized();
			float dif = std::max(l.dot(n), 0.f);
			float spec = std::pow(std::max(n.dot(h), 0.f), gloss);
			illum += diffuse * dif * light->illuminate(hitinfo.point)
				+ specular * spec * light->illuminate(hitinfo.point);
		}
	}
	illum.normalize();
	return illum;
}

