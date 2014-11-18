CS6620 - Will Usher
=
The project should build easily with CMake but does require a C++14 compliant compiler.
I did choose to use TinyXML2 instead of TinyXML1, since the library is so small I've included its source under tinyxml2/
so there are no additional dependencies that need to be downloaded to build the renderer.

Select the build mode when compiling with CMake, `-DCMAKE_BUILD_TYPE=Debug` will build with debugging symbols
while `-DCMAKE_BUILD_TYPE=Release` will compile with full optimizations.

If you want to build the renderer with the live preview you'll need [SDL2](http://libsdl.org/) installed and
can then run CMake with the `-DBUILD_PREVIEWER=1` flag to compile the previewer. To view the live preview
when rendering run with the `-p` flag.

Textures
-
The ray tracer supports PNG (8bit channel only), BMP (non-1bpp, non-RLE), and PPM image formats for textures.
Note that texture names beginning with \_\_ are reserved for generated texture names the ray tracer may need to create
and should not be used. If you want to compare trilinear filtering to EWA filtering for textures you can pass
`-DTEX_TRILINEAR=1` to indicate that trilinear filtering should be used for texture sampling instead of the default (EWA).

Materials
-
The ray tracer uses are very different material model than what is discussed in class, aiming to be more physically accurate.
As such no BlinnPhong material is provided. The supported materials are listed below along with the parameters they require.
Note that material names (as with all other names) should not conflict.

**Matte Material**
```XML
<material type="matte" name="my_matte_material">
	<!-- The color (or texture) for the material, required -->
	<diffuse r="1" g="0" b="0.5"/>
</material>
```

**Plastic Material**
```XML
<material type="plastic" name="my_plastic_material">
	<!-- The diffuse color (or texture) for the material, required -->
	<diffuse  r="0.8" g="0.2" b="0.2"/>
	<!-- The color (or texture) of specular highlights of the material, required -->
	<specular r="1.0" g="1.0" b="1.0" value="0.9"/>
	<!-- The material's roughness between (0, 1], defaults to 1 (roughest) -->
	<roughness value="0.5"/>
</material>
```

**Translucent Material**
Translucent material colors are computed by multiplying the base colors with the effect occuring, eg. the color
for diffuse reflection is `reflection * diffuse` and so on.
```XML
<material type="translucent" name="my_translucent_material">
	<!-- The base diffuse color (or texture) for the material, required -->
	<diffuse  r="0.8" g="0.2" b="0.2"/>
	<!-- The base color (or texture) of specular highlights of the material, required -->
	<specular r="1.0" g="1.0" b="1.0" value="0.9"/>
	<!-- The color (or texture) for reflections, required -->
	<reflection r="1.0" g="0.4" b="1.0" value="0.9"/>
	<!-- The color (or texture) for transmission, required -->
	<transmission r="1.0" g="1.0" b="0.2"/>
	<!-- The material's roughness between (0, 1], defaults to 1 (roughest) -->
	<roughness value="0.5"/>
	<!-- The material's index of refraction, defaults to 1 (air) -->
	<ior value="1.6"/>
</material>
```

**Metal Material**
```XML
<material type="metal" name="my_metal_material">
	<!-- The metal's index of refraction (can be a texture), required -->
	<ior r="1.3" g="1.1" b="0.4"/>
	<!-- The metal's absoprtion coefficient (can be a texture), required -->
	<absorption r="1.9" g="2.5" b="4.3"/>
	<!-- The material's roughness between (0, 1], defaults to 1 (roughest)
		 can also specify x="0.01" y="0.09" to get an anisotropic metal -->
	<roughness value="0.01"/>
</material>
```
Metals can also be specified using PBRT's [spd](https://github.com/mmp/pbrt-v2/tree/master/scenes/spds/metals) files which
contain measured metal properties over the visible light spectrum.
```XML
<material type="metal" name="my_pbrt_metal">
	<ior spd="./spds/Cu.eta.spd"/>
	<absorption spd="./spds/Cu.k.spd"/>
	<roughness value="0.000976"/>
</material>
```

**MERL Material**
The renderer supports materials from the [MERL BRDF database](http://www.merl.com/brdf/) from "A Data-Driven Reflectance Model", by Wojciech Matusik, Hanspeter Pfister, Matt Brand and Leonard McMillan which appeared in ACM Transactions on Graphics 22, 3(2003), 759-769.
```XML
<material type="merl" name="my_merl_material" file="./brdfs/alumina-oxide.binary"/>
```

**Glass Material**
```XML
<material type="glass" name="my_glass_mat">
	<!-- Color and strength of reflections -->
	<reflection r="1" g="0.5" b="0.8"/>
	<!-- Color and strength of transmission -->
	<transmission r="0.4" g="0.2" b="1"/>
	<!-- Index of refraction of the material, defaults to 1 (air) -->
	<ior value="1.52"/>
</material>
```

**Mix Material**
Specify that the material is constructed by mixing two previously declared materials.
```XML
<material type="mix" name="my_mix_mat">
	<material name="mix_mat_a" />
	<material name="mix_mat_b" />
	<!-- Materials are mixed by scaling the BxDFs by the color here
		mat_a is scaled by scale, mat_b is scaled by 1 - scale -->
	<scale r="0.2" g="0.8" b="0.5"/>
</material>
```

Lights
-
**Point Lights**
```XML
<light type="point" name="my_point_light">
	<!-- Position for the point light, required -->
	<position x="0" y="10" z="18"/>
	<!-- Emitted light color, required -->
	<intensity r="0.5" g="0.25" b="1"/>
</light>
```

**Area Lights**
An area light is attached to some geometry in the scene, making it emissive. Currently only spheres are supported.
The object is specified as normal but should not have a scaling attribute as the system has explicit assumptions that
light transformation matrices don't have scaling.
```XML
<!-- To adjust the size of the sphere light don't scale, change the radius -->
<object type="sphere" name="sphere_light" radius="1">
	<!-- Translate and rotate the light as a normal object -->
	<translate x="0" y="-10" z="22"/>
	<!-- Specify the light to be attached, only area lights are valid, a name is auto-generated -->
	<light type="area">
		<!-- Emitted light color, required -->
		<intensity r="0.5" g="0.25" b="1"/>
		<!-- Number of samples to take of the light, used in photon mapping. Default 6 -->
		<nsamples value="4"/>
	</light>
</object>
```


Extras
-
The ray tracer also supports some extra scene options that can be specified within a <config> block in the scene file.

Objects in Motion (Not currently supported)
-
Objects can be specified to be in motion during the rendering, resulting in motion blur. To do this you can specify the
start and end transformations along with their time points and specify the camera shutter open and close times.

The object transformation should look like:
```XML
<start time="1.5">
    <scale value="1.5"/>
	<rotate angle="45" x="1" y="0" z="0"/>
	<translate x="2" y="1" z="0"/>
</start>
<end time="5.5">
    <scale value="2"/>
	<rotate angle="0" x="1" y="0" z="0"/>
	<translate x="0" y="3" z="0"/>
</end>
```

The camera shutter open and close times can be specified as:
```XML
<camera>
    <!-- Your other camera properties -->
	<shutter open="0.5" close="5.5"/>
</camera>
```

Renderers
-
You can specify which renderer you want to have used to render your scene. Currently Whitted, path tracing and bidirectional
path tracing are supported.

- whitted - Use Whitted recursive ray tracing to render
	- `max_depth` - max recursion depth for rays
- path - Use camera path tracing to render
	- `max_depth` - max path length
	- `min_depth` - minimum path length
- bidir - Use bidirectional path tracing to render
	- `max_depth` - max path length
	- `min_depth` - minimum path length

Samplers
-
The type of sampler used to render the scene can be configured by the `<sampler type="">` tag. Available samplers and their
parameters are listed below by the type string that selects them.

- stratified - Selects a stratified sampler which renders the image using some desired number of jittered samples per pixel.
	- spp - number of samples to be taken per pixel in x & y, eg. sampler takes spp\*spp samples per pixel
- lowdiscrepancy - Selects a low-discrepancy which uses a (0, 2) sequence to generate sample positions
	- spp - number of samples to be taken per pixel
- adaptive - Selects the number of samples to take based on variance of the luminance of a lower number of samples. Uses
	(0, 2) sequence to pick sample positions.
	- min - number of samples to take when determining variance in luminance
	- max - number of samples to take in high-variance regions

Filters
-
The type of filter used when reconstructing the image can be configured by the `<filter type="">` tag. Available filters
and their parameters are listed below by the type string that selects them. The width/height of a single pixel is 0.5.

- box - Select a standard box filter (Default)
	- w - width of the filter to apply (Default 0.5)
	- h - height of the filter to apply (Default 0.5)
- triangle - Select a triangle filter
	- w - width of the filter to apply
	- h - height of the filter to apply
- gaussian - Select a Gaussian filter
	- w - width of the filter to apply
	- h - height of the filter to apply
	- alpha - alpha value for the Gaussian function
- mitchell - Select a Mitchell filter
	- w - width of the filter to apply
	- h - height of the filter to apply
	- b - b parameter for the filter
	- c - c parameter for the filter
- lanczos - Select a Lanczos filter
	- w - width of the filter to apply
	- h - height of the filter to apply
	- a - a parameter for the filter window size

Dependencies
-
- [SDL2](http://libsdl.org/) is used for the live previewer.
- [TinyXML-2](https://github.com/leethomason/tinyxml2) is used to parse the scene files.
- [stb_image](https://github.com/nothings/stb) is used to support a wider range of texture formats

