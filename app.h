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
	virtual void update();
	virtual void render();

	virtual std::string getVertexFilepath();
	virtual std::string getFragmentFilepath();
private:
	int width{ 640 };
	int height{ 480 };
	float frameTime;
	int numFrames;
	std::string title{ "VulkanDemo" };

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
	std::vector<vk::ImageView> swapchainFrames;


	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderpass;
	vk::Pipeline pipeline;

	double lastTime;
	double currentTime;
private:
	void createWindow();
	void createInstance();
	void createValidation();
	void choosePhysicalDevice();
	void createLogicalDevice();
	void createSwapChain();
	void createPipeline();
private:
	void calculateFrameRate();
	bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
	void printDeviceProperties(const vk::PhysicalDevice& device);
	bool checkDeviceSuitable(const vk::PhysicalDevice& device);
	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device,
		const std::vector<const char*>& requestedExtensions);
	void findQueueFamilies(const vk::PhysicalDevice& device, QueueFamilyIndices& indices);
	vk::ShaderModule createModule(std::string filename);
	void makePipelineLayout();
	void makeRenderpass();
};