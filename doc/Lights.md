Lights
===
Currently only two types of lights are supported, point lights and area lights. Point lights are infinitesimal light emitting points in space while area lights are attached to a piece of existing geometry, making it emissive. Point lights (and other lights without associated geometry when they're added) are specified at the end of the scene tag, as shown below. Area lights are attached to geometry by placing the light tag on the object, see the area light section.

```XML
<xml>
	<scene>
		<!-- Objects -->
		<!-- Materials -->
        
        <!-- Lights without associated geometry go here -->
	</scene>
    ...
</xml>
```

Point Lights
---
A point light is an infinitesimal light emitting point in space, where the position and intensity of the light can be specified.

```XML
<light type="point" name="my_point_light">
	<intensity r="0.780131" g="0.780409" b="0.775833" value="100"/>
	<position x="0" y="0" z="22"/>
</light>
```

Area Lights
---
An area light is attached to some geometry in the scene, making it emissive. Currently only spheres are fully supported, area lights can be attached to meshes but support is a bit buggy at the moment. There are also some subtleties in how the transformation and name for the object with the light attached should be specified. Area lights also require a material so they don't show up as objects missing a material in the renderer (eg. black).

One assumption in tray is that area light transformation matrices don't have scale applied to them. To get around this and still be able to scale objects the sphere radius can be set explicitly as shown below. In the case of meshes the transformation is applied to the mesh, bringing it into world space and making the model to world transform the identity. As a result of this the name specified for a sphere to have some area light attached should be made unique if a different radius than 1 will be used. Meshes will have the name of the light appended to their name in the cache to avoid conflicts with the model space version of themselves if they're also being used as a non-emissive model.

If this sounds a bit hacky that's because it's very hacky, hopefully I'll come up with something better and implement it :smile:.
```XML
<object type="sphere_r1.8" name="sphere_light" radius="1.8" material="diffuse_mat">
	<translate x="0" y="0" z="22"/>
	<light type="area" name="area_light">
		<intensity r="0.780131" g="0.780409" b="0.775833" value="50"/>
	</light>
</object>
```

Adding Your Own Lights
===
It's also possible to add your own light sources to tray by writing a class implementing the [Light interface](../include/lights/light.h) and adding code to load it in [load_light](../src/loaders/load_light.cpp). Detailed descriptions of the functions you need to implement to add a light source can be found in the light interface header file.

