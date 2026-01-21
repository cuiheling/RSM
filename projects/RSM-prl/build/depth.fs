#version 330 core
layout (location = 0) out vec3 lPosition;
layout (location = 1) out vec3 lNormal;
layout (location = 2) out vec3 lFlux;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
uniform int Mode1;
uniform int Mode2;
uniform int useTex;
uniform sampler2D albedoMap;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 lightDiff;
uniform vec2 rsmResolution;
uniform float lightOrthoHeight;
uniform float lightOrthoWidth;

void main() {
    lNormal = normalize(Normal);
    lPosition = FragPos - lNormal * 0.02;
    float deltaA = lightOrthoWidth * lightOrthoHeight / (rsmResolution.x * rsmResolution.y);
    vec3 albedo;
    if (useTex == 1){
        albedo = texture(albedoMap, TexCoords).rgb;
    }
    else{
        albedo = abs(lNormal);
    }
    vec3 intensity;
    if (Mode1 == 1){
        intensity = lightCol / (lightOrthoHeight * lightOrthoWidth);
    }
    else{
        intensity = lightCol;
    }
    vec3 inFlux = intensity * deltaA;
    if (Mode2 == 1){
        vec3 lightDir = normalize(lightPos - FragPos);
        lFlux = inFlux * lightDiff * albedo * max(dot(lightDir, lNormal), 0.0);
    }
    else{
        lFlux = inFlux * lightDiff * albedo;
    }
}