#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoords;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 lightProj;
uniform mat4 lightView;
uniform mat4 Model;
uniform int Reverse;

void main() {
    TexCoords = aTexCoord;
    if (Reverse == 1){
        Normal = transpose(inverse(mat3(Model))) * (-aNormal);
    }
    else{
        Normal = transpose(inverse(mat3(Model))) * aNormal;
    }
    FragPos = vec3(Model * vec4(aPos, 1.0));
    gl_Position = lightProj * lightView * Model * vec4(aPos, 1.0);
}