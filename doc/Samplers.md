Samplers
===
This doc covers the various samplers that can be selected. Samplers are used when finding sample positions to send rays from on the image plane and can have a big effect on image quality if they're not well distributed.

Stratified Sampler
---
This sampler partitions the pixel into a grid and takes jittered samples in each grid cell. It takes a single attribute to specify the number of samples per axis, spp, and will take spp \* spp samples after splitting the grid.

```XML
<!-- This stratified sampler will take 36 samples per pixel -->
<sampler type="stratified" spp="6"/>
```

Low Discrepancy Sampler
---
The low discrepency sampler chooses samples using sequences known to give well distributed samples, combining samples from a Van der Corput and Sobol sequence. The sampler also takes a parameter to select the number of samples to take per pixel which must also be a power of two. If a non-power of two number is passed it will be rounded up and a warning printed.

```XML
<sampler type="lowdiscrepancy" spp="32"/>
```

Adaptive Sampler
---
The adaptive sampler determines the number of samples to take based on the variance in the contrast of samples taken previously. To choose sample positions it uses the low discrepancy sampler described previously and as such also requires power of two sample counts. The sampler takes two parameters to specify the min and max samples to take per pixel and will begin by taking min samples and use their results to determine if more samples are needed. If it's determined more are required it will step up the sample count in powers of two until reaching the max.

```XML
<sampler type="adaptive" min="32" max="128"/>
```

