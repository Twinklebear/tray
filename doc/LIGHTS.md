Lights
===
TODO: Better doc, discuss location of point lights in scene file? Perhaps add spotlight or something?
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

