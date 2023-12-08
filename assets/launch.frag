#version 430

uniform vec3 v2fColor;
in vec3 v2fNormal;

layout(location=0) uniform vec3 oColor;
layout (location=2) uniform vec3 uLightDir;
layout (location=3) uniform vec3 uLightDiffuse;
layout (location=4) uniform vec3 uSceneAmbient;
layout (location=5) uniform vec3 uBaseColor;

void main()
{
    vec3 normal = normalize(v2fNormal);

    oColor = texture(uTexture, v2fTextureCoords).rgb * (uSceneAmbient + nDotL * uLightDiffuse);

}