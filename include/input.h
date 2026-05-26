#pragma once
#include <GLFW/glfw3.h>
#include <functional>
#include <vector>

struct KeyBind
{
    int key;
    std::function<void()> action;
    int previousState = 0; // per-bind state tracking
};

struct InputHandler
{
    std::vector<KeyBind> keybinds;
    std::vector<KeyBind> mouseBinds;

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
        for (auto& bind : keybinds)
        {
            int currentState = glfwGetKey(window, bind.key);
            if (currentState == GLFW_PRESS && bind.previousState != GLFW_PRESS)
            {
                bind.action();
            }
            bind.previousState = currentState;
        }
    }

    void processMouseInput(GLFWwindow* window)
    {
        for (auto& bind : mouseBinds)
        {
            int currentState = glfwGetMouseButton(window, bind.key);
            if (currentState == GLFW_PRESS && bind.previousState != GLFW_PRESS)
            {
                bind.action();
            }
            bind.previousState = currentState;
        }
    }
};