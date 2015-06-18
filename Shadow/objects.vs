#version 120
varying vec4 ShadowCoord;

uniform vec3 lightPosition;
uniform vec3 camPosition;
uniform mat4 projection;
uniform mat4 modelview;

uniform vec3 Id;
uniform vec3 Is;
uniform vec3 Iamb;

uniform vec4 kd;
uniform vec4 ka;
uniform vec4 ks;

uniform float specExp;

varying vec4 fragPos;
varying vec3 N;

void main()
{
	//vec4 lightPos = gl_ModelViewMatrix * vec4(lightPosition, 1.0);
	//vec4 eyePos = 	gl_ModelViewMatrix * vec4(camPosition, 1.0);
	vec4 p = gl_ModelViewMatrix * gl_Vertex; // translate to view coordinates
	fragPos = p;
	N = vec3(gl_ModelViewMatrixInverseTranspose * vec4(gl_Normal, 0)); // provided by the programmer
	N = normalize(N);
/*
	vec3 L = normalize(vec3(lightPosition)*gl_NormalMatrix);
	vec3 V = normalize( - vec3(p));
	vec3 H = normalize(L + V);

	float NdotL = dot(N, L);
	float NdotH = dot(N, H);

	vec4 diffuseColor = vec4(Id, 1.0) * kd * max(0, NdotL);
	vec4 ambientColor = vec4(Iamb, 1.0) * ka;
	vec4 specularColor = vec4(Is, 1.0) * ks * pow(max(0, NdotH), specExp);
	totalAmbient = ambientColor;
	totalColor = vec4(diffuseColor + ambientColor + specularColor);
	gl_FrontColor = totalColor;
	*/
	gl_Position=gl_ModelViewProjectionMatrix * gl_Vertex;
	

	mat4 biasMatrix = mat4(	0.5, 0.0, 0.0, 0.0, 
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0
						  );

	ShadowCoord = biasMatrix*projection*modelview*gl_Vertex;

}
