#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <vulkan/vulkan.hpp>

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
	vk::DispatchLoaderDynamic dynamicloader;
	std::string title{ "VulkanDemo" };
	double lastTime;
	double currentTime;
private:
	void createWindow();
	void createInstance();
	void createValidation();
private:
	void calculateFrameRate();
	bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
};