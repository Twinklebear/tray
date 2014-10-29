#ifndef LIGHT_H
#define LIGHT_H

#include "cache.h"
#include "linalg/point.h"
#include "linalg/vector.h"
#include "film/color.h"

/*
 * Light types
 */
enum class LIGHT { AMBIENT, DIRECT, POINT };

/*
 * Basic interface for lights
 */
class Light {
	LIGHT ltype;

public:
	/*
	 * Get the illumination from this light at the point p
	 */
	virtual Colorf illuminate(const Point &p) const = 0;
	/*
	 * Get the direction the light is coming to the point from,
	 * will not necessarily be normalized
	 */
	virtual Vector direction(const Point &p) const = 0;
	virtual LIGHT type() const = 0;
};

typedef Cache<Light> LightCache;

#endif

