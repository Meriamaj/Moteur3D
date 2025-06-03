#version 330 core

out vec4 FragColor; // Couleur finale

in vec3 ourColor; // Couleur re�ue du vertex shader


uniform float Ubright;

void main()
{
    FragColor = vec4(ourColor * Ubright, 1.0); // Utiliser la couleur re�ue
}

