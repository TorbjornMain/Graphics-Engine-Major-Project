Graphics Engine Demo
Zachary De Maria

Features:

	Simple Animated Models:
Simple animated models with frustum culling, rendered using Physically Based Rendering techniques and using normal, specular, and diffuse maps, with a single directional light


	GPU Particle System:
Particles updated on GPU, 50000 in example project, has been tested with up to 20000000, runs without lag up to 2000000 particles on test systems.
Particles update velocities using vector flow fields stored in 3d textures

Particle parameters tweakable through options menu:
Vector field functions: 0 = no field, 1 = random acceleration field, 2 = cyclonic acceleration field, 3 = weak hyperbolic acceleration field, 4 = constant directional acceleration field
Velocity max = max spawn velocity at start, values over 10 will be clamped, reccomended values < 5
Velocity min = min spawn velocity at start, values under -10 will be clamped, reccomended values > -5
Start/End Colour = Colour over lifetime, linearly blends between the two over lifetime
Lifetime max/min = min and max lifetimes for the particles
Field scale = size of the vector field
Field strength = strength of the acceleration force of the vector field on the particles
Field offset = the offset from the particle system centre of the field
Gravity = flat acceleration on the particles, reccomended -1 < value < 1
Friction coefficient = multiplier on the particles velocity at each frame, reccomended value ~1


	Post processing:
Simple two pass rendering consisting of one off screen render buffer with simple post processing effects

Post process parameter tweakable through options menu:
Post Process shader: 0 = bloom, 1 = mild edge detection, 2 = simple box blur, 3 = depth 'fog'

	Volumetric rendering:
2(3) pass raycast volume rendering system for visualising 3d data, using 3d framebuffers and simple unlit rendering, features partial depth buffer support and frustum culling