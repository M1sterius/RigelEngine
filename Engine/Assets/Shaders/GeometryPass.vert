//#version 450
//
//layout(location = 0) in vec3 a_Position;
//layout(location = 1) in vec2 a_TexCoords;
//layout(location = 2) in vec3 a_Normal; // TODO: Pass normal
//
//layout(location = 0) out vec2 v_TexCoords;
//layout(location = 1) out flat uint v_MaterialIndex;
//
//layout (push_constant) uniform PushConstants
//{
//    mat4 MVP;
//    uint MaterialIndex;
//} pc_ModelData;
//
//void main()
//{
//    gl_Position = pc_ModelData.MVP * vec4(a_Position, 1.0);
//    v_TexCoords = a_TexCoords;
//    v_MaterialIndex = pc_ModelData.MaterialIndex;
//}
