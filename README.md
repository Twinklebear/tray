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

Extras
-
The ray tracer also supports some extra scene options that can be specified within a <config> block in the scene file.

Moving Objects
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

