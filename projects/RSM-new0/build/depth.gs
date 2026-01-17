#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 lightProj;
uniform mat4 lightViews[6];

in vec2 TexCoords[];
in vec3 Norm[];
out vec2 gCoords;
out vec3 Normal;
out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main() {
    for(int face = 0; face < 6; face++) {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for(int i = 0; i < 3; i++) {
            // for each triangle's vertices
            FragPos = gl_in[i].gl_Position;
            gCoords = TexCoords[i];
            Normal = Norm[i];
            gl_Position = lightProj * lightViews[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}