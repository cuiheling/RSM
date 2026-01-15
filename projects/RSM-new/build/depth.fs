#version 330 core
layout (location = 0) out vec3 lPosition;
layout (location = 1) out vec3 lNormal;
layout (location = 2) out vec3 lFlux;

in vec2 gCoords;
in vec3 Normal;
in vec4 FragPos;
uniform int useTex;
uniform sampler2D albedoMap;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec2 rsmResolution;
uniform float far_plane;
uniform float fovX;
uniform float fovY;

void main() {
    lNormal = normalize(Normal);
    lPosition = vec3(FragPos) - lNormal * 0.02;
    vec2 uv = (gl_FragCoord.xy + 0.5) / rsmResolution;
    float x = (uv.x * 2.0 - 1.0) * tan(fovX * 0.5);
    float y = (uv.y * 2.0 - 1.0) * tan(fovY * 0.5);
    float dx = 2.0 * tan(fovX * 0.5) / rsmResolution.x;
    float dy = 2.0 * tan(fovY * 0.5) / rsmResolution.y;
    float deltaOmega = dx * dy / pow(x*x + y*y + 1.0, 1.5);
    vec3 albedo;
    if (useTex == 1){
        albedo = texture(albedoMap, gCoords).rgb;
    }
    else{
        albedo = abs(lNormal);
    }
    lFlux = lightCol * deltaOmega * albedo;
    float lightDistance = length(FragPos.xyz - lightPos);
    gl_FragDepth = lightDistance / far_plane;
}