Materials
===
TODO: Clean up doc and complete

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

