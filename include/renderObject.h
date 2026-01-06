#pragma once
#include <glad/glad.h>
#include <mesh.h>
class RenderObject
{
private:
	GLuint VAO;
	GLuint indicesCount;
public:
	void initialise(GLfloat* data);
	void initialise(Mesh& mesh);
	void render();
};