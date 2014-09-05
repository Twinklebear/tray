#ifndef LIGHT_H
#define LIGHT_H

#include "cache.h"
#include "linalg/point.h"
#include "linalg/vector.h"
#include "render/color.h"

/*
 * Basic interface for lights
 */
class Light {
public:
	/*
	 * Get the illumination from this light at the point p
	 */
	virtual Colorf illuminate(const Point &p) const = 0;
	/*
	 * Get the direction the light is coming to the point from
	 */
	virtual Vector direction(const Point &p) const = 0;
	virtual bool is_ambient() const = 0;
};

typedef Cache<Light> LightCache;

#endif

