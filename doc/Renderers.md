Renderers
===
This doc covers the usage of the various renderers implemented by the ray tracer.

Whitted Recursive Ray Tracing
---
Implements the Whitted recursive ray tracing algorithm, the renderer takes a max depth parameter which specifies the recursion depth at which rays should be terminated [Whitted 80](http://dl.acm.org/citation.cfm?id=358882).
```XML
<renderer type="whitted" max_depth="8"/>
```

Path Tracing
---
The path tracing renderer uses forward path tracing with explicit light sampling [Kajiya 86](http://dl.acm.org/citation.cfm?id=15902). The renderer takes both a min and max depth parameter for path lengths. Assuming that a next vertex is always found along the path, paths will be required to be min depth long and be terminated at max depth. Between min and max paths are terminated with Russian roulette based on their luminance.
```XML
<renderer type="path" min_depth="3" max_depth="8"/>
```

Bidirectional Path Tracing
---
The bidirectional renderer traces a path from the light and from the eye and combines visible vertices along each to form paths bringing illumination to the camera [Veach 97 ](htt9ps://graphics.stanford.edu/papers/veach_thesis/). The renderer takes both a min and max depth parameter for path lengths. Assuming that a next vertex is always found along the path, paths will be required to be min depth long and be terminated at max depth. Between min and max paths are terminated with Russian roulette based on their luminance.
```XML
<renderer type="bidir" min_depth="3" max_depth="8"/>
```

Photon Mapping
---
The photon mapping renderer uses two passes, one to shot and collect photons at surfaces and a second to render the scene computing indirect and caustic illumination with the photon maps [Jensen 96](http://graphics.ucsd.edu/~henrik/papers/photon_map/). The photon mapping renderer takes a bit more parameters than the others, listed below.

- max_depth : Recursion depth for rays, as in Whitted
- num_caustic : The number of caustic photons to shoot (note that more or fewer may actually be stored)
- num_indirect : The number of indrect photons to shoot (note that more of fewer may actually be stored)
- query_size : The number of photons to be used per lookup in the map when computing illumination
- max_dist_sqr : The squared max distance to look up photons in the photon maps
- final_gather_samples : The number of Monte Carlo samples to take when performing final gathering. Setting to 0 will disable final gathering.
- gather_angle : Angle of the cone to sample final gather rays in
- max_radiance_dist : The largest distance to look when finding a radiance photon for final gathering. This defaults to infinity, it's recommended to set it lower for performance reasons but the exact number depends on how many indirect photons are fired and the scene.


```XML
<renderer type="photon" max_depth="6" max_photon_depth="16"
	num_caustic="1000000" num_indirect="2000000"
	final_gather_samples="40" query_size="100"
	max_dist_sqr="0.1" max_radiance_dist="1"/>
```

