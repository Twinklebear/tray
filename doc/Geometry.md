Geometry
===
Geometry in tray is placed within the scene tag in your scene file and each geometry
to be drawn in the scene is defined within an `<object>` tag within which its transformation and any children are specified. It's also possible to have an object without any geometry but to simply apply some transformation to its children. Objects themselves should have a unique name so that they don't conflict in the cache and objects using instances of the same geometry (eg. multiple instances of the same mesh) will re-use the mesh which will only be stored once.
```XML
<xml>
	<scene>
		<!-- Geometry -->
		<!-- Materials -->
	</scene>
    ...	
</xml>
```

Spheres
---
The built in sphere is defined as a sphere centered at the origin with unit radius specified by setting the geometry type to sphere. It can be scaled, rotated and translated by specifying these operations within its tag. Note that the order transformations are specified is the order they're applied, so for the below sphere the model matrix created will be `T x R x S`. Spheres can also be passed an additional parameter to specify their radius explicitly as `radius="2"` to use in the case of attaching a light to a sphere where a scaling matrix can no longer be used.
```XML
<object type="sphere" name="my_sphere" material="sphere_mat">
	<scale value="5"/>
    <rotate angle="-90" y="1"/>
	<translate x="-6" y="8" z="5"/>
</object>
```

Planes
---
The built in plane is defined as a plane centered at the origin with extent [-1, -1, 0] to [1, 1, 0] and a normal facing [0, 0, 1] specified by setting the geometry type to plane. The same transformations can be applied as any other geometry. Below is an example of using child geometry to create a red/blue Cornell box built out of the built in plane geometry.
```XML
<object name="box">
	<translate x="0" y="0" z="12"/>
	<object type="plane" name="WallBottom" material="wall">
		<scale value="32"/>
		<translate z="-12"/>
	</object>
	<object type="plane" name="WallTop" material="wall">
		<scale value="32"/>
		<rotate angle="180" x="1"/>
		<translate z="12"/>
	</object>
	<object type="plane" name="WallBack" material="wall">
		<scale value="32"/>
		<rotate angle="90" x="1"/>
		<translate y="20"/>
	</object>
	<object type="plane" name="WallLeft" material="wallRed">
		<scale value="32"/>
		<rotate angle="90" y="1"/>
		<translate x="-15"/>
	</object>
	<object type="plane" name="WallRight" material="wallBlue">
		<scale value="32"/>
		<rotate angle="-90" y="1"/>
		<translate x="15"/>
	</object>
</object>
```

Triangle Meshes
---
tray also supports arbitrary geometry defined in [Wavefront OBJ](https://en.wikipedia.org/wiki/Wavefront_OBJ) files with the requirement that vertex positions, normals and texture coordinates are provided by the file. Since the ASCII OBJ parser is extremely slow there's also a built in mesh pre-processor that can be run to produce an optimized binary OBJ version of the OBJ files so that they can be loaded significantly faster. See the main [README](../README.md) for information on how to use the pre-processor.

A mesh is specified the geometry type to obj and the name to the path to the file you want to load. The file path should be relative to the location of the scene file being loaded. Note that binary OBJ files will be automatically searched for with the same name as the obj file passed with the extension bobj. Below is an example of using a Utah teapot obj file in tray.
```XML
<object type="obj" name="./models/teapot.obj" material="copper">
	<scale value="0.6"/>
	<rotate angle="-50" z="1"/>
	<translate x="-3.5"/>
</object>
```

