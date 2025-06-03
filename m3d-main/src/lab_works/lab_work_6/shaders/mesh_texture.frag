#version 450

in vec3 fragAmbientColor;
in vec3 fragDiffuseColor;
in vec3 fragSpecularColor;
in float fragShininess;

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

in vec2 fragTexCo;

in vec3 vNormal;   // Normal
in vec3 fragPos; 
in vec3 fragLight;	 

uniform int uSpecularType;

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
	vec3 rfNormal = normalize( uHasNormalMap == 1 ? texture(uNormalMap, fragTexCo).xyz * 2 - 1 : vNormal ) ;

	// parameters
	float shini = uHasShininessMap == 1 ? texture(uShininessMap, fragTexCo).x : fragShininess;
	vec3 fragToLight = fragLight - fragPos ;

	rfNormal = dot( rfNormal , normalize( fragToLight ) ) < 0 ? -rfNormal : rfNormal;
	rfNormal = normalize(rfNormal);
	
	diff *= max( dot( normalize( fragToLight ), rfNormal ), 0 );
	
	// apply the right specular type
	switch ( uSpecularType ) {
	case 0: // None
		spec = vec3(0);
		break;

	case 1: // Phong
		spec *= pow( max( dot( normalize( reflect( -fragToLight, rfNormal ) ), normalize( -fragPos ) ), 0 ), shini );
		break;

	case 2: // Blinn
		vec3 halfwayDirec =  normalize( normalize( fragToLight ) + normalize( -fragPos ) ); 
		spec *= pow( max( dot( normalize( halfwayDirec ), rfNormal ), 0 ), shini );
		break;

	}
		


	outPos = fragPos;
	outNormal = rfNormal;
	outAmbient = amb;
	outDiffuse = diff;
	outSpecular = vec4(spec, shini);

}
