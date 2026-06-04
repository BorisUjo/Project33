#version 330 core

layout (location = 0) out vec4 color;

uniform sampler2D tex0;
uniform vec4 tintColor;   // set to (1,1,1,1) for no tint
uniform float opacity;    // 0.0 - 1.0

in vec2 texCoord;

void main()
{
    vec4 col = texture(tex0, texCoord);

    if (col.a < 0.1)
        discard;

    color = col * tintColor * vec4(1.0, 1.0, 1.0, opacity);
}