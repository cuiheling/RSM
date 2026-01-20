#version 330 core
layout (location = 0) out vec3 lPosition;
layout (location = 1) out vec3 lNormal;
layout (location = 2) out vec3 lFlux;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
uniform int useTex;
uniform sampler2D albedoMap;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec2 rsmResolution;
uniform float fovX;
uniform float fovY;

float attenuate(float d){
    return 1.0 / (1.0 + 0.09 * d + 0.032 * d * d);
}

void main() {
    lNormal = normalize(Normal);
    lPosition = FragPos - lNormal * 0.02;
    vec2 uv = (gl_FragCoord.xy + 0.5) / rsmResolution;
    float x = (uv.x * 2.0 - 1.0) * tan(fovX * 0.5);
    float y = (uv.y * 2.0 - 1.0) * tan(fovY * 0.5);
    float dx = 2.0 * tan(fovX * 0.5) / rsmResolution.x;
    float dy = 2.0 * tan(fovY * 0.5) / rsmResolution.y;
    float deltaOmega = dx * dy / pow(x*x + y*y + 1.0, 1.5);
    vec3 albedo;
    if (useTex == 1){
        albedo = texture(albedoMap, TexCoords).rgb;
    }
    else{
        albedo = abs(lNormal);
    }

    float dist = length(lightPos - FragPos);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 intensity = lightCol / (2.0 * 3.14159 / 3.0);
    vec3 inFlux = intensity * deltaOmega * attenuate(dist);
    lFlux = inFlux * albedo * max(dot(lightDir, lNormal), 0.0);
    //lFlux = lightCol / (2.0 * 3.14159 / 3.0) * deltaOmega * albedo;
}