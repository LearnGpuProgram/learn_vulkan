#include "app.h"

#include <iostream>
#include <sstream>


Application::Application()
{

#ifdef DEBUG_MODE
	std::cout << "Create a graphics Application\n";
#endif

	createWindow();
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

	glfwTerminate();
}