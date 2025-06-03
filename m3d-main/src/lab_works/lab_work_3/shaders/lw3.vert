#version 330 core

layout(location = 0) in vec3 aPos;    // Position du sommet
layout(location = 1) in vec3 aColor;  // Couleur du sommet

out vec3 vColor; 

uniform mat4  uModelMatrix;
uniform mat4  uViewMatrix;
uniform mat4  uProjMatrix;

void main() {
    gl_Position = uProjMatrix*uViewMatrix*uModelMatrix* vec4(aPos, 1.0); 
    vColor = aColor;              // Transmettre la couleur
}
