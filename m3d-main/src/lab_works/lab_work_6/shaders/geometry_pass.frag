#version 450

in vec3 fragAmbientColor;
in vec3 fragDiffuseColor;
in vec3 fragSpecularColor;
in float fragShininess;

layout( binding = 0 ) uniform sampler2D uAmbientMap;
layout( binding = 1 ) uniform sampler2D uDiffuseMap;
layout( binding = 2 ) uniform sampler2D uSpecularMap;
layout( binding = 3 ) uniform sampler2D uShininessMap;
layout( binding = 4 ) uniform sampler2D uNormalMap;
uniform int uHasAmbientMap;
uniform int uHasDiffuseMap;
uniform int uHasSpecularMap;
uniform int uHasShininessMap;
uniform int uHasNormalMap;

in vec2 fragTexCo;
in vec3 vNormal;   // Normal
in vec3 fragPos; // Frag position from the camera (tangent space if uHasNormalMap = 1)
in mat3 TBN;

layout( location = 0 ) out vec3 outPos;
layout( location = 1 ) out vec3 outNormal;
layout( location = 2 ) out vec3 outAmbient;
layout( location = 3 ) out vec3 outDiffuse;
layout( location = 4 ) out vec4 outSpecular;

void main()
{


	float opacite = uHasDiffuseMap  == 1 ? texture(uDiffuseMap,  fragTexCo).w  : 1 ;
	if (opacite < 0.5 ) discard;

	vec3 amb  = uHasAmbientMap  == 1 ? texture(uAmbientMap,  fragTexCo).xyz  : fragAmbientColor ;
	vec3 diff  = uHasDiffuseMap  == 1 ? texture(uDiffuseMap,  fragTexCo).xyz  : fragDiffuseColor ;
	vec3 spec = uHasSpecularMap == 1 ? texture(uSpecularMap, fragTexCo).xxx  : fragSpecularColor ;

	// normal
	
	vec3 rfNormal = normalize( uHasNormalMap == 1 ? TBN * ( texture(uNormalMap, fragTexCo).xyz * 2 - 1 ) : vNormal ) ;
	//brightness
	float shini = uHasShininessMap == 1 ? texture(uShininessMap, fragTexCo).x : fragShininess;



	outPos = fragPos;
	outNormal = rfNormal;
	outAmbient = amb;
	outDiffuse = diff;
	outSpecular = vec4(spec, shini);

}
