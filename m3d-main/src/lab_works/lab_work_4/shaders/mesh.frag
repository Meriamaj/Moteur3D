#version 450

// Input from the vertex shader
in vec3 vNormal;    // Interpolated normal
in vec3 fragPos;    // Fragment position in view space

// Uniforms
uniform vec3 uMaterialDiffuse;   // Diffuse color of the material
uniform vec3 uMaterialSpecular;  // Specular color of the material
uniform float uShininess;        // Specular shininess exponent
uniform vec3 uLightPosition;     // Light position (view space)
uniform vec3 uLightColor;        // Light color
uniform vec3 uAmbientColor;      // Ambient color
uniform vec3 uCameraPosition;    // Camera position (view space)

// Output color
out vec4 FragColor;

void main()
{
    // Normalize interpolated normal and compute light direction
    vec3 norm = normalize(vNormal);
    vec3 lightDirec = normalize(uLightPosition - fragPos);

    // Compute view direction
    vec3 viewDirec = normalize(uCameraPosition - fragPos);

    // Compute halfway vector for Blinn-Phong specular
    vec3 halfwayDirec = normalize(lightDirec + viewDirec);

    // Ambient component
    vec3 ambient = vec3(0.7, 0.7, 0.7) * uMaterialDiffuse;

    // Diffuse component (Lambertian reflection)
    float diff = max(dot(norm, lightDirec), 0.0);
    vec3 diffuse = diff * uMaterialDiffuse * uLightColor;

    // Specular component (Blinn-Phong reflection)
    float spec = pow(max(dot(norm, halfwayDirec), 0.0), uShininess);
    vec3 specular = spec * uMaterialSpecular * uLightColor;

    // Combine all components for final color
    vec3 finalColor = ambient + diffuse + specular;

    // Output the fragment color
    FragColor = vec4(finalColor, 1.0);
}