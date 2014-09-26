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
The ray tracer supports JPEG baseline, PNG (8bit channel only), BMP (non-1bpp, non-RLE), and PPM image formats for textures.
Note that texture names beginning with \_\_ are reserved for generated texture names the ray tracer may need to create
and should not be used.

Extras
-
The ray tracer also supports some extra scene options that can be specified within a <config> block in the scene file.

Samplers
-
The type of sampler used to render the scene can be configured by the `<sampler type="">` tag. Available samplers and their
parameters are listed below by the type string that selects them.

- uniform - Selects a basic uniform sampler. A single ray is fired through the center of each pixel in the image. (Default)
- stratified - Selects a stratified sampler which renders the image using some desired number of jittered samples per pixel.
	- spp - number of samples to be taken per pixel in x & y, eg. sampler takes spp\*spp samples per pixel
- lowdiscrepancy - Selects a low-discrepancy which uses a (0, 2) sequence to generate sample positions
	- spp - number of samples to be taken per pixel

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

