#version 450

#include "Include/CommonStructs.glsl"

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in Varying_T
{
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    mat3 TBN;
    flat uint MaterialIndex;
} v_In;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D Textures[];

layout(set = 0, binding = 1, scalar) readonly buffer MaterialsBuffer_T
{
    MaterialData Materials[];
} b_Materials;

void main()
{
    MaterialData material = b_Materials.Materials[v_In.MaterialIndex];
    vec2 uv = v_In.TexCoords;

    vec4 albedo = vec4(0.0);

    // Albedo
    if (material.AlbedoIndex != 0)
        albedo = texture(Textures[nonuniformEXT(material.AlbedoIndex)], uv);
    else
        albedo = vec4(material.Color, 1.0);

    outColor = albedo;

    // TODO: Implement lighting calculations
}
