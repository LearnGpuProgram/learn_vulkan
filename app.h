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
	vk::DispatchLoaderDynamic dynamicloader;

	vk::PhysicalDevice physicalDevice{ nullptr };
	vk::Device logicalDevice{ nullptr };
	vk::Queue graphicsQueue{ nullptr };
	vk::Queue presentQueue{ nullptr };

	vk::SwapchainKHR swapchain{ nullptr };
	vk::Format swapchainFormat;
	vk::Extent2D swapchainExtent;
	vk::SurfaceKHR surface;
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats; 
	std::vector<vk::PresentModeKHR> presentModes;
	std::vector<vk::Image> swapchainImages{ nullptr };
	std::vector<vk::ImageView> swapchainFrames;
	std::vector<vk::Framebuffer> swapchainFramebuffers;
	std::vector<vk::CommandBuffer> swapchainCmdBuffers;

	vk::PipelineLayout pipelineLayout;
	vk::RenderPass renderpass;
	vk::Pipeline pipeline;

	vk::CommandPool cmdPool;
	vk::CommandBuffer mainCmdBuffer;

	vk::Fence inFlightFence;
	vk::Semaphore imageAvailable, renderFinished;

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
	void createFramebuffer();
	void createCommandPool();
	void createCommandBuffer();
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
	vk::Fence makeFence();
	vk::Semaphore makeSemaphore();
	void recordDrawCommands(vk::CommandBuffer commandBuffer, uint32_t imageIndex);
};