#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;


class VulkanApplication {
public:
    void run() {
        initWindow();
        mainLoop();
        cleanup();
    }
private:
    void initWindow();
    void mainLoop();
    void cleanup();
protected:
    GLFWwindow* window = nullptr;
public:
    bool framebufferResized = false;
};

static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<VulkanApplication*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void VulkanApplication::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void VulkanApplication::mainLoop() {
    while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();
    }
}

void VulkanApplication::cleanup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main()
{
    VulkanApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
	
}