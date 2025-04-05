#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

layout (binding = 0) uniform UBO
{
    mat4 u_MVP;
} ubo;

void main()
{
    gl_Position = ubo.u_MVP * vec4(a_Position, 1.0);
}
