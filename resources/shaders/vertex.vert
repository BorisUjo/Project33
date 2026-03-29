#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
 
uniform float glfwTime;
uniform mat4 transform;
uniform mat4 PVM;

out vec2 texCoord;
out vec3 fragPos;
out vec3 normal;
void main()
{
	gl_Position = PVM * transform * vec4(aPosition, 1.0f);

	texCoord = aTexCoord;	

	fragPos = vec3(transform * vec4(aPosition, 1.0f));
	normal = aNormal;


}