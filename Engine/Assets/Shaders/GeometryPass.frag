#version 450

#include "Include/CommonStructs.glsl"

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 v_TexCoords;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec3 v_FragPos;
layout(location = 3) in flat uint v_MaterialIndex;

layout(location = 0) out vec4 g_Position;
layout(location = 1) out vec4 g_Normal;
layout(location = 2) out vec4 g_AlbedoSpec;

layout(set = 0, binding = 0) uniform sampler2D textures[];

layout(std430, set = 0, binding = 1) readonly buffer SceneData_T
{
    MaterialData Materials[1024];
    MeshData Meshes[2048];
} SceneData;

void main()
{
    MaterialData material = SceneData.Materials[v_MaterialIndex];

    g_Position = vec4(v_FragPos, 0.0);
    g_Normal = vec4(normalize(v_Normal), 0.0);
    g_AlbedoSpec.rgb = texture(textures[nonuniformEXT(material.DiffuseIndex)], v_TexCoords).rgb;
    g_AlbedoSpec.a = texture(textures[nonuniformEXT(material.SpecularIndex)], v_TexCoords).r;
}
