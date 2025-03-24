#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
}
