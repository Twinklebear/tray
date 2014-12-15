Textures
===
The ray tracer supports PNG (8bit channel only), BMP (non-1bpp, non-RLE), and PPM image formats for textures. Note that texture names beginning with \_\_ are reserved for generated texture names the ray tracer may need to create and should not be used. Textures can also have transformations specified which will result in a transformation being applied to the texture coordinates when looking up the texel.

Checkerboard Texture
---
The checkerboard texture is a regular two color checkerboard and takes two colors to be selected for the colors of the checks. Below is an example of a checkerboard with some non-uniform scaling applied.

```XML
<texture_example texture="checkerboard">
	<color1 value="1"/>
	<color2 value="0"/>
	<scale x="0.2" y="0.2"/>
</texture_example>
```

Image Texture
---
The image texture loads an image and applies it to the object using its texture coordinates. TRay supports PNG (8bit channel only), BMP (non-1bpp, non-RLE), and PPM image formats for textures. The name used for an image texture should be the path to the file to load, relative to the scene file.

```XML
<texture_example texture="textures/bricks.ppm">
	<rotate angle="45" z="1"/>
</texture_example>
```

UV Texture
---
The UV texture just returns the texture coordinates passed in as the "color" of the texture, useful for debugging.

```XML
<texture_example texture="uv"/>
```

