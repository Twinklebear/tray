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
		ShootingTask(PhotonMapIntegrator &integrator, const Scene &scene, const Distribution1D &light_distrib, float seed);
		/*
		 * Run the shooting task to generate samples of photons of each type
		 */
		void shoot();
		/*
		 * Trace a single photon's path through the scene depositing it on surfaces as we go
		 */
		void trace_photon(const RayDifferential &r, Colorf weight, bool caustic_done, bool indirect_done, Sampler &sampler, MemoryPool &pool);
	};
	friend struct ShootingTask;

	//The desired number of caustic/indirect photons we want
	int num_caustic_wanted, num_indirect_wanted, max_depth;
	//Atomic counters to synchronize the number of caustic and indirect photons mapped so far
	std::atomic<int> num_caustic, num_indirect;
	std::unique_ptr<KdPointTree<Photon>> caustic_map, indirect_map, direct_map;
	std::unique_ptr<KdPointTree<RadiancePhoton>> radiance_map;

public:
	/*
	 * Create the photon mapping integrator, specifying the number of desired
	 * caustic and indirect photons and the max depth for paths and photons
	 */
	PhotonMapIntegrator(int num_caustic, int num_indirect, int max_depth);
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
	 * Run the photon shooting tasks and merge their results into the vectors passed
	 */
	void shoot_photons(std::vector<Photon> &caustic_photons, std::vector<Photon> &indirect_photons,
		std::vector<Photon> &direct_photons, std::vector<RadiancePhoton> &radiance_photons,
		std::vector<Colorf> &radiance_reflectance, std::vector<Colorf> &radiance_transmittance, const Scene &scene);
};

#endif

