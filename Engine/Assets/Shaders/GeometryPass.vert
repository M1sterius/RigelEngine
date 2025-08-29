#version 450

#include "Include/CommonStructs.glsl"

#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec4 a_Tangent;

layout(location = 0) out Varying_T
{
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    mat3 TBN;
    flat uint MaterialIndex;
} v_Out;

layout(set = 1, binding = 0, scalar) readonly buffer MeshBuffer_T
{
    MeshData Meshes[];
} b_MeshBuffer;

layout (push_constant) uniform PushConstants
{
    uint MeshIndex;
} pc_MeshData;

void main()
{
    MeshData meshData = b_MeshBuffer.Meshes[pc_MeshData.MeshIndex];

    vec3 T = normalize(meshData.NormalMat * a_Tangent.rgb);
    vec3 N = normalize(meshData.NormalMat * a_Normal);
    vec3 B = normalize(cross(N, T) * a_Tangent.w);

    v_Out.TexCoords = a_TexCoords;
    v_Out.Normal = N;
    v_Out.FragPos = vec3(meshData.ModelMat * vec4(a_Position, 1.0));
    v_Out.TBN = mat3(T, B, N);
    v_Out.MaterialIndex = meshData.MaterialIndex;

    gl_Position = meshData.MVP * vec4(a_Position, 1.0);
}
