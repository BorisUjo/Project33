#version 330 core

layout (location = 0) out vec4 color;
uniform sampler2D tex0;
uniform vec3 lightPos;
uniform bool isForest;
uniform bool isTile;
uniform bool isWalkable;
uniform vec3 nationColor;
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

	result *= vec3(0.0, 0.7, 0.25);

	if (isForest)
	{
		result *= vec3(0.0, 0.7, 0.2);
	}
	
	if (isWalkable)
	{
		result += vec3(0.0, 0.2, 0.7);
	}

	if (nationColor.r >= 0.9 && nationColor.g >= 0.9 && nationColor.b >= 0.9)
	{
		color = vec4(result, 1.0) * col;
		return;
	}

	color = vec4(nationColor, 0.8) + vec4(result, 1.0) * col;

}