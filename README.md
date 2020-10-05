# Path-Tracer
This is a CPU-only, unidirectional path tracer.

![Render](render_5000spp.png)

## Features
- Unidirectional path tracer with importance sampling and russian roulette
- Next event estimation (direct light sampling)
- Multiple importance sampling for next event estimation
- Physically-based BRDF inspired by the Disney BRDF
- Area lights
- Thin lense camera model
- Multithreaded rendering with tiles
- Only spheres for now

## Stuff I want to add in the future
- Microfacet BSDF
- Triangle meshes
- kd-trees
- Scene file format
- Directional lights

## Some useful resources
- [Physically Based Rendering: From Theory To Implementation](http://www.pbr-book.org/)
- [Real Shading in Unreal Engine 4](https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf)
- [Moving Frostbite to Physically Based Rendering 3.0](https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf)
- All [SIGGRAPH PBR courses](https://blog.selfshadow.com/publications/)
- Real-time Rendering, 4th Edition

## Library licenses
- std_image_write.h : v1.15 - public domain image writer - [Github link](https://github.com/nothings/stb/blob/master/stb_image_write.h)
