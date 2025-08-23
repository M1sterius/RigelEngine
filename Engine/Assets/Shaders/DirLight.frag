#version 450

layout(location = 0) in vec2 v_TexCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D g_Position;
layout(set = 0, binding = 1) uniform sampler2D g_Normal;
layout(set = 0, binding = 2) uniform sampler2D g_AlbedoSpec;

void main()
{
    outColor = vec4(texture(g_AlbedoSpec, v_TexCoords).rgb, 1.0);
}
