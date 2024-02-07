#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#include <iostream>
#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#else
#include <webgpu/webgpu_glfw.h>
#endif

wgpu::Instance instance;
wgpu::Device device;
wgpu::RenderPipeline pipeline;

wgpu::SwapChain swapChain;
const uint32_t kWidth = 512;
const uint32_t kHeight = 512;

/**
 * @brief Sets up the swap chain for rendering to a surface.
 *
 * This function creates a swap chain with the specified dimensions, format, and present mode.
 * The swap chain will be used as a render attachment.
 *
 * @param surface The surface to associate the swap chain with.
 */
void SetupSwapChain(wgpu::Surface surface) {
    wgpu::SwapChainDescriptor scDesc{
            .usage = wgpu::TextureUsage::RenderAttachment,
            .format = wgpu::TextureFormat::BGRA8Unorm,
            .width = kWidth,
            .height = kHeight,
            .presentMode = wgpu::PresentMode::Fifo};
    swapChain = device.CreateSwapChain(surface, &scDesc);
}

/**
 * @brief Retrieves a wgpu::Device object using a callback function.
 *
 * This function requests an adapter from `instance` and retrieves a device from the adapter
 * using another callback function. If the request fails, the function exits.
 *
 * @param callback A function pointer to a callback function that takes a wgpu::Device object as an argument.
 * @note It is the responsibility of the caller to ensure that the callback function properly handles the acquired device.
 */
void GetDevice(void (*callback)(wgpu::Device)) {
    instance.RequestAdapter(
            nullptr,
            // TODO(https://bugs.chromium.org/p/dawn/issues/detail?id=1892): Use
            // wgpu::RequestAdapterStatus, wgpu::Adapter, and wgpu::Device.
            [](WGPURequestAdapterStatus status, WGPUAdapter cAdapter,
               const char* message, void* userdata) {
                if (status != WGPURequestAdapterStatus_Success) {
                    exit(0);
                }
                wgpu::Adapter adapter = wgpu::Adapter::Acquire(cAdapter);
                adapter.RequestDevice(
                        nullptr,
                        [](WGPURequestDeviceStatus status, WGPUDevice cDevice,
                           const char* message, void* userdata) {
                            wgpu::Device device = wgpu::Device::Acquire(cDevice);
                            device.SetUncapturedErrorCallback(
                                    [](WGPUErrorType type, const char* message, void* userdata) {
                                        std::cout << "Error: " << type << " - message: " << message;
                                    },
                                    nullptr);
                            reinterpret_cast<void (*)(wgpu::Device)>(userdata)(device);
                        },
                        userdata);
            },
            reinterpret_cast<void*>(callback));
}

// shaderCode[] is a string containing a vertex shader and fragment shader defined in WGSL (WebGPU Shading Language)
// this is a simple way of creating a triangle on the screen
const char shaderCode[] = R"(
    @vertex fn vertexMain(@builtin(vertex_index) i : u32) ->
      @builtin(position) vec4f {
        const pos = array(vec2f(0, 1), vec2f(-1, -1), vec2f(1, -1));
        return vec4f(pos[i], 0, 1);
    }
    @fragment fn fragmentMain() -> @location(0) vec4f {
        return vec4f(1, 0, 0, 1);
    }
)";

/**
 * @brief Create a render pipeline using the given shader code.
 *
 * This function creates a render pipeline by compiling the provided shader code into a shader module and constructing
 * a render pipeline descriptor with the specified vertex and fragment states. The resulting pipeline is stored in the
 * 'pipeline' variable.
 */
void CreateRenderPipeline() {
    wgpu::ShaderModuleWGSLDescriptor wgslDesc{};
    wgslDesc.code = shaderCode;

    wgpu::ShaderModuleDescriptor shaderModuleDescriptor{
            .nextInChain = &wgslDesc};
    wgpu::ShaderModule shaderModule =
            device.CreateShaderModule(&shaderModuleDescriptor);

    wgpu::ColorTargetState colorTargetState{
            .format = wgpu::TextureFormat::BGRA8Unorm};

    wgpu::FragmentState fragmentState{.module = shaderModule,
            .targetCount = 1,
            .targets = &colorTargetState};

    wgpu::RenderPipelineDescriptor descriptor{
            .vertex = {.module = shaderModule},
            .fragment = &fragmentState};
    pipeline = device.CreateRenderPipeline(&descriptor);
}

/**
 * @brief Renders a frame using WebGPU.
 *
 * This function sets up a render pass to render a frame using WebGPU. It clears the current texture view,
 * sets the render pass descriptor with the color attachment, creates a command encoder, begins a render pass,
 * sets the pipeline, performs a draw operation, ends the render pass, finishes the command encoder,
 * and submits the command buffer to the device queue.
 */
void Render() {
    wgpu::RenderPassColorAttachment attachment{
            .view = swapChain.GetCurrentTextureView(),
            .loadOp = wgpu::LoadOp::Clear,
            .storeOp = wgpu::StoreOp::Store};

    wgpu::RenderPassDescriptor renderpass{.colorAttachmentCount = 1,
            .colorAttachments = &attachment};

    wgpu::CommandEncoder encoder = device.CreateCommandEncoder();
    wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderpass);
    pass.SetPipeline(pipeline);
    pass.Draw(3);
    pass.End();
    wgpu::CommandBuffer commands = encoder.Finish();
    device.GetQueue().Submit(1, &commands);
}

/**
 * @brief Initializes the graphics system for rendering to a surface.
 *
 * This function sets up the swap chain and creates a render pipeline for rendering to a specified surface.
 * The swap chain is created with the specified dimensions, format, and present mode and will be used as a render attachment.
 * The render pipeline is created by compiling the provided shader code into a shader module and constructing a render pipeline descriptor
 * with the specified vertex and fragment states.
 *
 * @param surface The surface to associate the swap chain with.
 */
void InitGraphics(wgpu::Surface surface) {
    SetupSwapChain(surface);
    CreateRenderPipeline();
}

/**
 * @brief Starts the WebGPU application.
 *
 * This function initializes GLFW, creates a window with the specified dimensions and title,
 * creates a WebGPU surface, initializes graphics with the created surface,
 * and runs the rendering loop.
 *
 * @return void
 *
 * @see glfwInit, glfwWindowHint, glfwCreateWindow, CreateSurfaceForWindow, InitGraphics, Render,
 * glfwWindowShouldClose, glfwPollEvents, swapChain, instance
 */
void Start() {
    if (!glfwInit()) {
        return;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window =
            glfwCreateWindow(kWidth, kHeight, "WebGPU window", nullptr, nullptr);

#if defined(__EMSCRIPTEN__)
    wgpu::SurfaceDescriptorFromCanvasHTMLSelector canvasDesc{};
  canvasDesc.selector = "#canvas";

  wgpu::SurfaceDescriptor surfaceDesc{.nextInChain = &canvasDesc};
  wgpu::Surface surface = instance.CreateSurface(&surfaceDesc);
#else
    wgpu::Surface surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
#endif

    InitGraphics(surface);

#if defined(__EMSCRIPTEN__)
    emscripten_set_main_loop(Render, 0, false);
#else
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Render();
        swapChain.Present();
        instance.ProcessEvents();
    }
#endif
}

int main() {
    instance = wgpu::CreateInstance();
    GetDevice([](wgpu::Device dev) {
        device = dev;
        Start();
    });
}