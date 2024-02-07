#include <GLFW/glfw3.h>
#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_glfw.h>
#include <iostream>

wgpu::Instance instance;
wgpu::SwapChain swapChain;

const uint32_t kWidth = 512;
const uint32_t kHeight = 512;

void Start() {
	if (!glfwInit()) {
		return;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window =
		glfwCreateWindow(kWidth, kHeight, "WebGPU window", nullptr, nullptr);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// TODO: render a triangle using webgpu
	}
}

int main() {
  instance = wgpu::CreateInstance();
  GetDevice([](wgpu::Device dev) {
    device = dev;
    Start();
  });
}

void GetDevice(void (*callback)(wgpu::Device)) {
  instance.RequestAdapter(
      nullptr,
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
