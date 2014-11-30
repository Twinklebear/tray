Filters
===
The filter to use when reconstructing the image from the samples taken can be optionally specified to use one of the filters below. The default is a box filter which covers only one pixel (equivalent to no filtering).

All filters require at least a width, `w`, and height, `h`, attribute which specify the extent of the filter on the image plane. In the coordinate system used for the filters each pixel is 0.5x0.5, so a 1.5x1.5 size filter will cover a 3x3 pixel region. If no width and height is specified they will take a default value of 0.5 (1 pixel) and the filter will have no effect along the axis where its extent wasn't specified.

The filter specifier tag should be placed within the config section of your scene file, as shown below for a simple box filter.
```XML
<xml>
	<scene>
		<!-- Objects -->
		<!-- Materials -->
	</scene>
    
	<camera>
		<!-- Camera settings -->
	</camera>

	<config>
		<!-- Config for sampling rates and renderer, filter tag goes here -->
        <filter type="box" w="1.5" h="1.5"/>
	</config>
</xml>
```

Box Filter
---
Select a box filter to be used to reconstruct the image.
```XML
<filter type="box" w="1.5" h="1.5"/>
```

Triangle Filter
---
Select a triangle filter to be used to reconstruct the image.
```XML
<filter type="triangle" w="1.5" h="1.5"/>
```

Gaussian Filter
---
Select a Gaussian filter to be used to reconstruct the image. The Gaussian filter also takes and extra `alpha` parameter which controls the falloff rate of the filter.
```XML
<filter type="gaussian" w="1.5" h="1.5" alpha="1"/>
```

Lanczos Windowed Sinc Filter
---
Select the [Lanczos filter](https://en.wikipedia.org/wiki/Lanczos_resampling) to be used to reconstruct the image. The Lanczos filter takes an additional parameter, `a`, which determines the size of the window the sinc filter is multiplied by.
```XML
<filter type="lanczos" w="1.5" h="1.5" a="2"/>
```

Mitchell-Netravali Filter
---
Select a Mitchell-Netravali filter to be used for image reconstruction. The Mitchell-Netravali filter takes two parameters to tune its effects, for more information on the paramters effects see [Reconstruction Filters in Computer Graphics](http://www.cs.utexas.edu/~fussell/courses/cs384g-fall2013/lectures/mitchell/Mitchell.pdf) these can be used to balance between blurring and sharpening of the image.
```XML
<filter type="mitchell" w="1.5" h="1.5" b="0.33" c="0.33"/>
```

Adding Your Own Filters
===
It is also possible to add your own filters to tray by writing a struct implementing the [Filter interface](include/filters/filter.h) and adding it to the filter loader in [load_filter.cpp](src/loaders/load_filter.cpp).

The only function that needs to be provided by your filter is one that evaluates its weight at some position, defined by the Filter interface below. You should also call the filter constructor with the width and height of the filter when your filter is constructed.
```c++
virtual float weight(float x, float y) const = 0;
```

