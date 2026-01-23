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
    vec4 samples[];
};

uniform Cbuffer cbuffer;
uniform Lbuffer lbuffer;
uniform vec3 lightPos;
uniform int Mode4;
uniform int Mode5;
uniform int sampleNum;

out vec4 FragColor;
in vec2 TexCoords;

float attenuate(float d){
    return 1.0 / (1.0 + 0.09 * d + 0.032 * d * d);
}

void main(){
    vec3 FragPos = texture(cbuffer.cPosition, TexCoords).rgb;
    vec3 Normal = texture(cbuffer.cNormal, TexCoords).rgb;
    vec3 fragToLight = FragPos - lightPos;
    vec3 irradiance = vec3(0.0);
    float total_weight = 0.0, total_count = 0.0;
    for (int i = 0; i < sampleNum; i++){
        vec4 xyzw = samples[i];
        vec3 sampleCoords = xyzw.xyz + normalize(fragToLight);
        float weight = xyzw.w;
        total_weight += weight;
        total_count += 1.0;
        vec3 pFragPos = texture(lbuffer.lPosition, sampleCoords).rgb;
        vec3 pNormal = texture(lbuffer.lNormal, sampleCoords).rgb;
        vec3 pFlux = texture(lbuffer.lFlux, sampleCoords).rgb;
        float tmp = max(0, dot(pNormal, FragPos - pFragPos)) * max(0, dot(Normal, pFragPos - FragPos));
        
        float dist = length(pFragPos - FragPos);
        if (Mode4 == 1){
            irradiance += weight * pFlux * tmp / pow(dist, 4.0);
        }
        else{
            irradiance += weight * pFlux * tmp * attenuate(dist) / (dist * dist);
        }
    }
    if (total_weight < 0.001){
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else{
        if (Mode5 == 1){
            irradiance /= total_weight;
        }
        else{
            irradiance /= total_count;
        }
        FragColor = vec4(irradiance * (512 * 512 * 6), 1.0);
    }
}