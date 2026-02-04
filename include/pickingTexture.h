#pragma once
#include <glad/glad.h>

struct PickingTexture
{
	GLuint framebufferID;
	GLuint pickingTextureID;

	struct PixelInfo
	{
		GLuint objectID = 0;
		GLuint drawID = 0;
		GLuint primID = 0;
	};

	void initialise(int width, int height);
	void enable_writing();
	void disable_writing();
	PixelInfo read_pixel(int x, int y);

};