#version 330 core
layout (location = 0) out vec3 cPosition;
layout (location = 1) out vec3 cNormal;
layout (location = 2) out vec4 cAlbedoSpec;

in vec2 TexCoords;
in vec3 Position;
in vec3 Normal;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};

uniform int useTex;
uniform Material material;

void main() {
    cPosition = Position;
    cNormal = normalize(Normal);
    if (useTex == 1){
        cAlbedoSpec.rgb = texture(material.texture_diffuse1, TexCoords).rgb;
        cAlbedoSpec.a = texture(material.texture_specular1, TexCoords).r;
    }
    else{
        cAlbedoSpec.rgb = abs(cNormal);
        cAlbedoSpec.a = 0.0;
    }
}