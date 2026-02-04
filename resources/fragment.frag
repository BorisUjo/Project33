#version 330 core

layout (location = 0) out vec4 color;
uniform sampler2D tex0;
uniform vec3 lightPos;
uniform bool isForest;
uniform bool isTile;
in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
void main()
{
	
	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1,1,1);

	vec4 col = texture(tex0, texCoord);

	if (col.a < 0.1)
	{
		discard;
	}

	

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - fragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(1,1,1);

	vec3 result = (ambient + diffuse) * vec3(1,1,1);

	if (isForest)
	{
		result *= vec3(0.0, 0.7, 0.2);
	}

	// temporary
	if (isTile)
	{
		result *= vec3(0.0, 0.7, 0.25);
	}

	color = vec4(result, 1.0) * col;

}