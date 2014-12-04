Materials
===
The renderer uses a physically based material model supporting a range of materials from microfacet models to different types of measured data. The materials selected specify a set of [BRDFs](https://en.wikipedia.org/wiki/Bidirectional_reflectance_distribution_function) to be returned that describe the material properties at some point on the surface.

Materials in tray are placed in a block at the bottom of the scene tag (or at the top if you like) and must have unique names to avoid conflicts in the cache. To assign a material to some geometry you specify the name of the material to be used as the material attribute. See the [Geometry](Geometry.md) documentation for more information.
```XML
<xml>
	<scene>
		<!-- Geometry -->
		<!-- Materials -->
	</scene>
    ...	
</xml>
```

Matte Material
---
The matte material can be used to select between two types of BRDFs that describe diffuse surfaces, based on the roughness attribute (or lack thereof). A [Lambertian](https://en.wikipedia.org/wiki/Lambertian_reflectance) BRDF is selected by setting the roughness to 0, or omitting the tag entirely. The [Oren-Nayar](http://en.wikipedia.org/wiki/Oren%E2%80%93Nayar_reflectance_model) microfacet model for rough diffuse surfaces can be selected by specifying a roughness value in (0, 1]. Samples of both Lambertian and Oren-Nayar matte materials are shown below.

```XML
<material type="matte" name="lambertian_model">
	<!-- The color (or texture) for the material, required -->
    <diffuse r="0.2" g="1" b="1"/>
    <!-- roughness defaults to 0, selecting Lambertian -->
</material>
```
```XML
<material type="matte" name="oren_nayar_model">
	<diffuse r="0.2" g="1" b="1"/>
	<roughness value="0.3"/>
</material>
```

Plastic Material
---
The plastic material is constructed by combining a Lambertian BRDF and a Torrance-Sparrow microfacet model and a dielectric Fresnel component to form the material BSDF. If the roughness of the plastic is uniform a Blinn microfacet distribution is used, if the roughness is specified to be anisotropic an [Ashikhmin-Shirley](http://www.cs.utah.edu/~shirley/papers/jgtbrdf.pdf) anisotropic microfacet distribution is used.

Examples of both uniform and anisotropic models are shown below.
```XML
<material type="plastic" name="uniform_plastic_mat">
	<!-- Lambertian color (or texture) for the material, required -->
    <diffuse  r="0.2" g="0.2" b="0.8" value="0.9"/>
    <!-- Torrance-Sparrow color (or texture) for the material, required -->
    <specular r="0.6" g="0.6" b="1.0" value="0.7"/>
    <!-- Blinn microfacet distribution roughness between (0, 1], defaults to 1 (roughest) -->
    <roughness value="0.05"/>
</material>
```
```XML
<material type="plastic" name="aniso_plastic_mat">
    <diffuse  r="0.2" g="0.2" b="0.8" value="0.9"/>
    <specular r="0.6" g="0.6" b="1.0" value="0.7"/>
	<!-- The material's anisotropic roughness x/y between (0, 1], defaults to 1 (roughest) -->
    <roughness x="0.05" y="0.005"/>
</material>
```

Translucent Material
---
The translucent BSDF is constructed by combining reflective and transmissive versions of the plastic material BSDF, wich the colors of the reflective and transmissive components multiplied by the reflective and transmissive colors set for the material. The index of refraction is used for the dielectric Fresnel component of the Torrance-Sparrow BRDFs in the material.

Note that there is currently a bug in the translucent material where light doesn't refract when passing through it, regardless of the index of refraction set for the material.
```XML
<material type="translucent" name="my_translucent_material">
	<!-- Lambertian color (or texture) for the material, required -->
	<diffuse  r="0.8" g="0.2" b="0.2"/>
    <!-- Torrance-Sparrow color (or texture) for the material, required -->
	<specular r="1.0" g="1.0" b="1.0" value="0.9"/>
	<!-- The color (or texture) to multiply in for reflections, required -->
	<reflection r="1.0" g="0.4" b="1.0" value="0.2"/>
	<!-- The color (or texture) to multiply in for transmission, required -->
	<transmission r="1.0" g="1.0" b="0.2" value="0.8"/>
    <!-- Blinn microfacet distribution roughness between (0, 1], defaults to 1 (roughest) -->
	<roughness value="0.5"/>
	<!-- The material's index of refraction, defaults to 1 (air) -->
	<ior value="1.6"/>
</material>
```

Metal Material
---
The metal material describes the metal BSDF using a Torrance-Sparrow BRDF with a conductor Fresnel component. The metal's index of refraction and absorption coefficient are used for the conductor Fresnel component and the roughness, both uniform and anisotropic can be specified. If the roughness of the metal is uniform a Blinn microfacet distribution is used, if the roughness is specified to be anisotropic an [Ashikhmin-Shirley](http://www.cs.utah.edu/~shirley/papers/jgtbrdf.pdf) anisotropic microfacet distribution is used.

The metal's index of refraction and absorption coefficients can also be loaded from measured metal data from PBRT's [spd](https://github.com/mmp/pbrt-v2/tree/master/scenes/spds/metals) files, which have measurements for a wide range of metals.

Examples for uniform, anisotropic and usage of PBRT's spd files are shown below.
```XML
<material type="metal" name="metal_uniform_rough">
	<!-- The metal's index of refraction (can be a texture), required -->
	<ior r="1.3" g="1.1" b="0.4"/>
	<!-- The metal's absoprtion coefficient (can be a texture), required -->
	<absorption r="1.9" g="2.5" b="4.3"/>
	<!-- The material's roughness between (0, 1], defaults to 1 (roughest) -->
	<roughness value="0.01"/>
</material>
```
```XML
<material type="metal" name="metal_aniso_rough">
	<ior r="1.3" g="1.1" b="0.4"/>
	<absorption r="1.9" g="2.5" b="4.3"/>
	<!-- The material's anisotropic roughness x/y between (0, 1], defaults to 1 (roughest) -->
	<roughness x="0.01" y="0.001"/>
</material>
```
```XML
<material type="metal" name="pbrt_spd_metal">
	<!-- PBRT SPD files are specified by an spd attribute with the spd file path -->
	<ior spd="./spds/Cu.eta.spd"/>
	<absorption spd="./spds/Cu.k.spd"/>
    <!-- Can also specify an anisotropic roughness -->
	<roughness value="0.000976"/>
</material>
```

MERL Material
---
The MERL material uses measured BRDFs from the [MERL BRDF database](http://www.merl.com/brdf/) introduced in "A Data-Driven Reflectance Model", by Wojciech Matusik, Hanspeter Pfister, Matt Brand and Leonard McMillan which appeared in ACM Transactions on Graphics 22, 3(2003), 759-769. MERL materials are specified by just passing the file path to the measured BRDF data.
```XML
<material type="merl" name="my_merl_material" file="./brdfs/alumina-oxide.binary"/>
```

Glass Material
---
The glass material defines glass as a combination of specular reflection and transmission components to define a specular glass BSDF.
```XML
<material type="glass" name="my_glass_mat">
	<reflection r="1" g="0.5" b="0.8"/>
	<transmission r="0.4" g="0.2" b="1"/>
	<!-- Index of refraction of the glass, defaults to 1 (air) -->
	<ior value="1.52"/>
</material>
```

Mix Material
---
The mix material can be used to construct a new material as the mixture of two previously declared materials. The resulting BSDF for the surface is the scaled combination of the BSDFs created by the materials being mixed, computed as `(1 - scale) * mat_a + scale * mat_b`, you can think of it as a LERP between the two materials.
```XML
<material type="mix" name="my_mix_mat">
	<material name="mix_mat_a" />
	<material name="mix_mat_b" />
	<!-- Mix weight for the materials (can also be a texture) -->
	<scale r="0.2" g="0.8" b="0.5"/>
</material>
```

