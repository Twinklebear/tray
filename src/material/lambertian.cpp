#include "linalg/util.h"
#include "material/lambertian.h"

Lambertian::Lambertian(const Colorf &reflectance)
	: BxDF(BxDFTYPE(BxDFTYPE::REFLECTION | BxDFTYPE::DIFFUSE)), reflectance(reflectance)
{}
Colorf Lambertian::operator()(const Vector&, const Vector&) const {
	return reflectance * INV_PI;
}
Colorf Lambertian::rho_hd(const Vector&, const std::array<float, 2>*, int) const {
	return reflectance;
}
Colorf Lambertian::rho_hh(const std::array<float, 2>*, const std::array<float, 2>*, int) const {
	return reflectance;
}

