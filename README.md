# Path-Tracer
This is a CPU-only, unidirectional path tracer. Most of the shading code is unoptimized for the sake of clarity and debugging.

![Render](render_5000spp.png)

## Features
- Unidirectional path tracer with importance sampling and russian roulette
- Next event estimation (direct light sampling)
- Multiple importance sampling for next event estimation
- Physically-based BSDF inspired by the Disney BSDF
- Area lights
- Thin lense camera model
- Multithreaded rendering with tiles
- Only spheres for now
- Unit tests for most things incl. chi-square tests for BxDF sampling, and (weak) white furnace tests for BxDFs 

## Stuff I want to add in the future
- Triangle meshes
- Some acceleration structure
- Scene file format
- Directional lights
- Firefly removal

## Some useful resources I used
- [Physically Based Rendering: From Theory To Implementation, Matt Pharr, Wenzel Jakob, and Greg Humphreys](http://www.pbr-book.org/)
- [Background: Physics and Math of Shading (2012), Naty Hoffman](https://blog.selfshadow.com/publications/s2012-shading-course/hoffman/s2012_pbs_physics_math_notes.pdf)
- [Some Thoughts on the Fresnel Term (2020), Naty Hoffman](https://blog.selfshadow.com/publications/s2020-shading-course/hoffman/s2020_pbs_hoffman_slides.pdf)
- [Physically Based Shading at Disney (2012), Brent Burley](https://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf)
- [Extending the Disney BRDF to a BSDF with Integrated Subsurface Scattering (2015), Brent Burley](https://blog.selfshadow.com/publications/s2015-shading-course/burley/s2015_pbs_disney_bsdf_notes.pdf)
- [Pixar's Foundation for Materials (2017), Christophe Hery et al.](https://blog.selfshadow.com/publications/s2017-shading-course/pixar/s2017_pbs_pixar_notes.pdf)
- [Real Shading in Unreal Engine 4 (2013), Brian Karis](https://cdn2.unrealengine.com/Resources/files/2013SiggraphPresentationsNotes-26915738.pdf)
- [Moving Frostbite to Physically Based Rendering 3.0 (2015), Sebastien Lagarde and Charles de Rousiers](https://seblagarde.files.wordpress.com/2015/07/course_notes_moving_frostbite_to_pbr_v32.pdf)
- Real-time Rendering - 4th Edition, Tomas Akenine-Möller, Eric Haines and Naty Hoffman
- [Reflections and Refractions in Ray Tracing (2006), Bram de Greve](https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf)
- [Microfacet Models for Refraction through Rough Surfaces (2007), Bruce Walter et al.](https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf)
- [Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs (2014), Eric Heitz](http://jcgt.org/published/0003/02/03/)
- [Sampling the GGX Distribution of Visible Normals (2018), Eric Heitz](http://jcgt.org/published/0007/04/01/)

## Library licenses
- std_image_write.h : v1.15 - public domain image writer - [GitHub link](https://github.com/nothings/stb/blob/master/stb_image_write.h)
- catch.hpp : v2.13.3 - BSL-1.0 License - [GitHub link](https://github.com/catchorg/Catch2)
- json.hpp : v3.9.1 - MIT License - [GitHub link](https://github.com/nlohmann/json)
- tiny_obj_loader.h : v1.0.6 - MIT License - [GitHub link](https://github.com/tinyobjloader/tinyobjloader/)
