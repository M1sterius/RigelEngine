#version 450

layout(location = 0) in vec2 v_TexCoords;
layout(location = 1) in flat uint v_DrawIndex;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(v_TexCoords, 0.0, 1.0);
}
