#include <pickingTexture.h>
void PickingTexture::initialise(int width, int height)
{
	glGenFramebuffers(1, &framebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);

	glGenTextures(1, &pickingTextureID);
	glBindTexture(GL_TEXTURE_2D, pickingTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, width, height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, (void*)0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingTextureID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void PickingTexture::enable_writing()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebufferID);
}

void PickingTexture::disable_writing()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}
PickingTexture::PixelInfo PickingTexture::read_pixel(int x, int y)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebufferID);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	PixelInfo pixel;
	GLuint data[3];
	glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, data);

	pixel.objectID = data[0];
	pixel.drawID = data[1];
	pixel.primID = data[2];

	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);


	return pixel;
}
