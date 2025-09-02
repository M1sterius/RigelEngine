#version 450

#include "Include/CommonStructs.glsl"

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable

const uint ERROR_TEXTURE_INDEX = 0;
const uint BLACK_TEXTURE_INDEX = 1;
const uint WHITE_TEXTURE_INDEX = 2;

layout(location = 0) in Varying_T
{
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    mat3 TBN;
    flat uint MaterialIndex;
} v_In;

layout(location = 0) out vec4 g_Position;
layout(location = 1) out vec4 g_NormalRoughness;
layout(location = 2) out vec4 g_AlbedoMetallic;

layout(set = 0, binding = 0) uniform sampler2D Textures[];

layout(set = 0, binding = 1, scalar) readonly buffer MaterialsBuffer_T
{
    MaterialData Materials[];
} b_Materials;

vec3 ObtainNormal(uint textureIndex, vec2 uv)
{
    vec3 normal = texture(Textures[nonuniformEXT(textureIndex)], uv).rgb;
    normal = normal * 2.0 - 1.0;
    normal = normalize(v_In.TBN * normal);

    return normal;
}

void main()
{
    MaterialData material = b_Materials.Materials[v_In.MaterialIndex];
    vec2 uv = v_In.TexCoords;

    g_Position = vec4(v_In.FragPos, 1.0);

    // Albedo
    if (material.AlbedoIndex != 0)
        g_AlbedoMetallic.rgb = texture(Textures[nonuniformEXT(material.AlbedoIndex)], uv).rgb;
    else
        g_AlbedoMetallic.rgb = material.Color;

    // Metallic
    if (material.MetallicIndex != 1)
        g_AlbedoMetallic.a = texture(Textures[nonuniformEXT(material.MetallicIndex)], uv).r;
    else
        g_AlbedoMetallic.a = material.Metalness;

    // Roughness
    if (material.RoughnessIndex != 1)
        g_NormalRoughness.a = texture(Textures[nonuniformEXT(material.RoughnessIndex)], uv).r;
    else
        g_NormalRoughness.a = material.Roughness;

    // Normal
    if (material.NormalIndex != 1)
        g_NormalRoughness.rgb = ObtainNormal(material.NormalIndex, uv);
    else
        g_NormalRoughness.rgb = v_In.Normal;
}
