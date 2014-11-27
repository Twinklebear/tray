Renderers
===
TODO: Clean up doc

You can specify which renderer you want to have used to render your scene. Currently Whitted, path tracing and bidirectional
path tracing are supported.

**Whitted Recursive Ray Tracing**
Use Whitted recursive ray tracing
```XML
<!-- max_depth specifies the max ray recursion depth -->
<renderer type="whitted" max_depth="8"/>
```

**Path Tracing**
Use path tracing with explicit light sampling
```XML
<!-- min/max depth specify the min/max path lengths -->
<renderer type="path" min_depth="3" max_depth="8"/>
```

**Bidirectional Path Tracing**
Use bidirectional path tracing
```XML
<!-- min/max depth specify the min/max path lengths -->
<renderer type="bidir" min_depth="3" max_depth="8"/>
```

**Photon Mapping**
Use photon mapping
```XML
<!-- max depth specifies max bounces for photons and depth for rays
	num_caustic/indirect specify the desired number of caustic/indirect photons to shoot.
	note that less than this number may be created depening on the scene
	query_size: optionally specify number of photons to query from the maps, default 50
	final_gather_samples: optionally specify the number of final gather rays to shoot, default 32
	max_dist_sqr: optionally specify the square distance to look for photons in the map, default 0.1
	gather_angle: optionally specify the angle of the cone for final gathering, default 10.0 -->
<renderer type="photon" max_depth="8" num_caustic="1000000" num_indirect="1000000" query_size="50"
	final_gather_samples="32" max_dist_sqr="0.1" gather_angle="10"/>
```

