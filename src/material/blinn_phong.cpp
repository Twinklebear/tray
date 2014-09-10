#include <cmath>
#include "lights/light.h"
#include "material/blinn_phong.h"


BlinnPhong::BlinnPhong(const Colorf &diffuse, const Colorf &specular, float gloss, const Colorf &reflection,
	const Colorf &refraction, const Colorf &absorption, float refr_index)
	: diffuse(diffuse), specular(specular), reflection(reflection), refraction(refraction),
		absorption(absorption), gloss(gloss), refr_index(refr_index)
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
bool BlinnPhong::is_reflective() const {
	return reflection.r > 0 || reflection.g > 0 || reflection.b > 0;
}
Colorf BlinnPhong::reflective() const {
	return reflection;
}
bool BlinnPhong::is_transparent() const {
	return refr_index > 0;
}
float BlinnPhong::refractive_idx() const {
	return refr_index;
}
Colorf BlinnPhong::refractive() const {
	return refraction;
}

