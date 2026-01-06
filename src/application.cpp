#include <glad/glad.h>
#include <application.h>
#include <openglDebug.h>
#include <renderObject.h>
#include <demoShaderLoader.h>
#include <iostream>
#include <testscene.h>

int Application::init()
{
	if (!glfwInit())
		return -1;


#pragma region report opengl errors to std
	//enable opengl debugging output.
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#pragma endregion


	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //you might want to do this when testing the game for shipping

	input.addKeyBind({ GLFW_KEY_ESCAPE, [this]() { glfwSetWindowShouldClose(this->window, GLFW_TRUE); } });



	window = glfwCreateWindow(640, 480, "[_] PROJECT: 33", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


#pragma region report opengl errors to std
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebugOutput, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
#pragma endregion

	return 0;
}

void Application::run()
{

	TestingScene scene;

	scene.init();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		int width = 0, height = 0;

		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		input.processInput(window);
		scene.update();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	/*there is no need to call the clear function for the libraries since the os will do that for us.
	by calling this functions we are just wasting time.*/
	glfwDestroyWindow(window);
	glfwTerminate();
}
