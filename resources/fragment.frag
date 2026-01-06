#version 330 core

layout (location = 0) out vec4 color;
uniform float time;
in vec2 vertexCol;

void main()
{
	
	color = vec4(vertexCol,0.25*sin(time), 1.0);

}