#version 450

#include "Include/CommonStructs.glsl"

#extension GL_EXT_scalar_block_layout : enable

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in vec3 a_Normal;

layout(location = 0) out vec2 v_TexCoords;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec3 v_FragPos;
layout(location = 3) out flat uint v_MaterialIndex;

layout(set = 0, binding = 1, scalar) readonly buffer SceneData_T
{
    MaterialData Materials[1024];
    MeshData Meshes[2048];
} SceneData;

layout (push_constant) uniform PushConstants
{
    uint MeshIndex;
} pc_MeshData;

void main()
{
    MeshData meshData = SceneData.Meshes[pc_MeshData.MeshIndex];

    v_TexCoords = a_TexCoords;
    v_Normal = meshData.NormalMat * a_Normal;
    v_FragPos = vec3(meshData.ModelMat * vec4(a_Position, 1.0));
    v_MaterialIndex = meshData.MaterialIndex;

    gl_Position = meshData.MVP * vec4(a_Position, 1.0);
}
