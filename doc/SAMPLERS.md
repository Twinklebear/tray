Samplers
===
TODO: rewrite doc

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

