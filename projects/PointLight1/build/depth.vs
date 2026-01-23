#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoords;
out vec3 Norm;

uniform mat4 Model;
uniform int Reverse;

void main() {
    TexCoords = aTexCoord;
    if (Reverse == 1){
        Norm = transpose(inverse(mat3(Model))) * (-aNormal);
    }
    else{
        Norm = transpose(inverse(mat3(Model))) * aNormal;
    }
    gl_Position = Model * vec4(aPos, 1.0);
}