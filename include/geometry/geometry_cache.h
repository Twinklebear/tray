#ifndef GEOMETRY_CACHE
#define GEOMETRY_CACHE

#include <string>
#include <memory>
#include <unordered_map>
#include "geometry.h"

/*
 * Caches geometry, really just a hash table of type name -> Geometry
 */
class GeometryCache {
	std::unordered_map<std::string, std::unique_ptr<Geometry>> cache;

public:
	/*
	 * Add some geometry to the cache
	 */
	void add(const std::string &name, std::unique_ptr<Geometry> geom);
	/*
	 * Get a non-owning pointer to some geometry in the cache
	 * nullptr is returned if the name isn't found
	 */
	const Geometry* get(const std::string &name) const;
	Geometry* get(const std::string &name);
};

#endif

