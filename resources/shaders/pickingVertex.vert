
#version 330 core

layout (location = 0) in vec3 aPosition;
uniform mat4 transform;
uniform mat4 PVM;
void main()
{
	gl_Position = PVM * transform * vec4(aPosition, 1.0f);

}