#pragma once
#include <GLFW/glfw3.h>
#include <functional>
struct KeyBind
{
	int key;
	std::function<void()> action;
};
struct InputHandler
{
	std::vector<KeyBind> keybinds;

	void addKeyBind(KeyBind bind)
	{
		keybinds.push_back(bind);
	}

	void processInput(GLFWwindow* window)
	{
		for (const auto& bind : keybinds)
		{
			if (glfwGetKey(window, bind.key) == GLFW_PRESS)
			{
				bind.action();
			}
		}
	}

};