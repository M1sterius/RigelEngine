#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;
layout(location = 2) in vec3 a_Normal;

layout(location = 0) out vec2 v_TexCoords;
layout(location = 1) out flat uint v_MaterialIndex;

struct MaterialData
{
    uint DiffuseIndex;
    uint SpecularIndex;
    uint NormalsIndex;
    float Roughness;
};

struct MeshData
{
    mat4 MVP;
    uint MaterialIndex;
    float _pad0[3];
};

layout(std430, set = 0, binding = 1) readonly buffer SceneData_T
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
    v_MaterialIndex = meshData.MaterialIndex;

    gl_Position = meshData.MVP * vec4(a_Position, 1.0);
}
