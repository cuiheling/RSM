#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoords;
out vec3 Normal;
out vec3 Position;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;
uniform int Reverse;

void main(){
    if (Reverse == 1){
        Normal = transpose(inverse(mat3(Model))) * (-aNormal);
    }
    else{
        Normal = transpose(inverse(mat3(Model))) * aNormal;
    }
    Position = vec3(Model * vec4(aPos, 1.0));
    gl_Position = Proj * View * Model * vec4(aPos, 1.0);
    TexCoords = aTexCoord;
}