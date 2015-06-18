#version 120
uniform float lightPositionX;
uniform float lightPositionZ;
uniform float camPosX;
uniform float camPosZ;
uniform float earthAngle;

//-----------------------------------
varying vec2 texture_coordinate;
uniform sampler2D heightmap;
uniform float heightConstant;
varying vec4 color;
//-----------------------------------
void main()
{
	float eRad = radians(earthAngle);
	float e23 = radians(23.5f);
	
	mat4 translate = mat4(1.0, 0.0, 0.0, 0.0,
						  0.0, 1.0, 0.0, 0.0,
						  0.0, 0.0, 1.0, 0.0,
						  -1.f*lightPositionX, 0.0, -1.f*lightPositionZ, 1.0);
						  

	mat4 rotate23 = mat4(cos(e23), sin(e23), 0.0, 0.0,
						 -1.f*sin(e23), cos(e23), 0.0, 0.0,
						 0.0, 0.0, 1.0, 0.0,
						 0.0, 0.0, 0.0, 1.0);
	
	mat4 rotateEarth = mat4(cos(eRad), 0.0, -1.f*sin(eRad), 0.0,
							0.0, 1.0, 0.0, 0.0,
							sin(eRad), 0.0, cos(eRad), 0.0,
							0.0, 0.0, 0.0, 1.0);
							
	mat4 MVM = translate*rotate23*rotateEarth;
							
	vec4 p = MVM * gl_Vertex;
	
	vec3 lightPosition = vec3(lightPositionX, 0.0, lightPositionZ);
	
	vec3 camPosition = vec3(camPosX, 0.0, camPosZ);

	vec3 lightAmbient = vec3(0.3, 0.3, 0.3);
	vec3 lightDiffuse = vec3(1.0, 1.0, 1.0);
	vec3 lightSpecular = vec3(1.0, 1.0, 1.0);

	vec3 materialAmbient = vec3(0.25, 0.25, 0.25);
	vec3 materialDiffuse = vec3(1.0, 1.0, 1.0);
	vec3 materialSpecular = vec3(1.0, 1.0, 1.0);
	int materialShininess = 100;
		
	vec3 L = normalize(lightPosition - vec3(p));
	vec3 V = normalize(camPosition - vec3(p));
	vec3 H = normalize(L + V);
	vec3 N = vec3(MVM * vec4(gl_Normal, 0)); // provided by the programmer
	N = normalize(N);
	float NdotL = dot(N, L);
	float NdotH = dot(N, H);
	
	vec3 diffuseColor = lightDiffuse * materialDiffuse * max(0, NdotL);
	vec3 ambientColor = lightAmbient * materialAmbient;
	vec3 specularColor = lightSpecular* materialSpecular * pow(max(0, NdotH), materialShininess);   
	color = vec4(diffuseColor + ambientColor + specularColor, 1.0);
	gl_FrontColor = vec4(diffuseColor + ambientColor + specularColor, 1.0); 
	
	//--------------------------------------------------------------------
    texture_coordinate = vec2(gl_MultiTexCoord0);
    
    vec4 c = texture2D(heightmap,texture_coordinate);
    vec4 normal = vec4(gl_Normal,0);
    vec4 pos = normal * heightConstant * c.x;

    gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex + pos);
    //----------------------------------------------------------------------
}
