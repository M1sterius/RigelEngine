#ifndef _COMMON_STRUCTS_
#define _COMMON_STRUCTS_

struct MaterialData
{
    uint AlbedoIndex;
    vec3 Color;

    uint MetallicIndex;
    float Metalness;

    uint RougnessIndex;
    float Roughness;

    uint NormalIndex;
    uint AmbientOcclusionIndex;

    vec2 Tiling;
    vec2 Offset;
};

struct MeshData
{
    uint MaterialIndex;

    mat4 MVP;
    mat4 ModelMat;
    mat3 NormalMat;
};

#endif
