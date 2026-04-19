#version 330 core

layout (location = 0) out vec4 color;
uniform sampler2D tex0;
uniform vec3 planeColor;
in vec2 texCoord;
void main()
{
	vec4 col = texture(tex0, texCoord);

	

	if (col.a > 0.0)
    {
        color = col * vec4(planeColor, 1.0f);
    }
    else
    {
        color = vec4(planeColor, 0.25f); // adjust alpha for transparency level
    }

}