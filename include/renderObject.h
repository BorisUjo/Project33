#pragma once
#include <glad/glad.h>
#include <mesh.h>
class RenderObject
{
private:
	bool renderEnabled = true;

	GLuint VAO;
	GLuint indicesCount;
public:
	void initialise(GLfloat* data);
	void initialise(Mesh& mesh);
	void set_render_state(bool state);
	void render();

	bool get_render_state();
};