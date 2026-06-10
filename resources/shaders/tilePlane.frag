#version 330 core

layout (location = 0) out vec4 color;
uniform sampler2D tex0;
uniform vec3 planeColor;
uniform bool hideNorth;
uniform bool hideSouth;
uniform bool hideEast;
uniform bool hideWest;
uniform float borderAlpha;
uniform float fillAlpha;
in vec2 texCoord;

void main()
{
    vec4 col = texture(tex0, texCoord);
    float border = 0.1f;

    bool isBorder = col.a > 0.0;

    if (hideNorth && texCoord.y > 1.0 - border) isBorder = false;
    if (hideSouth && texCoord.y < border)        isBorder = false;
    if (hideEast  && texCoord.x > 1.0 - border)  isBorder = false;
    if (hideWest  && texCoord.x < border)         isBorder = false;

    if (isBorder)
        color = vec4(planeColor, borderAlpha);
    else
        color = vec4(planeColor, fillAlpha);
}