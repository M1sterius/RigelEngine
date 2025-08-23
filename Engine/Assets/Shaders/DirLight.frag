#version 450

layout(location = 0) in vec2 v_TexCoords;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D g_Position;
layout(set = 0, binding = 1) uniform sampler2D g_Normal;
layout(set = 0, binding = 2) uniform sampler2D g_AlbedoSpec;

layout (push_constant) uniform PushConstants
{
    vec3 pos;
} pc_CamPos;

const float AMBIENT_INTENSITY = 0.15;

const vec3 LIGHT_DIR = vec3(1.0, -1.0, 0.0);
const vec3 LIGHT_COLOR = vec3(1.0);
const float LIGHT_INTENSITY = 1.0;

vec3 CalcLight(vec3 normal, vec3 viewDir, vec3 diffuse, float specular)
{
    vec3 lightDir = -normalize(LIGHT_DIR);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseRes = diff * LIGHT_COLOR * LIGHT_INTENSITY * diffuse;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specularRes = spec * LIGHT_COLOR * LIGHT_INTENSITY * specular;

    return diffuseRes + specularRes;
}

void main()
{
    vec3 fragPos = texture(g_Position, v_TexCoords).rgb;
    vec3 normal = texture(g_Normal, v_TexCoords).rgb;
    vec3 albedo = texture(g_AlbedoSpec, v_TexCoords).rgb;
    float specular = texture(g_AlbedoSpec, v_TexCoords).a;

    vec3 viewDir = normalize(pc_CamPos.pos - fragPos);

    vec3 light = CalcLight(normal, viewDir, albedo, specular);

    vec3 color = (albedo * AMBIENT_INTENSITY) + light;

    outColor = vec4(color, 1.0);
}
