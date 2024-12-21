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
	std::string title{ "VulkanDemo" };
	double lastTime;
	double currentTime;
private:
	void createWindow();
	void calculateFrameRate();
};