#include "app.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <optional>


Application::Application()
{

#ifdef DEBUG_MODE
	std::cout << "Create a graphics Application\n";
#endif

	createWindow();
	createInstance();
	createValidation();
	choosePhysicalDevice();
	createLogicalDevice();
	createSwapChain();
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

    //Ϊ�˼����Ժ��ȶ��ԣ����ǿ��Խ��Ͱ汾������Ӧ����Ӳ�������Ͱ汾�����ַ��������ﶼ�г���
	version &= ~(0xFFFFU);

	version = VK_MAKE_API_VERSION(0, 1, 0, 0);

	vk::ApplicationInfo appInfo = vk::ApplicationInfo(
		title.c_str(),
		version
	);

    //Vulkan �����й��ܶ��ǡ���ѡ���á��ģ����������Ҫ��ѯ GLFW ��Ҫ��Щ��չ, �Ա��� Vulkan ���н�����
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

	// ��д createInfo
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


void Application::printDeviceProperties(const vk::PhysicalDevice& device)
{
	vk::PhysicalDeviceProperties properties = device.getProperties();

	std::cout << "Device name: " << properties.deviceName << '\n';

	std::cout << "Device type: ";
	switch (properties.deviceType) {

	case (vk::PhysicalDeviceType::eCpu):
		std::cout << "CPU\n";
		break;

	case (vk::PhysicalDeviceType::eDiscreteGpu):
		std::cout << "Discrete GPU\n";
		break;

	case (vk::PhysicalDeviceType::eIntegratedGpu):
		std::cout << "Integrated GPU\n";
		break;

	case (vk::PhysicalDeviceType::eVirtualGpu):
		std::cout << "Virtual GPU\n";
		break;

	default:
		std::cout << "Other\n";
	}
}

bool Application::checkDeviceExtensionSupport(const vk::PhysicalDevice& device,
	const std::vector<const char*>& requestedExtensions)
{
	std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());

#ifdef DEBUG_MODE
	std::cout << "Device can support extensions:\n";
#endif // DEBUG_MODE

	for (vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties()) {

#ifdef DEBUG_MODE
			std::cout << "\t\"" << extension.extensionName << "\"\n";
#endif // DEBUG_MODE

		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool Application::checkDeviceSuitable(const vk::PhysicalDevice& device)
{
#ifdef DEBUG_MODE
		std::cout << "Checking if device is suitable\n";
#endif // DEBUG_MODE

	const std::vector<const char*> requestedExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

#ifdef DEBUG_MODE
		std::cout << "We are requesting device extensions:\n";

		for (const char* extension : requestedExtensions) {
			std::cout << "\t\"" << extension << "\"\n";
		}
#endif 

	if (bool extensionsSupported = checkDeviceExtensionSupport(device, requestedExtensions)) {

#ifdef DEBUG_MODE
			std::cout << "Device can support the requested extensions!\n";
#endif 
	}
	else 
	{
#ifdef DEBUG_MODE
			std::cout << "Device can't support the requested extensions!\n";
#endif 

		return false;
	}
	return true;
}

void Application::choosePhysicalDevice()
{
#ifdef DEBUG_MODE
	std::cout << "Choosing Physical Device \n";
#endif 

	std::vector<vk::PhysicalDevice> availableDevice = instance.enumeratePhysicalDevices();

#ifdef DEBUG_MODE
	std::cout << "There are " << availableDevice.size() << " physical devices available on this system\n";
#endif // DEBUG_MODE

	for (vk::PhysicalDevice device : availableDevice)
	{
#ifdef DEBUG_MODE
		printDeviceProperties(device);
#endif // DEBUG_MODE
		if (checkDeviceSuitable(device))
		{
#ifdef DEBUG_MODE
			std::cout << "Choosing Physical Device Successful \n";
#endif 
			physicalDevice = device;
			break;
		}
	}
}

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

void Application::findQueueFamilies(const vk::PhysicalDevice& device, QueueFamilyIndices& indices)
{
	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

#ifdef DEBUG_MODE
	std::cout << "There are " << queueFamilies.size() << " queue families available on the system.\n";
#endif

	int i = 0;
	for (const vk::QueueFamilyProperties& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
			indices.presentFamily = i;

#ifdef DEBUG_MODE
			std::cout << "Queue Family " << i << " is suitable for graphics and presenting\n";
#endif
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}
}

void Application::createLogicalDevice()
{
	QueueFamilyIndices indices;
	findQueueFamilies(physicalDevice, indices);

	float queuePriority = 1.0f;

	vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo(
		vk::DeviceQueueCreateFlags(), indices.graphicsFamily.value(),
		1, &queuePriority
	);

	vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();

	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	std::vector<const char*> enabledLayers;
#ifdef DEBUG_MODE
		enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
#endif
	vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
		vk::DeviceCreateFlags(),
		1, &queueCreateInfo,
		enabledLayers.size(), enabledLayers.data(),
		deviceExtensions.size(), deviceExtensions.data(),
		&deviceFeatures
	);

	try 
	{
		logicalDevice = physicalDevice.createDevice(deviceInfo);
		graphicsQueue = logicalDevice.getQueue(indices.graphicsFamily.value(), 0);
		presentQueue = logicalDevice.getQueue(indices.presentFamily.value(), 0),
#ifdef DEBUG_MODE
		std::cout << "GPU has been successfully abstracted!\n";
#endif
	}
	catch (vk::SystemError err) 
	{
#ifdef DEBUG_MODE
		std::cout << "Device creation failed!\n";
#endif // DEBUG_MODE
		logicalDevice = nullptr;
		graphicsQueue = nullptr;
	}
}

static std::vector<std::string> getTransformBits(vk::SurfaceTransformFlagsKHR bits)
{
	std::vector<std::string> result;
	if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity) {
		result.push_back("identity");
	}
	if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90) {
		result.push_back("90 degree rotation");
	}
	if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180) {
		result.push_back("180 degree rotation");
	}
	if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270) {
		result.push_back("270 degree rotation");
	}
	if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror) {
		result.push_back("horizontal mirror");
	}
	if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90) {
		result.push_back("horizontal mirror, then 90 degree rotation");
	}
	if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180) {
		result.push_back("horizontal mirror, then 180 degree rotation");
	}
	if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270) {
		result.push_back("horizontal mirror, then 270 degree rotation");
	}
	if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit) {
		result.push_back("inherited");
	}

	return result;
}

static std::vector<std::string> getAlphaCompositeBits(vk::CompositeAlphaFlagsKHR bits) 
{
	std::vector<std::string> result;

	if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque) 
	{
		result.push_back("opaque (alpha ignored)");
	}
	if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
	{
		result.push_back("pre multiplied (alpha expected to already be multiplied in image)");
	}
	if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
	{
		result.push_back("post multiplied (alpha will be applied during composition)");
	}
	if (bits & vk::CompositeAlphaFlagBitsKHR::eInherit)
	{
		result.push_back("inherited");
	}

	return result;
}

static std::vector<std::string> getImageUsageBits(vk::ImageUsageFlags bits) 
{
	std::vector<std::string> result;

	if (bits & vk::ImageUsageFlagBits::eTransferSrc)
	{
		result.push_back("transfer src: image can be used as the source of a transfer command.");
	}
	if (bits & vk::ImageUsageFlagBits::eTransferDst)
	{
		result.push_back("transfer dst: image can be used as the destination of a transfer command.");
	}
	if (bits & vk::ImageUsageFlagBits::eSampled)
	{
		result.push_back("sampled: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot either of type VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE or \
VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and be sampled by a shader.");
	}
	if (bits & vk::ImageUsageFlagBits::eStorage)
	{
		result.push_back("storage: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.");
	}
	if (bits & vk::ImageUsageFlagBits::eColorAttachment)
	{
		result.push_back("color attachment: image can be used to create a VkImageView suitable for use as \
a color or resolve attachment in a VkFramebuffer.");
	}
	if (bits & vk::ImageUsageFlagBits::eDepthStencilAttachment) 
	{
		result.push_back("depth/stencil attachment: image can be used to create a VkImageView \
suitable for use as a depth/stencil or depth/stencil resolve attachment in a VkFramebuffer.");
	}
	if (bits & vk::ImageUsageFlagBits::eTransientAttachment) 
	{
		result.push_back("transient attachment: implementations may support using memory allocations \
with the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT to back an image with this usage. This \
bit can be set for any image that can be used to create a VkImageView suitable for use as \
a color, resolve, depth/stencil, or input attachment.");
	}
	if (bits & vk::ImageUsageFlagBits::eInputAttachment)
	{
		result.push_back("input attachment: image can be used to create a VkImageView suitable for \
occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; be read from \
a shader as an input attachment; and be used as an input attachment in a framebuffer.");
	}
	if (bits & vk::ImageUsageFlagBits::eFragmentDensityMapEXT)
	{
		result.push_back("fragment density map: image can be used to create a VkImageView suitable \
for use as a fragment density map image.");
	}
	if (bits & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR)
	{
		result.push_back("fragment shading rate attachment: image can be used to create a VkImageView \
suitable for use as a fragment shading rate attachment or shading rate image");
	}
	return result;
}

static std::string getPresentMode(vk::PresentModeKHR presentMode) 
{
	if (presentMode == vk::PresentModeKHR::eImmediate) 
	{
		return "immediate: the presentation engine does not wait for a vertical blanking period \
to update the current image, meaning this mode may result in visible tearing. No internal \
queuing of presentation requests is needed, as the requests are applied immediately.";
	}
	if (presentMode == vk::PresentModeKHR::eMailbox)
	{
		return "mailbox: the presentation engine waits for the next vertical blanking period \
to update the current image. Tearing cannot be observed. An internal single-entry queue is \
used to hold pending presentation requests. If the queue is full when a new presentation \
request is received, the new request replaces the existing entry, and any images associated \
with the prior entry become available for re-use by the application. One request is removed \
from the queue and processed during each vertical blanking period in which the queue is non-empty.";
	}
	if (presentMode == vk::PresentModeKHR::eFifo)
	{
		return "fifo: the presentation engine waits for the next vertical blanking \
period to update the current image. Tearing cannot be observed. An internal queue is used to \
hold pending presentation requests. New requests are appended to the end of the queue, and one \
request is removed from the beginning of the queue and processed during each vertical blanking \
period in which the queue is non-empty. This is the only value of presentMode that is required \
to be supported.";
	}
	if (presentMode == vk::PresentModeKHR::eFifoRelaxed) 
	{
		return "relaxed fifo: the presentation engine generally waits for the next vertical \
blanking period to update the current image. If a vertical blanking period has already passed \
since the last update of the current image then the presentation engine does not wait for \
another vertical blanking period for the update, meaning this mode may result in visible tearing \
in this case. This mode is useful for reducing visual stutter with an application that will \
mostly present a new image before the next vertical blanking period, but may occasionally be \
late, and present a new image just after the next vertical blanking period. An internal queue \
is used to hold pending presentation requests. New requests are appended to the end of the queue, \
and one request is removed from the beginning of the queue and processed during or after each \
vertical blanking period in which the queue is non-empty.";
	}
	if (presentMode == vk::PresentModeKHR::eSharedDemandRefresh)
	{
		return "shared demand refresh: the presentation engine and application have \
concurrent access to a single image, which is referred to as a shared presentable image. \
The presentation engine is only required to update the current image after a new presentation \
request is received. Therefore the application must make a presentation request whenever an \
update is required. However, the presentation engine may update the current image at any point, \
meaning this mode may result in visible tearing.";
	}
	if (presentMode == vk::PresentModeKHR::eSharedContinuousRefresh)
	{
		return "shared continuous refresh: the presentation engine and application have \
concurrent access to a single image, which is referred to as a shared presentable image. The \
presentation engine periodically updates the current image on its regular refresh cycle. The \
application is only required to make one initial presentation request, after which the \
presentation engine must update the current image without any need for further presentation \
requests. The application can indicate the image contents have been updated by making a \
presentation request, but this does not guarantee the timing of when it will be updated. \
This mode may result in visible tearing if rendering to the image is not timed correctly.";
	}
	return "none/undefined";
}

static vk::SurfaceFormatKHR chooseSwapchainSurfaceFormat(std::vector<vk::SurfaceFormatKHR> formats) 
{
	for (vk::SurfaceFormatKHR format : formats)
	{
		if (format.format == vk::Format::eB8G8R8A8Unorm
			&& format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return format;
		}
	}
	return formats[0];
}

static vk::PresentModeKHR chooseSwapchainPresentMode(std::vector<vk::PresentModeKHR> presentModes) 
{
	for (vk::PresentModeKHR presentMode : presentModes) 
	{
		if (presentMode == vk::PresentModeKHR::eMailbox)
		{
			return presentMode;
		}
	}
	return vk::PresentModeKHR::eFifo;
}

static vk::Extent2D chooseSwapchainExtent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities) 
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		vk::Extent2D extent = { width, height };

		extent.width = std::min(
			capabilities.maxImageExtent.width,
			std::max(capabilities.minImageExtent.width, extent.width)
		);

		extent.height = std::min(
			capabilities.maxImageExtent.height,
			std::max(capabilities.minImageExtent.height, extent.height)
		);

		return extent;
	}
}

void Application::createSwapChain()
{
	VkSurfaceKHR csurface;
	if (glfwCreateWindowSurface(instance, window, nullptr, &csurface) != VK_SUCCESS)
	{
#ifdef DEBUG_MODE
		std::cout << "Failed to abstract glfw surface for Vulkan\n";
#endif // DEBUG_MODE

	}
	else
	{
#ifdef DEBUG_MODE
		std::cout << "Successfully abstracted glfw surface for Vulkan\n";
#endif // DEBUG_MODE
	}

	surface = csurface;

	capabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
#ifdef DEBUG_MODE
	std::cout << "Swapchain can support the following surface capabilities:\n";

	std::cout << "\tminimum image count: " << capabilities.minImageCount << '\n';
	std::cout << "\tmaximum image count: " << capabilities.maxImageCount << '\n';

	std::cout << "\tcurrent extent: \n";

	std::cout << "\t\twidth: " << capabilities.currentExtent.width << '\n';
	std::cout << "\t\theight: " << capabilities.currentExtent.height << '\n';

	std::cout << "\tminimum supported extent: \n";
	std::cout << "\t\twidth: " << capabilities.minImageExtent.width << '\n';
	std::cout << "\t\theight: " << capabilities.minImageExtent.height << '\n';

	std::cout << "\tmaximum supported extent: \n";
	std::cout << "\t\twidth: " << capabilities.maxImageExtent.width << '\n';
	std::cout << "\t\theight: " << capabilities.maxImageExtent.height << '\n';

	std::cout << "\tmaximum image array layers: " << capabilities.maxImageArrayLayers << '\n';

	std::cout << "\tsupported transforms:\n";
	std::vector<std::string> stringList = getTransformBits(capabilities.supportedTransforms);
	for (std::string line : stringList) {
		std::cout << "\t\t" << line << '\n';
	}

	std::cout << "\tcurrent transform:\n";
	stringList = getTransformBits(capabilities.currentTransform);
	for (std::string line : stringList) {
		std::cout << "\t\t" << line << '\n';
	}

	std::cout << "\tsupported alpha operations:\n";
	stringList = getAlphaCompositeBits(capabilities.supportedCompositeAlpha);
	for (std::string line : stringList) {
		std::cout << "\t\t" << line << '\n';
	}

	std::cout << "\tsupported image usage:\n";
	stringList = getImageUsageBits(capabilities.supportedUsageFlags);
	for (std::string line : stringList) {
		std::cout << "\t\t" << line << '\n';
	}
#endif // DEBUG_MODE

	formats = physicalDevice.getSurfaceFormatsKHR(surface);

#ifdef DEBUG_MODE
	for (vk::SurfaceFormatKHR supportedFormat : formats) {
		std::cout << "supported pixel format: " << vk::to_string(supportedFormat.format) << '\n';
		std::cout << "supported color space: " << vk::to_string(supportedFormat.colorSpace) << '\n';
	}
#endif 

	presentModes = physicalDevice.getSurfacePresentModesKHR(surface);
#ifdef DEBUG_MODE
	for (vk::PresentModeKHR presentMode : presentModes) {
		std::cout << '\t' << getPresentMode(presentMode) << '\n';
	}
#endif 

	vk::SurfaceFormatKHR format = chooseSwapchainSurfaceFormat(formats);

	vk::PresentModeKHR presentMode = chooseSwapchainPresentMode(presentModes);

	vk::Extent2D extent = chooseSwapchainExtent(width, height, capabilities);

	uint32_t imageCount = std::min(
		capabilities.maxImageCount,
		capabilities.minImageCount + 1
	);

	vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR(
		vk::SwapchainCreateFlagsKHR(), surface, imageCount, format.format, format.colorSpace,
		extent, 1, vk::ImageUsageFlagBits::eColorAttachment
	);

	QueueFamilyIndices indices;
	findQueueFamilies(physicalDevice, indices);

	if (physicalDevice.getSurfaceSupportKHR(indices.presentFamily.value(), surface)) 
	{
#ifdef DEBUG_MODE
		std::cout << "Queue Family " << indices.presentFamily.value() << " is suitable for presenting\n";
#endif
	}

	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) 
	{
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

	try
	{
		swapchain = logicalDevice.createSwapchainKHR(createInfo);
	}
	catch (vk::SystemError err)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}
	swapchainImages = logicalDevice.getSwapchainImagesKHR(swapchain);
	swapchainFormat = format.format;
	swapchainExtent = extent;
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
#endif 

	logicalDevice.destroySwapchainKHR(swapchain);
	logicalDevice.destroy();

	instance.destroySurfaceKHR(surface);
#ifdef DEBUG_MODE
	instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dynamicloader);
#endif

	instance.destroy();

	glfwTerminate();
}