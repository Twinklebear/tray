Volumes
===
TRay has some support for participating media, in the form of volume regions. These volumes can have homoegeneous density, exponential falloff in some direction or be loaded from a 3D grid volume data set. In addition volumes can be rendered by accounting for emission and absorption only or single scattering effects as well. Volumes are specified in the scene within volume nodes which behave like the object tag for geometry and take the transformation to be applied to position the volume in the scene. Volume nodes should be placed with the object tags in the scene file, an example volume node is shown below:

```XML
<volume_node name="floor_fog">
	<scale x="20" y="20" z="4.5"/>
	<volume name="floor_fog" type="exponential">
		<absorption value="0.04"/>
		<scattering value="0.08"/>
		<emission value="0.0005"/>
		<phase_asymmetry value="-0.67"/>
		<a value="4"/>
		<b value="1.2"/>
		<min x="-1" y="-1" z="0"/>
		<max x="1" y="1" z="1"/>
		<up x="0" y="0" z="1"/>
	</volume>
</volume_node>
```

All volumes require at least absorption and scattering coefficient parameters along with emission proprties and phase asymmetry to compute the Henyey-Greenstein phase function for the volume's scattering direction.

Homogeneous Volume
---
The homogeneous volume is a box region containing some volume with uniform properties throughout. The volume takes additional parameters to specify the min and max corners of the box containing the volume region.

```XML
<volume name="smoke_box" type="homogeneous">
	<absorption r="1" g="1" value="0.04"/>
	<scattering r="1" b="1"value="0.08"/>
	<emission r="1" g="0.4" value="0.0005"/>
	<phase_asymmetry value="-0.67"/>
	<min x="-1" y="-1" z="0"/>
	<max x="1" y="1" z="1"/>
</volume>
```

Exponential Falloff Volume
---
The exponential fallof volume is similar to the homogeneous volume but has some falloff applied to its density. This falloff is controlled by the a and b attributes in the falloff function: `a * std::exp(-b * h)` where h is the height of the point being evaluated along the specified up vector.

```XML
<volume_node name="floor_fog">
	<scale x="20" y="20" z="4.5"/>
	<volume name="floor_fog" type="exponential">
		<absorption value="0.04"/>
		<scattering value="0.08"/>
		<emission r="1" g="0.4" value="0.0005"/>
		<phase_asymmetry value="-0.67"/>
		<a value="4"/>
		<b value="1.2"/>
		<min x="-1" y="-1" z="0"/>
		<max x="1" y="1" z="1"/>
		<up x="0" y="0" z="1"/>
	</volume>
</volume_node>
```

Gridded Data Volume
---
The gridded data volume uses density information stored in some file to determine the density of points within the volume. The only volume file format supported is Mitsuba's vol format, see the grid-based volume data source section of the [Mitsuba doc](http://www.mitsuba-renderer.org/releases/current/documentation.pdf) for information on this format. The grid volume takes the file to the data set relative to the scene file and an optional extra scaling to be applied to the density values in the file.

```
<volume name="smoke_sim" type="vol" file="volumes/simulated_smoke.vol">
	<absorption value="0.06"/>
	<scattering value="0.008"/>
	<emission value="0.0005"/>
	<phase_asymmetry value="-0.47"/>
	<density_scale value="300"/>
</volume>
```

Volume Integrators
===
TRay provides two integrators for computing the effects of volumes in the scene, one that only accounts for emission and absorption and another that also accounts for single-scattering effects. Note that at the moment volume integrators are only supported in the path tracing and bidirectionl path tracing integrators. Both volume integrators take a step parameter which specifies how large of a step to take while ray marching through the volume.

Emission-Only Integrator
---
The emission only integrator just takes into account emission and absorption effects from volumes in the scene.

```XML
<vol_integrator type="emission" step="0.2"/>
```

Single Scattering Integrator
---
The single scattering integrator accounts for emission and absorption effects along with single scattering events off particles in the volume.

```XML
<vol_integrator type="single_scatter" step="0.1"/>
```

