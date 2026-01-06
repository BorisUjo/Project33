#pragma once
#include <GLFW/glfw3.h>
#include <input.h>
class Application
{
public:
	InputHandler input;
	GLFWwindow* window;

	int init();
	void run();
};