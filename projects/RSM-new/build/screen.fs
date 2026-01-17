#version 430 core

struct Cbuffer {
    sampler2D cPosition;
    sampler2D cNormal;
    sampler2D cAlbedoSpec;
};

struct Lbuffer {
    samplerCube lPosition;
    samplerCube lNormal;
    samplerCube lFlux;
};

struct Light{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 color;
};

uniform sampler2D lowresMap;
uniform samplerCube shadowMap;
uniform Light light;
uniform Cbuffer cbuffer;
uniform Lbuffer lbuffer;
uniform vec3 viewPos;
uniform float shininess;
uniform float far_plane;
uniform mat4 lightProj;
uniform mat4 lightViews[6];

layout(std430, binding = 0) buffer SampleBuffer {
    vec3 samples[];
};

out vec4 FragColor;
in vec2 TexCoords;

int similar(vec2 xCoords, vec2 xpCoords){
    vec3 FragPos = texture(cbuffer.cPosition, xCoords).rgb;
    vec3 Normal = texture(cbuffer.cNormal, xCoords).rgb;
    vec3 pFragPos = texture(cbuffer.cPosition, xpCoords).rgb;
    vec3 pNormal = texture(cbuffer.cNormal, xpCoords).rgb;
    if (length(Normal - pNormal) > 0.02 || abs(dot(Normal, pNormal)) < 0.3){
        return 0;
    }
    else{
        return 1;
    }
}

vec3 sampleRSM(){
    vec3 FragPos = texture(cbuffer.cPosition, TexCoords).rgb;
    vec3 Normal = texture(cbuffer.cNormal, TexCoords).rgb;
    vec3 fragToLight = FragPos - light.position;
    vec3 d = normalize(fragToLight);
    vec3 up = abs(d.z) < 0.999 ? vec3(0,0,1) : vec3(0,1,0);
    vec3 t = normalize(cross(up, d));
    vec3 b = cross(d, t);
    vec3 irradiance = vec3(0.0);
    float total_weight = 0.0;
    for (int i = 0; i < 400; i++){
        vec3 xywt = samples[i];
        vec3 sampleCoords = xywt.x * t + xywt.y * b + d;
        float weight = xywt.z;
        total_weight += weight;
        vec3 pFragPos = texture(lbuffer.lPosition, sampleCoords).rgb;
        vec3 pNormal = texture(lbuffer.lNormal, sampleCoords).rgb;
        vec3 pFlux = texture(lbuffer.lFlux, sampleCoords).rgb;
        float tmp = max(0, dot(pNormal, FragPos - pFragPos)) * max(0, dot(Normal, pFragPos - FragPos));
        irradiance += weight * pFlux * tmp / pow(length(FragPos - pFragPos), 4.0);
    }
    if (total_weight < 0.001){
        return vec3(0.0, 0.0, 0.0);
    }
    else{
        irradiance /= total_weight;
        return vec3(irradiance * (512 * 512 * 6));
    }
    //return texture(lbuffer.lNormal, fragToLight).rgb;
}

void main(){
    vec3 FragPos = texture(cbuffer.cPosition, TexCoords).rgb;
    vec3 Normal = texture(cbuffer.cNormal, TexCoords).rgb;
    vec3 Diffuse = texture(cbuffer.cAlbedoSpec, TexCoords).rgb;
    float Specular = texture(cbuffer.cAlbedoSpec, TexCoords).a;
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(light.position - FragPos);
    vec3 ambient = light.ambient * Diffuse * light.color;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * Diffuse * light.color;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, norm), 0.0), shininess);
    vec3 specular = spec * light.specular * Specular * light.color;
    
    vec3 fragToLight = FragPos - light.position; 
    float currentDepth = length(fragToLight);
    vec3 sampleOffsetDirections[20] = vec3[](
        vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
        vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
        vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
        vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
        vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );
    float shadow = 0.0;
    int samples = 20;
    float bias = max(0.01 * (1.0 - dot(norm, lightDir)), 0.005);
    float viewDistance = length(viewPos - FragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for (int i = 0; i < samples; i++) {
        float closestDepth = texture(shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;   // Undo mapping [0;1]
        shadow += (currentDepth -  bias > closestDepth ? 1.0 : 0.0);
    }
    shadow /= float(samples);
    vec3 direct = ambient + (diffuse + specular) * (1.0 - shadow);

    vec3 indirect;
    vec2 lowresTSize = 1.0 / textureSize(lowresMap, 0);
    float lowresNx = int(TexCoords.x / lowresTSize.x) * lowresTSize.x;
    float lowresNy = int(TexCoords.y / lowresTSize.y) * lowresTSize.y;
    vec2 lowres00 = vec2(lowresNx, lowresNy);
    vec2 lowres01 = vec2(lowresNx, lowresNy + lowresTSize.y);
    vec2 lowres10 = vec2(lowresNx + lowresTSize.x, lowresNy);
    vec2 lowres11 = vec2(lowresNx + lowresTSize.x, lowresNy + lowresTSize.y);
    vec3 F00 = texture(lowresMap, lowres00).rgb;
    vec3 F10 = texture(lowresMap, lowres10).rgb;
    vec3 F01 = texture(lowresMap, lowres01).rgb;
    vec3 F11 = texture(lowresMap, lowres11).rgb;
    float uu = TexCoords.x - lowresNx;
    float vv = TexCoords.y - lowresNy;
    if (uu < 0.00001 && vv < 0.00001) {
        indirect = F00;
    }
    else if (uu < 0.00001) {
        if (similar(TexCoords.xy, lowres00) == 0 || similar(TexCoords.xy, lowres01) == 0){
            indirect = sampleRSM();
            //indirect = vec3(1.0) - direct;
        }
        else{
            indirect = vv * F01 + (1 - vv) * F00;
        }
    }
    else if (vv < 0.00001) {
        if (similar(TexCoords.xy, lowres00) == 0 || similar(TexCoords.xy, lowres10) == 0){
            indirect = sampleRSM();
            //indirect = vec3(1.0) - direct;
        }
        else{
            indirect = uu * F10 + (1 - uu) * F00;
        }
    }
    else {
        int count = similar(TexCoords.xy, lowres00) + similar(TexCoords.xy, lowres01);
        count += similar(TexCoords.xy, lowres10) + similar(TexCoords.xy, lowres11);
        if (count < 3){
            indirect = sampleRSM();
            //indirect = vec3(1.0) - direct;
        }
        else{
            indirect = uu * vv * F11 + (1-uu) * vv * F01 + uu * (1-vv) * F10 + (1-uu) * (1-vv) * F00;
        }
    }
    //vec3 fragColor = pow(lighting, vec3(1.0/2.2));
    FragColor = vec4(direct + indirect, 1.0);
}