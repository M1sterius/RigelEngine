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

    mat4 MVP;
    mat4 ModelMat;
    mat3 NormalMat;
};

#endif
