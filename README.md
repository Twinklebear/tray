tray - A Toy Ray Tracer
===
tray is a toy physically based ray tracer built off of the techniques discussed in [Physically Based Rendering](http://pbrt.org/). It currently has support for path tracing, bidirectional path tracing, photon mapping and can describe materials with physically based microfacet models and measured data although support for more features is planned.

Building
---
A compliant C++14 compiler is required, or at least one that implements just the C++11/14 features I use (I don't have a list of required features though). GCC 4.9+ or Clang 3.4+ should definitely work, I haven't yet tested on the VS2015 CTP but I'll update this note when I do.

The project should build easily with CMake and if you aren't building the live previewer will download the minimal dependencies it requires. I also *strongly* recommend compiling in release mode to avoid the ray tracer being incredibly slow.

### Building the Live Previewer

The live previewer lets you optionally open a window and see the render update live as the image is computed. This is useful if you're trying to get some objects positioned correctly or just don't want to have to open a separate image viewer. To build the live previewer you'll need [SDL2](http://libsdl.org/) and when running CMake should pass `-DBUILD_PREVIEWER=1` to build it.

### Building With Trilinear Texture Filtering

tray defaults to using elliptically weighted averaging for high quality texture filtering results but if you want to trade the quality of the texture filtering for speed you can tell tray to use trilinear texture filtering instead. This is done by passing `-DTEX_TRILINEAR=1` when building the project. This was done as a compile flag to try and keep extraneous checks and work out of the texture filtering code as it's called a lot when rendering.

Dependencies
---
- [SDL2](http://libsdl.org/), only required if building the live previewer
- [TinyXML-2](https://github.com/leethomason/tinyxml2), downloaded by CMake
- [stb_image](https://github.com/nothings/stb), downloaded by CMake
- [glLoadGen](https://bitbucket.org/alfonse/glloadgen/wiki/Home), GL3.3 files included

Usage
---
tray accepts a few options to specify some parameters for the render, eg. the number of threads, scene file, output image file and so on. Information about the options can be printed at any time by running with `-h` and are listed in detail below.

- `-f <file>` Specify the scene file to render, should be an XML scene file, for specifics on the scene file format see `doc`.
- `-o <out_file>` Specify the output image file name, currently supports PPM and BMP image output.
- `-n <num>` Optional: specify the number of threads to use when rendering, the default is 1
- `-bw <num>` Optional: specify the desired width of blocks to partition the image into for the threads to work on, this size must evenly divide the image width. The default value is the image width.
- `-bh <num>` Optional: specify the desired height of blocks to partition the image into for the threads to work on, this size must evenly divide the image height. The default value is the image height.
- `-pmesh [<files>]` Specify a list of meshes to be run through the the obj -> binary obj  (bobj) processor so that they can be loaded faster when rendering. The renderer will check for bobj files with the same name when trying to load an obj file in a scene.
- `-p` Show a live preview of the image as it's rendered, this is only available if tray was built with the previewer. Rendering performance measurements won't be printed in this mode
- `-h` Print the help information

Scene File Format
---
Scene files are in XML and should have the following structure:
```XML
<xml>
	<scene>
		<!-- Geometry -->
		<!-- Materials -->
	</scene>
    
	<camera>
		<!-- Camera settings -->
	</camera>

	<config>
		<!-- Config for sampling rates and renderer -->
	</config>
</xml>
```
Some example scenes can be found under `scenes` and detailed documentation about the supported objects, materials and so on can be found under `doc` where files are titled by the section they cover, eg. `Objects.md` discusses the object types supported by tray and their usage.

