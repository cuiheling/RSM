#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube depthMap;
uniform samplerCube normMap;
uniform samplerCube fluxMap;

void main() {
    //float closestDepth = texture(depthMap, TexCoords).r;
    //FragColor = vec4(vec3(closestDepth), 1.0);
    vec3 closestFlux = texture(fluxMap, TexCoords).rgb;
    FragColor = vec4(closestFlux, 1.0);
    //FragColor = vec4(texture(normMap, TexCoords, 1.0).rgb, 1.0f);
}