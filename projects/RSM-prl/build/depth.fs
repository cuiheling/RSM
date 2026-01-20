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
uniform float lightOrthoHeight;
uniform float lightOrthoWidth;

void main() {
    lNormal = normalize(Normal);
    lPosition = FragPos - lNormal * 0.02;
    vec3 albedo;
    if (useTex == 1){
        albedo = texture(albedoMap, TexCoords).rgb;
    }
    else{
        albedo = abs(lNormal);
    }
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 inFlux = lightCol / (rsmResolution.x * rsmResolution.y) ;
    lFlux = inFlux * albedo * max(dot(lightDir, lNormal), 0.0);
    //lFlux = lightCol * albedox / (rsmResolution.x * rsmResolution.y);
}