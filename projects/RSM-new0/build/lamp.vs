#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;
uniform mat4 lightProj;
uniform mat4 lightView;
uniform int Reverse;

void main(){
    FragPos = vec3(Model * vec4(aPos, 1.0));
    vec4 fragpos = vec4(FragPos, 1.0);
    TexCoords = aTexCoord;
    if (Reverse == 1){
        Normal = transpose(inverse(mat3(Model))) * (-aNormal);
    }
    else{
        Normal = transpose(inverse(mat3(Model))) * aNormal;
    }
    FragPosLightSpace = lightProj * lightView * fragpos;
    gl_Position = Proj * View * fragpos;
}