#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitagent;

out vec3 fragAmbientColor;
out vec3 fragDiffuseColor;
out vec3 fragSpecularColor;
out float fragShininess;

uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat4 uMVMatrix;  //              View * Model
uniform mat4 uVMatrix;   //				 View
uniform mat4 uNormalMatrix;
uniform vec3 uAmbientColor;
uniform vec3 uDiffuseColor;
uniform vec3 uSpecularColor;
uniform float uShininess;
uniform vec3 uLightPos;
uniform int ufblin;
uniform int uHasNormalMap;

out mat3 TBN;
out vec2 fragTexCo;
out vec3 vNormal;
out vec3 fragPos;
out vec3 fragLight;


void main()
{
	
	fragAmbientColor = uAmbientColor;
	fragDiffuseColor = uDiffuseColor;
	fragSpecularColor = uSpecularColor;
	fragShininess = uShininess;

	fragTexCo = aVertexTexCoords;


	vec3 T = normalize( vec3( uNormalMatrix * vec4( normalize( aVertexTangent  ), 1.0) ) );
	vec3 N = normalize( vec3( uNormalMatrix * vec4( normalize( aVertexNormal   ), 1.0) ) );
	
	T = normalize ( T - dot (T , N ) * N ); 
	vec3 B = cross (N , T );

	TBN = mat3(T, B, N);
	
	fragPos = vec3( uMVMatrix * vec4( aVertexPosition, 1.) );

	vNormal	 = N;

	gl_Position  = uMVPMatrix * vec4( aVertexPosition, 1.f );


}
