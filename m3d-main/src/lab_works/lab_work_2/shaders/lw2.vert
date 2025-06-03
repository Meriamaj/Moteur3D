#version 330 core

layout(location = 0) in vec2 aPos;    // Position des sommets
layout(location = 1) in vec3 aColor;  // Couleur des sommets

out vec3 ourColor; // Variable de sortie pour passer la couleur au fragment shader

uniform float uTranslationX;


void main()
{
    gl_Position = vec4(aPos.x+ uTranslationX,aPos.y, 0.0, 1.0); // Définir la position
    ourColor = aColor; // Passer la couleur au fragment shader
}

