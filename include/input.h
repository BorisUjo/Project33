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
	std::vector<KeyBind> mouseBinds;

	int previousKeyState = 0;
	int previousMouseButtonState = 0;

	void addKeyBind(KeyBind bind)
	{
		keybinds.push_back(bind);
	}

	void addMouseBind(KeyBind bind)
	{
		mouseBinds.push_back(bind);
	}

	void processInput(GLFWwindow* window)
	{

		int currentKeyState = 0;

		for (auto& bind : keybinds)
		{
			currentKeyState = glfwGetKey(window, bind.key);
			if (currentKeyState == GLFW_PRESS && previousKeyState != GLFW_PRESS)
			{
				bind.action();
				break;
			}

		}

		previousKeyState = currentKeyState;

	}

	void processMouseInput(GLFWwindow* window)
	{
		int currentKeyState = 0;
		for (auto& bind : mouseBinds)
		{
			currentKeyState = glfwGetMouseButton(window, bind.key);
			if (currentKeyState == GLFW_PRESS && previousMouseButtonState != GLFW_PRESS)
			{
				bind.action();
				break;
			}
		}
		previousMouseButtonState = currentKeyState;
	}

};