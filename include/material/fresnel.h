#ifndef FRESNEL_H
#define FRESNEL_H

#include "film/color.h"

/*
 * Implementation of BxDF motivated by PBR
 */
/*
 * Compute the Fresnel reflectance term for dielectric materials
 */
Colorf fresnel_dielectric(float cos_i, float cos_t, float eta_i, float eta_t);
/*
 * Compute the Fresnel reflectance term for conductive materials
 */
Colorf fresnel_conductor(float cos_i, const Colorf &eta, const Colorf &k);

/*
 * Interface for computing Fresnel terms generically
 */
class Fresnel {
public:
	/*
	 * Evaluate the Fresnel reflectance term for light incident along some angle
	 */
	virtual Colorf operator()(float cos_i) const = 0;
};

/*
 * Computes the Fresnel reflectance term for dielectrics using fresnel_dielectric
 */
class FresnelDielectric : public Fresnel {
public:
	const float eta_i, eta_t;

	/*
	 * Construct the Fresnel dielectric evaluator for the interface
	 * between two materials
	 */
	FresnelDielectric(float eta_i, float eta_t);
	/*
	 * Evaluate the Fresnel reflectance term for light incident along some angle
	 */
	Colorf operator()(float cos_i) const override;
};

/*
 * Computes the Fresnel reflectance term for conductors using fresnel_conductor
 */
class FresnelConductor : public Fresnel {
public:
	const Colorf eta, k;

	/*
	 * Construct the Fresnel conductor evaluator for the conductor being hit
	 */
	FresnelConductor(const Colorf &eta, const Colorf &k);
	/*
	 * Evaluate the Fresnel reflectance term for light incident along some angle
	 */
	Colorf operator()(float cos_i) const override;
};

/*
 * No-op Fresnel, simply returns 1
 */
class FresnelNoOp : public Fresnel {
public:
	/*
	 * Evaluate the Fresnel reflectance term for light incident along some angle
	 */
	Colorf operator()(float cos_i) const override;
};

/*
 * FresnelFlip takes the Fresnel value computed by the one passed and returns
 * 1 - fresnel_value, eg. given a reflective Fresnel value this will return the
 * transmission Fresnel value
 */
class FresnelFlip : public Fresnel {
	const Fresnel *fresnel;

public:
	FresnelFlip(const Fresnel *fresnel);
	/*
	 * Evaluate the Fresnel reflectance term for light incident along some angle
	 */
	Colorf operator()(float cos_i) const override;
};

#endif

