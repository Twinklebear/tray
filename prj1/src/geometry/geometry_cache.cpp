#include <string>
#include <memory>
#include <unordered_map>
#include "geometry/geometry.h"
#include "geometry/geometry_cache.h"

void GeometryCache::add(const std::string &name, std::unique_ptr<Geometry> geom){
	cache[name] = std::move(geom);
}
const Geometry* GeometryCache::get(const std::string &name) const {
	auto fnd = cache.find(name);
	if (fnd != cache.end()){
		return fnd->second.get();
	}
	return nullptr;
}
Geometry* GeometryCache::get(const std::string &name){
	auto fnd = cache.find(name);
	if (fnd != cache.end()){
		return fnd->second.get();
	}
	return nullptr;
}

