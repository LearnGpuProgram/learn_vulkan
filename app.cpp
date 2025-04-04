#include "app.h"

#include <iostream>
#include <sstream>
#include <vector>


Application::Application()
{

#ifdef DEBUG_MODE
	std::cout << "Create a graphics Application\n";
#endif

	createWindow();
	createInstance();
	createValidation();
}

void Application::createWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	if (window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr))
	{
#ifdef DEBUG_MODE
		std::cout << "Successfully made a glfw window called \"VulkanWindow\", width: " << width << ", height: " << height << "\n";
#endif
	}
	else
	{
#ifdef DEBUG_MODE
		std::cout << "GLFW window creation failed\n";
#endif
	}
}

void Application::createInstance()
{
#ifdef DEBUG_MODE
	std::cout << "Makeing an instance... \n";
#endif

	uint32_t version{ VK_API_VERSION_1_0 };
	vkEnumerateInstanceVersion(&version);

#ifdef DEBUG_MODE
	std::cout << "System can support vulkan version: " << VK_API_VERSION_VARIANT(version)
		<< ", Major: " << VK_API_VERSION_MAJOR(version)
		<< ", Minor: " << VK_API_VERSION_MINOR(version)
		<< ", Patch: " << VK_API_VERSION_PATCH(version)
		<< '\n';
#endif 

    //为了兼容性和稳定性，我们可以降低版本，以适应更多硬件，降低版本有两种方法，这里都列出来
	version &= ~(0xFFFFU);

	version = VK_MAKE_API_VERSION(0, 1, 0, 0);

	vk::ApplicationInfo appInfo = vk::ApplicationInfo(
		title.c_str(),
		version
	);

    //Vulkan 的所有功能都是“可选启用”的，因此我们需要查询 GLFW 需要哪些扩展, 以便与 Vulkan 进行交互。
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions; 
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef DEBUG_MODE
	std::cout << "extensions to be requested:\n";
	for (const char* extensionName : extensions)
	{
		std::cout << "\t\"" << extensionName << "\"\n";
	}
#endif

#ifdef DEBUG_MODE
	const std::vector<const char*> validationLayers = {
		 "VK_LAYER_KHRONOS_validation"
	};

	extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif 

#ifdef DEBUG_MODE
	if (!checkValidationLayerSupport(validationLayers)) {
		throw std::runtime_error("Validation layers requested, but not available!");
	}
	else
	{
		std::cout << "Validation layers are available!\n";
	}
#endif

	// 填写 createInfo
	vk::InstanceCreateInfo createInfo{};
	createInfo.sType = vk::StructureType::eInstanceCreateInfo;
	createInfo.pApplicationInfo = &appInfo;
#ifdef DEBUG_MODE
	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	createInfo.ppEnabledLayerNames = validationLayers.data();
#else
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
#endif
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	try
	{
		instance = vk::createInstance(createInfo);
	}
	catch (vk::SystemError err)
	{
#ifdef DEBUG_MODE
		std::cout << "Failed to create Instance!\n";
		instance = nullptr;
#endif 
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

void Application::createValidation()
{
#ifdef DEBUG_MODE	
	dynamicloader = vk::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);

	vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
		vk::DebugUtilsMessengerCreateFlagsEXT(),
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
		&debugCallback,
		nullptr
	);

	debugMessenger = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dynamicloader);

	if(!debugMessenger)
	{
		std::cerr << "Failed to create Debug Utils Messenger!" << std::endl;
	}
#endif 
}

bool Application::checkValidationLayerSupport(const std::vector<const char*>& validationLayers) {
	uint32_t layerCount;
	vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<vk::LayerProperties> availableLayers(layerCount);
	vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;
		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}
		if (!layerFound) {
			std::cout << "Validation layer not found: " << layerName << "\n";
			return false;
		}
	}
	return true;
}


void Application::update()
{

}

void Application::render()
{

}

void Application::run()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		update();
		render();
		calculateFrameRate();
	}
}

void Application::calculateFrameRate()
{
	currentTime = glfwGetTime();
	double delta = currentTime - lastTime;

	if (delta >= 1)
	{
		int framerate{ std::max(1, int(numFrames / delta)) };
		std::stringstream sstitle;
		sstitle << title << " Running at " << framerate << " fps.";
		glfwSetWindowTitle(window, sstitle.str().c_str());
		lastTime = currentTime;
		numFrames = -1;
		frameTime = float(1000.0 / framerate);
	}
	++numFrames;
}

Application::~Application()
{
#ifdef DEBUG_MODE
	std::cout << "Destroy a graphics Application!\n";

	instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dynamicloader);
#endif

	instance.destroy();

	glfwTerminate();
}