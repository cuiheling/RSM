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

out vec4 FragColor;
in vec2 TexCoords;

void main(){
    vec3 FragPos = texture(cbuffer.cPosition, TexCoords).rgb;
    vec3 Normal = texture(cbuffer.cNormal, TexCoords).rgb;
    vec3 fragToLight = FragPos - lightPos;
    vec3 d = normalize(fragToLight);
    vec3 irradiance = vec3(0.0);
    float total_weight = 0.0;
    for (int i = 0; i < 400; i++){
        vec4 xyzw = samples[i];
        vec3 sampleCoords = xyzw.xyz + d;
        float weight = xyzw.w;
        total_weight += weight;
        vec3 pFragPos = texture(lbuffer.lPosition, sampleCoords).rgb;
        vec3 pNormal = texture(lbuffer.lNormal, sampleCoords).rgb;
        vec3 pFlux = texture(lbuffer.lFlux, sampleCoords).rgb;
        float tmp = max(0, dot(pNormal, FragPos - pFragPos)) * max(0, dot(Normal, pFragPos - FragPos));
        irradiance += weight * pFlux * tmp / pow(length(FragPos - pFragPos), 4.0);
    }
    if (total_weight < 0.001){
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else{
        irradiance /= total_weight;
        FragColor = vec4(irradiance * (512 * 512 * 6), 1.0);
    }
    //FragColor = vec4(texture(lbuffer.lNormal, fragToLight).rgb, 1.0);
}