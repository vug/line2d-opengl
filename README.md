# 2D Light Simulation

Simulates light emission and reflection via raymarching on the GPU.

* An object consists of a Shape + Emission Coefficient + Reflection Coefficient.
* Shapes are defined via "Signed Distance Functions" which are suited for Raymarching.
* For each pixel computes 64 samples (from 64 jittered random directions)
  * Does raytracing via raymarching to compute potentials paths reaching to that pixel
  * If ray does not intersects with an emitting object sample has 0 contribution to pixel's brightness value
  * If intersects with an emitting object, sample's brightness += emission value
  * If intersects with a reflective object, changes ray direction towards the reflected direction
    * Path follows up to 3 reflections
  * Average of 64 samples becomes the pixel's value
  * (Currently working on adding a Refraction Coefficient)
* Reflections from a convex object produces caustics effects.

Using

* OpenGL 4.6
* GLSL 3.3
* Visual Studio 2017 C++
* Cherno's OpenGL C++ tutorial project layout
* C code for non-realtime CPU rendering found at [github.com/miloyip/light2d](https://github.com/miloyip/light2d)

Reflection example

videos: https://imgur.com/a/fgWCdOS

![a](https://i.imgur.com/8y5Ms1W.jpg)

Light + Shadow example

![a](https://i.imgur.com/yK8EHX8.jpg)

TO DO:

* Implement refraction by converting recursive tracing function into iteration via a fixed size array as the stack
  * Define a struct that'll be used a the scope of a recursive call
* Ability to choose scene from a file, or on-the-fly
* Ability to produce a scene via an editor
