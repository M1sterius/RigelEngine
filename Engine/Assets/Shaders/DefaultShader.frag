#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec2 v_TexCoords;

void main()
{
    outColor = vec4(v_TexCoords, 0.0, 1.0);
}
