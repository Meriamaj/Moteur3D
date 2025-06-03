#version 450

uniform vec3 uMaterialDiffuse;   // Diffuse color of the material
uniform vec3 uMaterialSpecular;  // Specular color of the material
uniform float uShininess;        // Specular shininess exponent
uniform vec3 uAmbientColor;      // Ambient color

// Texture maps
uniform int uHasAmbientMap;
layout( binding = 0 ) uniform sampler2D uAmbientMap;
uniform int uHasDiffuseMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
uniform int uHasSpecularMap;
layout( binding = 2 ) uniform sampler2D uSpecularMap;
uniform int uHasShininessMap;
layout( binding = 3 ) uniform sampler2D uShininessMap;
uniform int uHasNormalMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;

in vec2 cordTex;
in vec3 vNormal; 
in vec3 fragPos;  
in vec3 lightPos; 

uniform int uSpecularType;

out vec4 FragColor;

void main()
{

//caculating composants
float opacite = uHasDiffuseMap  == 1 ? texture(uDiffuseMap,  cordTex).w  : 1 ;
if (opacite < 0.5 ) discard;
vec3 ambient  = uHasAmbientMap  == 1 ? texture(uAmbientMap,  cordTex).xyz  : uAmbientColor ;
vec3 diffuse  = uHasDiffuseMap  == 1 ? texture(uDiffuseMap,  cordTex).xyz  : uMaterialDiffuse ;
vec3 specular = uHasSpecularMap == 1 ? texture(uSpecularMap, cordTex).xxx  : uMaterialSpecular ;

// normal
vec3 revNorm = normalize( uHasNormalMap == 1 ? texture(uNormalMap, cordTex).xyz * 2 - 1 : vNormal ) ;

float shini = uHasShininessMap == 1 ? texture(uShininessMap, cordTex).x : uShininess;

vec3 lightDir = normalize(lightPos - fragPos) ;
vec3 fragToCam =  - fragPos ;

revNorm = dot( revNorm , normalize( fragToCam ) ) < 0.0 ? -revNorm : revNorm;
revNorm = normalize(revNorm);
	
diffuse *= max( dot( normalize( lightDir ), revNorm ), 0.0 );
	
// apply the right specular type
switch ( uSpecularType ) {
case 0: // None
	specular = vec3(0);
	break;

case 1: // Phong
	specular *= pow( max( dot( normalize( reflect( -lightDir, revNorm ) ), normalize( -fragPos ) ), 0.0 ), shini );
	break;

case 2: // Blinn
	vec3 halfwayDirec =  normalize( normalize( lightDir ) + normalize( -fragPos ) ); // H = normalize(L + V)
	specular *= pow( max( dot( normalize( halfwayDirec ), revNorm ), 0.0 ), shini );
	break;
}
  // Combine all components for final color
  vec3 finalColor = ambient + diffuse + specular;

  // Output the fragment color
  FragColor = vec4(finalColor, 1.0);
}
