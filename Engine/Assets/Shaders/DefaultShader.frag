#version 450

#include "Include/CommonStructs.glsl"

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 v_TexCoords;
layout(location = 1) in flat uint v_MaterialIndex;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D textures[];

layout(std430, set = 0, binding = 1) readonly buffer SceneData_T
{
    MaterialData Materials[1024];
    MeshData Meshes[2048];
} SceneData;

void main()
{
    MaterialData material = SceneData.Materials[v_MaterialIndex];

    outColor = texture(textures[nonuniformEXT(material.DiffuseIndex)], v_TexCoords);
}
