#include "samplers/stratified_sampler.h"
#include "material/bsdf.h"

BSDF::BSDF(const DifferentialGeometry &dg, float eta)
	: ns(dg.normal), ng(dg.geom_normal), s(dg.dp_du.normalized()), t(ns.cross(s)), dg(dg), eta(eta)
{}
void BSDF::add(std::unique_ptr<BxDF> b){
	bxdfs.push_back(std::move(b));
}
int BSDF::num_bxdfs() const {
	return bxdfs.size();
}
int BSDF::num_bxdfs(BxDFTYPE flags) const {
	int n = 0;
	for (const auto &b : bxdfs){
		if (b->matches(flags)){
			++n;
		}
	}
	return n;
}
Vector BSDF::to_shading(const Vector &v) const {
	return Vector{v.dot(s), v.dot(t), v.dot(ns)};
}
Vector BSDF::from_shading(const Vector &v) const {
	return Vector{s.x * v.x + t.x * v.y + ns.x * v.z,
		s.y * v.x + t.y * v.y + ns.y * v.z,
		s.z * v.x + t.z * v.y + ns.z * v.z};
}
Colorf BSDF::operator()(const Vector &wo_world, const Vector &wi_world, BxDFTYPE flags) const {
	Vector wo = to_shading(wo_world);
	Vector wi = to_shading(wi_world);
	//Determine if we should be evaluating reflection or transmission based on the geometry normal
	if (wo_world.dot(ng) * wi_world.dot(ng) > 0){
		flags = BxDFTYPE(flags & ~BxDFTYPE::TRANSMISSION);
	}
	else {
		flags = BxDFTYPE(flags & ~BxDFTYPE::REFLECTION);
	}
	Colorf color;
	for (const auto &b : bxdfs){
		if (b->matches(flags)){
			color += (*b)(wo, wi);
		}
	}
	return color;
}
Colorf BSDF::rho_hd(const Vector &wo, Sampler &sampler, BxDFTYPE flags, int sqrt_samples) const {
	std::vector<std::array<float, 2>> samples(sqrt_samples * sqrt_samples);
	sampler.get_samples(samples);
	Colorf color;
	for (const auto &b : bxdfs){
		if (b->matches(flags)){
			color += b->rho_hd(wo, samples);
		}
	}
	return color;
}
Colorf BSDF::rho_hh(Sampler &sampler, BxDFTYPE flags, int sqrt_samples) const {
	std::vector<std::array<float, 2>> samples_a(sqrt_samples * sqrt_samples);
	std::vector<std::array<float, 2>> samples_b(sqrt_samples * sqrt_samples);
	sampler.get_samples(samples_a);
	sampler.get_samples(samples_b);
	Colorf color;
	for (const auto &b : bxdfs){
		if (b->matches(flags)){
			color += b->rho_hh(samples_a, samples_b);
		}
	}
	return color;

}
float BSDF::pdf(const Vector &wo_world, const Vector &wi_world, BxDFTYPE flags) const {
	Vector wo = to_shading(wo_world);
	Vector wi = to_shading(wi_world);
	float pdf_val = 0;
	int n_comps = 0;
	for (const auto &b : bxdfs){
		if (b->matches(flags)){
			++n_comps;
			pdf_val += b->pdf(wo, wi);
		}
	}
	return n_comps > 0 ? pdf_val / n_comps : 0;
}

