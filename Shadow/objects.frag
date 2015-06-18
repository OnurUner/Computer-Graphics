#version 120

varying vec4 ShadowCoord;

uniform sampler2D depth;
uniform mat4 depthMVP;
uniform float isLight;

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
	if(isLight == 1.0){
		gl_FragColor = vec4(0.8, 0.8, 0.0, 1.0);	
	}
	else{
		vec3 L = normalize(vec3(lightPosition) - vec3(fragPos));
		vec3 V = normalize( - vec3(fragPos));
		vec3 H = normalize(L + V);

		float NdotL = dot(N, L);
		float NdotH = dot(N, H);

		vec4 diffuseColor = vec4(Id, 1.0) * kd * max(0, NdotL);
		vec4 ambientColor = vec4(Iamb, 1.0) * ka;
		vec4 specularColor = vec4(Is, 1.0) * ks * pow(max(0, NdotH), specExp);
		vec4 totalColor = vec4(diffuseColor + ambientColor + specularColor);
		
		vec4 finalcolor = totalColor;
	
		vec4 shadowMapCoordinates = ShadowCoord / ShadowCoord.w;
		float bias = 0.03;

		float cx = texture2D(depth, shadowMapCoordinates.xy).x;
	
		if(cx < shadowMapCoordinates.z-bias){
			finalcolor = ambientColor;
		}

		if(shadowMapCoordinates.x < 0 || shadowMapCoordinates.x > 1 || shadowMapCoordinates.y < 0 || shadowMapCoordinates.y > 1)
		{
			finalcolor = totalColor;
		}

	
		gl_FragColor = finalcolor;
	}
	
}

