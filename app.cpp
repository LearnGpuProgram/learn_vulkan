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

	uint32_t version{ 0 };
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

	vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
		vk::InstanceCreateFlags(),
		&appInfo,
		0,
		nullptr,//enable layers 
		static_cast<uint32_t>(extensions.size()),
		extensions.data() //enable extensions 
	);

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
		frameTime = float(1000.0 / frameTime);
	}
	++numFrames;
}

Application::~Application()
{
#ifdef DEBUG_MODE
	std::cout << "Destroy a graphics Application!\n";
#endif

	instance.destroy();

	glfwTerminate();
}