#version 330 core

in vec3 vColor; // Couleur interpolée reçue du vertex shader
out vec4 FragColor; // Couleur finale du fragment

void main() {
    FragColor = vec4(vColor, 1.0); // Appliquer la couleur
}
