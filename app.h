#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices;

class Application
{
public:
	Application();
	~Application();
public:
	void run();
protected:
	void update();
	void render();
private:
	int width{ 640 };
	int height{ 480 };
	float frameTime;
	int numFrames;
	GLFWwindow* window{ nullptr };
	vk::Instance instance{ nullptr };
	vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
	vk::PhysicalDevice physicalDevice{ nullptr };
	vk::Device logicalDevice{ nullptr };
	vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };
	vk::SwapchainKHR swapchain{ nullptr };
	std::vector<vk::Image> swapchainImages{ nullptr };
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;
	vk::SurfaceKHR surface;
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats; 
	std::vector<vk::PresentModeKHR> presentModes;
	vk::DispatchLoaderDynamic dynamicloader;
	std::string title{ "VulkanDemo" };
	double lastTime;
	double currentTime;
private:
	void createWindow();
	void createInstance();
	void createValidation();
	void choosePhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
private:
	void calculateFrameRate();
	bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
	void printDeviceProperties(const vk::PhysicalDevice& device);
	bool checkDeviceSuitable(const vk::PhysicalDevice& device);
	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions);
	void findQueueFamilies(const vk::PhysicalDevice& device, QueueFamilyIndices& indices);
};