#ifndef _COMMON_STRUCTS_
#define _COMMON_STRUCTS_

struct MaterialData
{
    uint DiffuseIndex;
    uint SpecularIndex;
    uint NormalsIndex;
    float Roughness;
};

struct MeshData
{
    uint MaterialIndex;
    float _pad0[3]; // 12 bytes

    mat4 MVP;
    mat4 ModelMat;
    mat4 NormalMat;
};

#endif
