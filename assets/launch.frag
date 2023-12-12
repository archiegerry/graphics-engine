#version 430

in vec3 v2fColor;
in vec3 v2fNormal;
in vec2 v2fTextureCoords;

layout (location=0) out vec3 oColor;
//layout (location=4) uniform vec3 uSceneAmbient;
layout (location=6) uniform mat4 uInvProjCameraWorld;

layout (location=7) uniform vec3 uBaseColor;

layout (binding=0) uniform sampler2D uTexture;

struct DirectLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirectLight directLight;


vec3 getDirLight(DirectLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	vec3 reflectDir = reflect(-lightDir, normal);

	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

	vec3 ambient = light.ambient * uBaseColor;
	vec3 diffuse = light.diffuse * diff * uBaseColor;
	vec3 specular = light.specular * spec * uBaseColor;

	return (ambient + diffuse + specular);
}

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define NUM_POINT_LIGHTS 3
	uniform PointLight pointLights[NUM_POINT_LIGHTS];

vec3 getPointLight(PointLight light, vec3 normal, vec2 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - gl_FragCoord.xyz);

	vec3 reflectDir = reflect(-lightDir, normal);
	
	// -- Diffuse -- //
	float diff = max(dot(normal, lightDir), 0.0);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

	float distance = length(light.position - gl_FragCoord.xyz);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	vec3 ambient = light.ambient * uBaseColor; //material information should go here
	vec3 diffuse = light.diffuse * diff * uBaseColor;
	vec3 specular = light.specular * spec * uBaseColor;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}


void main()
{
	// vec3 normal = normalize(v2fNormal);
	vec3 normal = normalize(v2fNormal);

	vec4 viewPos4d = uInvProjCameraWorld * gl_FragCoord.xyzw;

	vec3 viewPos = viewPos4d.xyz / viewPos4d.w;

    vec3 viewDir = normalize(viewPos - gl_FragCoord.xyz); // Assuming a window size of 800x600, adjust accordingly

	// directional lighting

	vec3 resultantLighting = getDirLight(directLight, normal, viewDir);

	for(int i = 0; i < NUM_POINT_LIGHTS; i++)
	{
		resultantLighting += getPointLight(pointLights[i], normal, gl_FragCoord.xy, viewDir);
	}

	//oColor = texture(uTexture, v2fTextureCoords).rgb;
	oColor = resultantLighting;
	//fragColor = vec4(resultantLighting, 1.0f);
	//oColor = uBaseColor * v2fColor;

	
	//oColor = texture(uTexture, v2fTextureCoords).rgb * resultantLighting; //* (resultantLighting); //might not work

	//oColor = (uSceneAmbient + nDotL * uLightDiffuse) * v2fColor;
	//oColor = texture(uTexture, v2fTextureCoords).rgb * (uSceneAmbient + nDotL * uLightDiffuse); //might not work

}