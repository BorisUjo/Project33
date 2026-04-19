#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <pickingTexture.h>

// cijela ova klasa je za kurac, treba spalit i bacit i ispocetka sve

class TempCamera
{
private:

	unsigned int WIDTH;
	unsigned int HEIGHT;
	float cameraFOV = 45.0f;



public:

	
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 Position = glm::vec3(0, 0, 0);
	glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);

	const glm::vec3 cameraFront = glm::vec3(0.0f, -0.85f, -1.0f);
	const glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraPosition = glm::vec3(0.0f, 12.5f,14.0f);

	float cameraMoveSpeed = 5.0f;
	//float cameraMoveSpeed = 8.5f;

	PickingTexture::PixelInfo currentPixel;

	TempCamera(unsigned int width, unsigned int height)
	{
		WIDTH = width;
		HEIGHT = height;


	}

	glm::mat4 projection_view_matrix();
	glm::mat4 get_PV_static();
	void input(class GLFWwindow* window);
public:
	bool mouseLock = false;
private:

	bool firstClick = true;	
	float FOV = 45.0f;
	float nearPlane = 0.1f;
	float farPlane = 10000.0f;

	float window_width = 640.0f;
	float window_height = 480.0f;

	float speed = 0.075f;
	float sensitivity = 100.0f;





};