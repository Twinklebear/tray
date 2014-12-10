#ifndef BIDIR_PATH_INTEGRATOR_H
#define BIDIR_PATH_INTEGRATOR_H

#include "surface_integrator.h"
#include "renderer/renderer.h"

/*
 * Surface integrator that uses bidirectional path tracing for computing illumination
 * at a a point on the surface
 */
class BidirPathIntegrator : public SurfaceIntegrator {
	const int min_depth, max_depth;

	/*
	 * Struct used to store information about a vertex along a camera or light path
	 * stores the vertex differential geometry, outgoing and incident vectors (both point out of surface)
	 * the BSDF, specularity and throughput information
	 */
	struct PathVertex {
		DifferentialGeometry dg;
		Vector w_o, w_i;
		BSDF *bsdf;
		bool specular_bounce;
		int num_specular_comp;
		Colorf throughput;
	};

public:
	/*
	 * Create the path tracing integrator and set the min and max depth for paths
	 * rays are randomly stopped by Russian roulette after reaching min_depth and are stopped
	 * at max_depth
	 */
	BidirPathIntegrator(int min_depth, int max_depth);
	/*
	 * Compute the illumination at a point on a surface in the scene
	 */
	Colorf illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const override;

private:
	/*
	 * Generate a path of length at least min_depth and at most max_depth, returning the actual
	 * length of the path generated and filling out the path vertex array passed. There must be
	 * room for at least max_depth vertices in the array. Weight is the starting weight of the
	 * path, eg. weight from the camera or light source generating the ray
	 */
	int trace_path(const Scene &scene, const Renderer &renderer, const RayDifferential &ray, const Colorf &weight,
		Sampler &sampler, MemoryPool &pool, PathVertex *path_vertices) const;
	/*
	 * Compute the luminance along the camera path using the regular camera path tracing lighting
	 * computation but with the path vertices for path of length path_len provided in path_vertices
	 */
	Colorf camera_luminance(const Scene &scene, const Renderer &renderer, const PathVertex *path_vertices,
		int path_len, Sampler &sampler, MemoryPool &pool) const;
	/*
	 * Compute the luminance coming back along the camera path by combining the camera and light
	 * paths that were traced previously
	 */
	Colorf bidir_luminance(const Scene &scene, const Renderer &renderer, PathVertex *cam_path, int cam_path_len,
		PathVertex *light_path, int light_path_len, Sampler &sampler, MemoryPool &pool) const;
};

#endif

