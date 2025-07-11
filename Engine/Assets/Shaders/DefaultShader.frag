#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 v_TexCoords;
layout(location = 1) in flat uint v_DrawIndex;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D textures[];

void main()
{
//    outColor = vec4(v_TexCoords, 0.0, 1.0);
    outColor =  texture(textures[nonuniformEXT(v_DrawIndex)], v_TexCoords);
}
