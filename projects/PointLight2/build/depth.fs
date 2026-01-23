#version 330 core
layout (location = 0) out vec3 lPosition;
layout (location = 1) out vec3 lNormal;
layout (location = 2) out vec3 lFlux;

in vec2 gCoords;
in vec3 Normal;
in vec4 FragPos;
uniform int Mode1;
uniform int Mode2;
uniform int useTex;
uniform sampler2D albedoMap;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 lightDiff;
uniform vec2 rsmResolution;
uniform float far_plane;

float attenuate(float d){
    return 1.0 / (1.0 + 0.09 * d + 0.032 * d * d);
}

void main() {
    float lightDistance = length(lightPos - FragPos.xyz);

    lNormal = normalize(Normal);
    lPosition = FragPos.xyz - lNormal * 0.02;
    vec2 uv = (gl_FragCoord.xy + 0.5) / rsmResolution;
    lFlux = vec3(uv, 0.0);
    float x = uv.x * 2.0 - 1.0;
    float y = uv.y * 2.0 - 1.0;
    float dx = 2.0 / rsmResolution.x;
    float dy = 2.0 / rsmResolution.y;
    float deltaOmega = dx * dy / pow(x*x + y*y + 1.0, 1.5);
    vec3 albedo;
    if (useTex == 1){
        albedo = texture(albedoMap, gCoords).rgb;
    }
    else{
        albedo = abs(lNormal);
    }
    vec3 lightDir = normalize(lightPos - FragPos.xyz);
    vec3 intensity;
    if (Mode1 == 1){
        intensity = lightCol;
    }
    else{
        intensity = lightCol / (4.0 * 3.14159);
    }
    if (Mode2 == 1){
        vec3 inFlux = intensity * deltaOmega * attenuate(lightDistance);
        lFlux = inFlux * lightDiff * albedo * max(dot(lightDir, lNormal), 0.0);
    }
    else{
        vec3 inFlux = intensity * deltaOmega;
        lFlux = inFlux * lightDiff * albedo;
    }
    gl_FragDepth = lightDistance / far_plane;
}