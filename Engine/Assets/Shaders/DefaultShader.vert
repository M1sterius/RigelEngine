#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

layout (push_constant) uniform PushConstants
{
    mat4 mvp;
} modelData;

void main()
{
    gl_Position = modelData.mvp * vec4(a_Position, 1.0);
}
