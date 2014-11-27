Filters
===
TODO: rewrite doc

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

