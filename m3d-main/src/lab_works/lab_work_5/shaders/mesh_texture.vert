#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitagent;

uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat4 uModelMtarix;
uniform mat4 uNormalMatrix;
uniform mat4 uMVMatrix;  //           View * Model
uniform mat4 uViewMatrix;   //				 View


uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform float uShininess;
uniform int uHasNormalMap;

uniform vec3 LightPos;
uniform int ufblin;

out vec2 cordTex;

out vec3 vNormal;
out vec3 fragPos;
out vec3 lightPos;

out flat int fragBlin;


void main()
{
	
	cordTex = aVertexTexCoords;
	fragBlin = ufblin;
	vec3 N = normalize( vec3( uNormalMatrix * vec4( normalize( aVertexNormal   ), 1.) ) );
	vec3 T = normalize( vec3( uNormalMatrix * vec4( normalize( aVertexTangent  ), 1.) ) );
	T = normalize ( T - dot (T , N ) * N ); 
	vec3 B = cross (N , T );

	mat3 inver_TBN = transpose( mat3(T, B, N) );
	
	if ( uHasNormalMap == 1 ) {
		fragPos = inver_TBN * vec3( uMVMatrix * vec4( aVertexPosition, 1. ) );
		lightPos	 = inver_TBN * vec3( uViewMatrix  * vec4( LightPos,  1. ) );
	} else {
		fragPos = vec3( uMVMatrix * vec4( aVertexPosition, 1.) );
		lightPos	 = vec3( uViewMatrix *  vec4( LightPos,  1.) );
	}
	vNormal	 = N;

	gl_Position  = uMVPMatrix * vec4( aVertexPosition, 1.f );


}
