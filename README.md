#  GPGPU possibilities using [WebGPU API](https://developer.mozilla.org/en-US/docs/Web/API/WebGPU_API)

## WebGPU in a nutshell
WebGPU is the hot new item of web APIs focused on unlocking a computer's hidden muscle, the GPU. Here's what it brings to the party:

- Performance Boost:️ Get ready for lightning-fast rendering and smooth animations! Even complex calculations will be a breeze for your GPU.
- Advanced Features: Unlock fancy graphics techniques like realistic lighting and mind-blowing effects ✨ that were impossible before.
- Developer Friendly: Writing code for WebGPU is more accessible and safer than the old ways—no more endless lines of code or worrying about security leaks.
- Future-Proof: WebGPU adapts to new hardware like a chameleon, so your code stays relevant even as technology evolves.

So, what does it all mean? WebGPU gives your web browser superpowers, making it faster, more capable, and ready to handle even the most demanding tasks. It's not just about fancy graphics; it's about opening doors to immersive experiences, complex calculations, and maybe even machine learning directly in the browser!
While still under development, the potential is massive. WebGPU is poised to shape the future of web graphics and computation, and you don't want to miss the ride!

## Dependencies
- [WebGPU API](https://github.com/webgpu-native/webgpu-headers)
- [GLFW](https://www.glfw.org/)
- [emscripten](https://emscripten.org/)

The project has the following dependencies:
- WebGPU API: This is a next-generation graphics and compute API.
- GLFW: A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input.
- 

## Shader language

The shaders are written in WebGPU Shading Language (WGSL).

## Building the project

First, install the required dependencies on your system. Then use emscripten to build the project.

```emcmake cmake -B build-web && cmake --build build-web```

## Running the project

After it's built, you can run the project with:
```./build-web/app``` or ```npx http-server``` for a web browser. 

---
## Application structure

### Shader Code
The application is based on a simple vertex and fragment shader defined in WGSL. The vertex shader function `vertexMain` takes an input `i`, the vertex index. It uses the index to select a position from a hard-coded array of 2D vertices that define a triangle.

The fragment shader `fragmentMain` doesn't take any input and returns a 4D vector with the color red.

### Render Pipeline

The `CreateRenderPipeline` function constructs a set of shaders for a given render pipeline and sets up its general properties using a pipeline descriptor.

### Main Render Sequence

The `Render` function sets up a simple render pass that clears the color attachment (i.e., the screen) and then draws a triangle using the pipeline.

The `Start` function initializes GLFW, a library for creating windows and contexts, and then opens a window for rendering. It then enters a loop where it repeatedly renders the scene and presents the results on the screen.

See the blog [WebGPU Cross Platform](https://developer.chrome.com/blog/webgpu-cross-platform?hl=en) for steps to recreate this repo.