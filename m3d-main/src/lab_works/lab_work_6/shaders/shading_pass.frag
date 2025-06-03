#version 450

// Texture maps
layout ( binding = 0 ) uniform sampler2D uPosition;
layout ( binding = 1 ) uniform sampler2D uNormal;
layout ( binding = 2 ) uniform sampler2D uAmbient;
layout ( binding = 3 ) uniform sampler2D uDiffuse;
layout ( binding = 4 ) uniform sampler2D uSpecular;


uniform vec3 uLight; 

uniform int uSpecularType;

layout( location = 0 ) out vec4 outColor;

void main()
{

		ivec2 c = ivec2( gl_FragCoord.xy );

		vec3  fragPos	= texelFetch( uPosition, c, 0 ).xyz;
		vec3  vNormal	= texelFetch( uNormal,   c, 0 ).xyz;
		vec3  fragAmbient	= texelFetch( uAmbient,  c, 0 ).xyz;
		vec3  fragDiffuse	= texelFetch( uDiffuse,  c, 0 ).xyz;
		vec3  fragSpecular	= texelFetch( uSpecular, c, 0 ).xyz;
		float fragShininess = texelFetch( uSpecular, c, 0 ).w;

		vec3 fragToLight    = uLight - fragPos  ;
		vec3 fragToLightVec = normalize( fragToLight   );
		vec3 fragToCamVec   = normalize( -fragPos );

		vNormal = dot( vNormal , fragToCamVec ) < 0 ? -vNormal : vNormal;
	
		vec3 amb = fragAmbient;
	
		vec3 diff = fragDiffuse * max( dot( fragToLightVec, vNormal ), 0 );
	
		// specular component
		vec3 spec = fragSpecular;

		// apply the right specular type
		switch ( uSpecularType ) {
		case 0: // None
			spec = vec3(0);
			break;

		case 1: // Phong
			spec *= pow( max( dot( reflect( -fragToLightVec, vNormal ), fragToCamVec ), 0 ), fragShininess );
			break;

		case 2: // Blinn
			vec3 halfwayDirec		=  normalize( fragToLightVec + fragToCamVec ); 
			spec  *= pow( max( dot( normalize( halfwayDirec ), vNormal ), 0 ), fragShininess );
			break;

		}



	outColor = vec4( amb + diff+ spec, 1. );

}
