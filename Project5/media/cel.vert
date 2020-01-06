
#version 400 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec4 vColour;
layout( location = 2 ) in vec3 vNormal;
layout (location = 3) in vec2 aTexCoord;

uniform vec3 lightPos;

uniform vec4 ambient;
uniform vec3 dLight;


uniform vec3 sLight;
uniform float sShine;

uniform mat4 mv_matrix;
uniform mat4 p_matrix;

out vec4 fragColour;
out vec2 TexCoord;

out vec3 lightDir;

out vec3 normal;

void
main()
{

// view-space coordinate
    vec4 P = mv_matrix * vec4(vPosition,1.0);
	
	vec3 L = lightPos - P.xyz;
	// calc the view vector
	
	//Normalise
	L = normalize(L);

	gl_Position = p_matrix * P;

	lightDir = L;
	
	normal = vec3(mv_matrix * vec4(vNormal,0.0));

	gl_Position = p_matrix * P;
}
