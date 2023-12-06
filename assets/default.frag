#version 430

in vec3 v2fColor;
in vec2 v2fTextureCoords;

layout (location=5) uniform vec3 uBaseColor;
layout (location=0) out vec3 oColor;
layout (binding=0) uniform sampler2D uTexture;

void main()
{
	oColor = texture(uTexture, v2fTextureCoords).rgb;
}