# Basic Triangle with WebGPU

This is a simple cross-platform application that presents a basic usage of the WebGPU API to render a triangle on screen.

## Dependencies
- [WebGPU API](https://github.com/webgpu-native/webgpu-headers)
- [GLFW](https://www.glfw.org/)

The project has the following dependencies:
- WebGPU API: This is a next-generation graphics and compute API.
- GLFW: A multi-platform library for OpenGL, OpenGL ES, Vulkan, window and input.

## Shader language

The shaders are written in WebGPU Shading Language (WGSL).

## Building the project

First, install the required dependencies on your system. Then use your C++ compiler to build the project.

```emcmake cmake -B build-web && cmake --build build-web```

## Running the project

After it's built, you can run the project with:
```./build-web/app``` or ```npx http-server``` for a web browser. 


## Application structure

### Shader Code

The application is based on a simple vertex and fragment shader defined in WGSL. The vertex shader function `vertexMain` takes an input `i` which is the index of the vertex. It uses the index to select a position from a hard-coded array of 2D vertices which define a triangle.

The fragment shader `fragmentMain` doesn't take any input and simply returns a 4D vector with the color red.

### Render Pipeline

The `CreateRenderPipeline` function constructs a set of shaders for a given render pipeline and sets up its general properties using a pipeline descriptor.

### Main Render Sequence

The `Render` function sets up a simple render pass that clears the color attachment (i.e., the screen) and then draws a triangle using the pipeline.

The `Start` function initializes GLFW, a library for creating windows and contexts, and then opens a window for rendering. It then enters a loop where it repeatedly renders the scene and presents the results on the screen.

See the blog [WebGPU Cross Platform](https://developer.chrome.com/blog/webgpu-cross-platform?hl=en) for steps to recreate this repo.