#version 430 core

struct Cbuffer {
    sampler2D cPosition;
    sampler2D cNormal;
};

struct Lbuffer {
    samplerCube lPosition;
    samplerCube lNormal;
    samplerCube lFlux;
};

layout(std430, binding = 0) buffer SampleBuffer {
    vec3 samples[];
};

uniform Cbuffer cbuffer;
uniform Lbuffer lbuffer;
uniform vec3 lightPos;

out vec4 FragColor;
in vec2 TexCoords;

float attenuate(float d){
    return 1.0 / (1.0 + 0.09 * d + 0.032 * d * d);
}

void main(){
    vec3 FragPos = texture(cbuffer.cPosition, TexCoords).rgb;
    vec3 Normal = texture(cbuffer.cNormal, TexCoords).rgb;
    vec3 fragToLight = FragPos - lightPos;
    vec3 d = normalize(fragToLight);
    vec3 up = abs(d.z) < 0.999 ? vec3(0,0,1) : vec3(0,1,0);
    vec3 t = normalize(cross(up, d));
    vec3 b = cross(d, t);
    vec3 irradiance = vec3(0.0);
    float total_weight = 0.0;
    for (int i = 0; i < 300; i++){
        vec3 xywt = samples[i];
        vec3 sampleCoords = xywt.x * t + xywt.y * b + d;
        float weight = xywt.z;
        total_weight += weight;
        vec3 pFragPos = texture(lbuffer.lPosition, sampleCoords).rgb;
        vec3 pNormal = texture(lbuffer.lNormal, sampleCoords).rgb;
        vec3 pFlux = texture(lbuffer.lFlux, sampleCoords).rgb;
        float tmp = max(0, dot(pNormal, FragPos - pFragPos)) * max(0, dot(Normal, pFragPos - FragPos));
        
        float dist = length(pFragPos - FragPos);
        irradiance += weight * pFlux * tmp * attenuate(dist) / (dist * dist);
    }
    if (total_weight < 0.001){
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else{
        irradiance /= total_weight;
        FragColor = vec4(irradiance * (512 * 512 * 6), 1.0);
    }
}