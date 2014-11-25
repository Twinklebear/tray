#ifndef PHOTON_MAP_INTEGRATOR_H
#define PHOTON_MAP_INTEGRATOR_H

#include <vector>
#include <memory>
#include <atomic>
#include "samplers/sampler.h"
#include "monte_carlo/distribution1d.h"
#include "accelerators/kd_point_tree.h"
#include "film/color.h"
#include "integrator/surface_integrator.h"

/*
 * Surface integrator that computes illumination at surfaces using photon mapping,
 * following the photon mapping implementation described in PBR
 */
class PhotonMapIntegrator : public SurfaceIntegrator {
	enum MAP_TYPE {CAUSTIC, INDIRECT, DIRECT};
	/*
	 * A photon stored in the photon mapping process with its location,
	 * weight and incident vector
	 */
	struct Photon {
		Point position;
		Colorf weight;
		Vector w_i;
	};

	/*
	 * Information about a photon that was found during a query of the maps, the photon
	 * found and the square distance from the photon to the query point
	 * Can also be sorted by their distance for finding the closest/farthest photon we found
	 */
	struct NearPhoton {
		const Photon *photon;
		float dist_sqr;

		bool operator<(const NearPhoton &b) const;
	};

	/*
	 * Radiance photons are used to map the reflected radiance by a photon which
	 * we use in final gathering to quickly compute the light being emitted at some surface
	 */
	struct RadiancePhoton {
		Point position;
		Normal normal;
		Colorf emit;
	};

	/*
	 * Object that handles the shooting of photons and reporting them back to the
	 * integrator when computing the photon maps
	 */
	struct ShootingTask {
		PhotonMapIntegrator &integrator;
		const Scene &scene;
		const Distribution1D &light_distrib;
		std::unique_ptr<Sampler> sampler;
		//The photons that this task has shot
		std::vector<Photon> caustic_photons, indirect_photons, direct_photons;
		//Radiance photons fired by the task along with their reflectance and transmittances
		//for computing the final radiance once the photon maps are complete
		std::vector<RadiancePhoton> radiance_photons;
		std::vector<Colorf> radiance_reflectance, radiance_transmittance;

		/*
		 * Create the photon shooting task for the integrator on the scene also passing a seed
		 * for the sampler to generate random samples for photons
		 */
		ShootingTask(PhotonMapIntegrator &integrator, const Scene &scene, const Distribution1D &light_distrib, int seed);
		/*
		 * Run the shooting task to generate samples of photons of each type
		 */
		void shoot();
		/*
		 * Trace a single photon's path through the scene depositing it on surfaces as we go
		 */
		void trace_photon(const RayDifferential &r, Colorf weight, bool caustic_done, bool indirect_done,
			Sampler &sampler, MemoryPool &pool);
	};
	friend struct ShootingTask;

	/*
	 * Object that handles the computation of radiance values for a subset of the radiance photons
	 */
	struct RadianceTask {
		const PhotonMapIntegrator &integrator;
		int begin, end;
		std::vector<RadiancePhoton> &radiance_photons;
		const std::vector<Colorf> &radiance_reflectance, &radiance_transmittance;

		/*
		 * Create the radiance task to compute the subset of radiance photons from [begin, end)
		 */
		RadianceTask(const PhotonMapIntegrator &integrator, int begin, int end, std::vector<RadiancePhoton> &radiance_photons,
			const std::vector<Colorf> &radiance_reflectance, const std::vector<Colorf> &radiance_transmittance);
		/*
		 * Compute the radiance for the subset of photons assigned to this task
		 */
		void compute();
	};
	friend struct RadianceTask;

	/*
	 * Object that handles construction of one of the photon maps in the background
	 */
	struct TreeBuildTask {
		PhotonMapIntegrator &integrator;
		std::vector<Photon> photons;
		MAP_TYPE type;

		/*
		 * Create the tree building task and specify the photons to use to build the desired tree
		 * note that the photons will be consumed by the build process and moved into the tree
		 */
		TreeBuildTask(PhotonMapIntegrator &integrator, std::vector<Photon> &&photons, MAP_TYPE type);
		/*
		 * Build the tree and set the integrator's unique_ptr for the tree that was built
		 */
		void build();
	};
	friend struct TreeBuildTask;

	/*
	 * Stores information about a photon query as it's performed on a map
	 */
	struct PhotonQueryCallback {
		NearPhoton *queried_photons;
		int query_size, found;

		void operator()(const Point &pos, const Photon &photon, float dist_sqr, float &max_dist_sqr);
	};

	/*
	 * Stores information about a radiance photon query, where we just want the nearest radiance photon
	 * that is on the side we're sampling
	 */
	struct RadianceQueryCallback {
		const Normal &normal;
		const RadiancePhoton *photon;

		void operator()(const Point &pos, const RadiancePhoton &p, float dist_sqr, float &max_dist_sqr);
	};

	//The desired number of caustic/indirect photons we want
	int num_caustic_wanted, num_indirect_wanted, max_depth;
	//Atomic counters to synchronize the number of caustic and indirect photons mapped so far
	std::atomic<int> num_caustic, num_indirect, num_direct;
	std::unique_ptr<KdPointTree<Photon>> caustic_map, indirect_map, direct_map;
	std::unique_ptr<KdPointTree<RadiancePhoton>> radiance_map;
	//Some default configuration values for the photon mapping, TODO: make configurable?
	const int query_size = 50, final_gather_samples = 32;
	const float max_dist_sqr = 0.1, gather_angle = 10;

public:
	/*
	 * Create the photon mapping integrator, specifying the number of desired
	 * caustic and indirect photons and the max depth for paths and photons
	 */
	PhotonMapIntegrator(int num_caustic_wanted, int num_indirect_wanted, int max_depth);
	/*
	 * Pre-process the scene and build the photon maps needed for rendering
	 */
	void preprocess(const Scene &scene) override;
	/*
	 * Compute the illumination at a point on the surface in the scene
	 */
	Colorf illumination(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		DifferentialGeometry &dg, Sampler &sampler, MemoryPool &pool) const;

private:
	/*
	 * Compute indirect illumination contribution using final gathering, this is done by combining 
	 * sampling a distribution based on the BSDF and a distribution based on the
	 * average indirect illumination direction
	 */
	Colorf final_gather(const Scene &scene, const Renderer &renderer, const RayDifferential &ray,
		const Point &p, const Normal &n, const BSDF &bsdf, Sampler &sampler, MemoryPool &pool) const;
	/*
	 * Run the photon shooting tasks and merge their results into the vectors passed
	 */
	void shoot_photons(std::vector<Photon> &caustic_photons, std::vector<Photon> &indirect_photons,
		std::vector<Photon> &direct_photons, std::vector<RadiancePhoton> &radiance_photons,
		std::vector<Colorf> &radiance_reflectance, std::vector<Colorf> &radiance_transmittance, const Scene &scene);
	/*
	 * Build the caustic, indirect and direct photon maps in parallel
	 */
	void build_maps(std::vector<Photon> &caustic_photons, std::vector<Photon> &indirect_photons,
		std::vector<Photon> &direct_photons);
	/*
	 * Compute the irradiance the hemisphere at the point (centered about the normal) using
	 * the photons in the map provided
	 * The buffer to store the found photons in is also provided to avoid having to allocate it every call
	 * num_paths: total number of paths traced for photons in the map
	 * query_size: number of near photons desired, near_photons should have at least this much room
	 */
	static Colorf photon_irradiance(const KdPointTree<Photon> &photons, int num_paths, int query_size,
		NearPhoton *near_photons, float max_dist_sqr, const Point &p, const Normal &n);
	/*
	 * Compute the radiance of the photons in the map reflecting of the BSDF passed in the direction w_o
	 * num_paths: total number of paths traced for photons in the map
	 * query_size: number of near photons desired, near_photons should have at least this much room
	 */
	static Colorf photon_radiance(const KdPointTree<Photon> &photons, int num_paths, int query_size,
		NearPhoton *near_photons, float max_dist_sqr, BSDF &bsdf, Sampler &sampler, MemoryPool &pool,
		const DifferentialGeometry &dg, const Vector &w_o);
};

#endif

