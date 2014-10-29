#include <cmath>
#include "lights/light.h"
#include "material/blinn_phong.h"

BlinnPhong::BlinnPhong(const Texture *diffuse, const Texture *specular, float gloss, const Texture *reflection,
	const Texture *refraction, const Texture *absorption, float refr_index)
	: diffuse(diffuse), specular(specular), reflection(reflection), refraction(refraction),
		absorption(absorption), gloss(gloss), refr_index(refr_index)
{}
Colorf BlinnPhong::shade(const Ray &r, const DifferentialGeometry &diff_geom, const Light &light) const {
	Colorf illum;
	if (light.type() == LIGHT::AMBIENT){
		illum = diffuse->sample(diff_geom) * light.illuminate(diff_geom.point);
	}
	else {
		Vector l = -light.direction(diff_geom.point).normalized();
		Vector v = -r.d.normalized();
		Vector h = (l + v).normalized();
		//Normal may not be normalized due to translation into world space
		Normal n = diff_geom.normal.normalized();
		float dif = std::max(l.dot(n), 0.f);
		if (dif == 0.f){
			return Colorf{0};
		}
		float spec = std::pow(std::max(n.dot(h), 0.f), gloss);
		illum = diffuse->sample(diff_geom) * dif * light.illuminate(diff_geom.point)
			+ specular->sample(diff_geom) * spec * dif * light.illuminate(diff_geom.point);
	}
	illum.normalize();
	return illum;
}
bool BlinnPhong::is_reflective() const {
	return reflection != nullptr;
}
Colorf BlinnPhong::reflective(const DifferentialGeometry &dg) const {
	return reflection ? reflection->sample(dg) : Colorf{0, 0, 0};
}
bool BlinnPhong::is_transparent() const {
	return refr_index > 0;
}
Colorf BlinnPhong::absorbed(const DifferentialGeometry &dg) const {
	return absorption ? absorption->sample(dg) : Colorf{0, 0, 0};
}
float BlinnPhong::refractive_idx() const {
	return refr_index;
}
Colorf BlinnPhong::refractive(const DifferentialGeometry &dg) const {
	return refraction ? refraction->sample(dg) : Colorf{0, 0, 0};
}

