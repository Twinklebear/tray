CS6620 - Will Usher
=
The project should build easily with CMake but does require a C++11 compliant compiler.
I did choose to use TinyXML2 instead of TinyXML1, since the library is so small I've included its source under tinyxml2/
so there are no additional dependencies that need to be downloaded to build the renderer.

Select the build mode when compiling with CMake, `-DCMAKE_BUILD_TYPE=Debug` will build with debugging symbols
while `-DCMAKE_BUILD_TYPE=Release` will compile with full optimizations.

If you want to build the renderer with the live preview you'll need [SDL2](http://libsdl.org/) installed and
can then run CMake with the `-DBUILD_PREVIEWER=1` flag to compile the previewer. To view the live preview
when rendering run with the `-p` flag.

