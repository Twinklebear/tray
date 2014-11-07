#include <cassert>
#include "samplers/stratified_sampler.h"
#include "material/bsdf.h"

BSDF::BSDF(const DifferentialGeometry &dg, float eta)
	: normal(dg.normal), geom_normal(dg.geom_normal), bitangent(dg.dp_du.normalized()),
	tangent(normal.cross(bitangent).normalized()), dg(dg), eta(eta)
{
	//Update the bitangent to get a proper coordinate system
	bitangent = tangent.cross(Vector{normal}).normalized();
}
void BSDF::add(BxDF *b){
	bxdfs.push_back(b);
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
	return Vector{v.dot(bitangent), v.dot(tangent), v.dot(normal)};
}
Vector BSDF::from_shading(const Vector &v) const {
	return Vector{bitangent.x * v.x + tangent.x * v.y + normal.x * v.z,
		bitangent.y * v.x + tangent.y * v.y + normal.y * v.z,
		bitangent.z * v.x + tangent.z * v.y + normal.z * v.z};
}
Colorf BSDF::operator()(const Vector &wo_world, const Vector &wi_world, BxDFTYPE flags) const {
	Vector wo = to_shading(wo_world);
	Vector wi = to_shading(wi_world);
	//Determine if we should be evaluating reflection or transmission based on the geometry normal
	if (wo_world.dot(geom_normal) * wi_world.dot(geom_normal) > 0){
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
Colorf BSDF::sample(const Vector &wo_world, Vector &wi_world, const std::array<float, 2> &u, float comp,
	float &pdf_val, BxDFTYPE flags, BxDFTYPE *sampled_type) const
{
	//Select which component to sample that matches the desired flags
	int n_matching = num_bxdfs(flags);
	if (n_matching == 0){
		pdf_val = 0;
		if (sampled_type){
			*sampled_type = BxDFTYPE(0);
		}
		return Colorf{0};
	}
	int select = std::min(static_cast<int>(comp * n_matching), n_matching - 1);
	const BxDF *bxdf = matching_at(select, flags);
	assert(bxdf);

	Vector wo = to_shading(wo_world);
	Vector wi;
	pdf_val = 0;
	Colorf f = bxdf->sample(wo, wi, u, pdf_val);
	if (pdf_val == 0){
		if (sampled_type){
			*sampled_type = BxDFTYPE(0);
		}
		return Colorf{0};
	}
	if (sampled_type){
		*sampled_type = bxdf->type;
	}
	wi_world = from_shading(wi);

	//Compute the overall sampling PDF for the direction we sampled. This is skipped for specular BxDFs
	//since they contain a delta distribution which would make this incorrect
	if (!(bxdf->type & BxDFTYPE::SPECULAR) && n_matching > 1){
		pdf_val = pdf(wo_world, wi_world, flags);
		//Compute the total contribution from all BxDFs matching the flags along this direction
		f = (*this)(wo_world, wi_world, flags);
	}
	return f;
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
const BxDF* BSDF::matching_at(int i, BxDFTYPE flags) const {
	for (const auto &b : bxdfs){
		if (b->matches(flags) && i-- == 0){
			return b;
		}
	}
	return nullptr;
}

