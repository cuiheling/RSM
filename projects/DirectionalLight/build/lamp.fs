#version 330 core

uniform vec3 lightCol;
out vec4 FragColor;

void main(){
    FragColor = vec4(lightCol, 1.0);
}