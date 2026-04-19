#include <glad/glad.h>
#include <application.h>
#include <openglDebug.h>
#include <renderObject.h>
#include <demoShaderLoader.h>
#include <iostream>
#include <testscene.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <mainScene.h>

void init_ui(GLFWwindow* window, ImGuiIO& output);

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

	//TestingScene testScene;

	PickingTexture picker;

	ImGuiIO io;

	MainScene scene;

	scene.configure_inputs(input);

	scene.init();
	scene.set_window(window);

	init_ui(window, io);

	int w = 0, h= 0;
	glfwGetFramebufferSize(window, &w, &h);
	picker.initialise(w,h);

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);

		glViewport(0, 0, width, height);
		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		input.processInput(window);
		input.processMouseInput(window);	

		// for now picking without raycast, just read the pixel under the mouse cursor from the picking texture.
		// implement raycasting later
		// to reduce lag: render only if a unit is selected, or if the mouse moves,
		// until a unit is selected, picking should only catch units and not every gameobject in the scene
		// and when a unit is selected, only pick the tiles inside the Unit's move range
		// maybe have a specific vector of objects to pick through in GameManager
		picker.enable_writing();

		glViewport(0, 0, width, height);

		scene.picking_render();

		double x, y;
		glfwGetCursorPos(window, &x, &y);
		scene.sceneCamera.currentPixel = picker.read_pixel(x, height - y - 1);

		picker.disable_writing();


		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		scene.render();

		scene.update();

		scene.render_ui();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	/*there is no need to call the clear function for the libraries since the os will do that for us.
	by calling this functions we are just wasting time.*/
	glfwDestroyWindow(window);
	glfwTerminate();
}


void init_ui(GLFWwindow* window, ImGuiIO& output)
{
	const char* glsl_version = "#version 130";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	output = ImGui::GetIO(); (void)output;

	output.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	output.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	output.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (output.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

}