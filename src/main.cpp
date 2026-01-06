#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <openglDebug.h>
#include <demoShaderLoader.h>
#include <iostream>
#include <renderObject.h>
#include <application.h>
#define USE_GPU_ENGINE 0
#define GOOD_INIT 0
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = USE_GPU_ENGINE;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = USE_GPU_ENGINE;
}
int main(void)
{


	Application app;

	if (app.init() == GOOD_INIT)
	{
		app.run();
	}

	return 0;
}
