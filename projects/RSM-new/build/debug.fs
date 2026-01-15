#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube depthMap;
uniform samplerCube normMap;

void main() {
    float closestDepth = texture(depthMap, TexCoords).r;
    FragColor = vec4(vec3(closestDepth), 1.0);
    //FragColor = vec4(texture(normMap, TexCoords, 1.0).rgb, 1.0f);
}