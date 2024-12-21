
#include "app.h"

int main()
{
	Application* vkApp = new Application();

	vkApp->run();

	delete vkApp;

	return 0;
}