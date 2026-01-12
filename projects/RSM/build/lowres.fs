#version 430 core

struct Cbuffer {
    sampler2D cPosition;
    sampler2D cNormal;
};

struct Lbuffer {
    sampler2D lPosition;
    sampler2D lNormal;
    sampler2D lFlux;
};

layout(std430, binding = 0) buffer SampleBuffer {
    vec3 samples[];
};

uniform Cbuffer cbuffer;
uniform Lbuffer lbuffer;
uniform mat4 lightProj;
uniform mat4 lightView;

out vec4 FragColor;
in vec2 TexCoords;

void main(){
    vec3 FragPos = texture(cbuffer.cPosition, TexCoords).rgb;
    vec3 Normal = texture(cbuffer.cNormal, TexCoords).rgb;
    vec4 FragPosLightSpace = lightProj * lightView * vec4(FragPos, 1.0);
    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    vec3 irradiance = vec3(0.0);
    float total_weight = 0.0;
    for (int i = 0; i < 400; i++){
        vec3 xywt = samples[i];
        vec2 sampleCoords = xywt.xy + projCoords.xy;
        if (sampleCoords.x < 0 || sampleCoords.x > 1 || sampleCoords.y < 0 || sampleCoords.y > 1) {
            continue;
        }
        float weight = xywt.z;
        total_weight += weight;
        vec3 pFragPos = texture(lbuffer.lPosition, sampleCoords).rgb;
        vec3 pNormal = texture(lbuffer.lNormal, sampleCoords).rgb;
        vec3 pFlux = texture(lbuffer.lFlux, sampleCoords).rgb;
        float tmp = max(0, dot(pNormal, FragPos - pFragPos)) * max(0, dot(Normal, pFragPos - FragPos));
        irradiance += weight * pFlux * tmp / pow(length(FragPos - pFragPos), 4.0);
    }
}