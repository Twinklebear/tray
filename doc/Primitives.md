Primitives
===
This documentation covers how to specify the various basic types that are used throughout the scene files to specify attributes such as colors, points, vectors and transformations.

Points
---
Points are specified by passing their coordinates as attributes. Any unspecified coordinates are assumed to be zero.

```XML
<!-- Z is unspecified here and will be set to 0 -->
<point_example x="1.2" y="2"/>
```

Vectors
---
Vectors are specified identically to points but can also take an additional scaling value to multiply the components with. As with points unspecified components are assumed to be 0.

```XML
<!-- The vector here will be set to {0.5, 2, 1.125} -->
<vector_example x="1" y="4" z="2.5" value="0.5"/>
```

Colors
---
Colors are specified identical to vectors but the attributes specified are the RGB  components. You can also specify a grayscale color by only providing the value attribute. Examples of both are given below.

```XML
<color_example r="1" g="0.2" b="0.9"/>
<!-- Specify a grayscale color by only setting a value, sets all components to the value -->
<color_example value="0.7"/>
```

Transformations
---
Transformations are specified by passing scale, rotation and translation parameters. The order in which the tags are read in is the order in which the transformations will be applied to the object. Both the scale and translate tags tags take the vector of scale/translation values for x, y and z. For scaling you can also specify a value attribute to have a uniform scaling applied to all axes, unspecified axes for scaling are assumed to be 1. Rotation takes an angle in degrees and a vector to rotate about, treated as described in the vector section. Some example transformations are given below.

```XML
<!-- Scale x and y by 2 and translate +5 z -->
<transform_example>
	<scale x="2" y="2"/>
    <translate z="5"/>
</transform_example>
<!-- Rotate about some arbitrary axis then translate, rotation axes will be normalized -->
<transform_example>
	<rotate angle="50" x="1" y="1"/>
    <translate x="5" y="-2"/>
</transform_example>
<!-- Do a bit of everything -->
<transform_example>
	<scale x="2" y="2"/>
	<rotate angle="50" x="1" y="1"/>
    <translate x="5" y="-2"/>
</transform_example>
```

