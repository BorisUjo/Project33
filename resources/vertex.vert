#version 330 core

layout (location = 0) in vec3 p;
layout (location = 1) in vec3 n;
layout (location = 2) in vec2 t;
 
uniform float glfwTime;
uniform mat4 transform;

out vec2 vertexCol;

void main()
{
	float scale = 0.025f;
	vertexCol = t;

	gl_Position = transform * vec4(p * scale, 1);

}