#version 430

layout (location = 0) uniform mat4 uProjCameraWorld;
layout (location = 0) in vec3 iPosition;
layout (location = 1) in vec3 iColor;
layout (location = 2) in vec2 iTextureCoords;

out vec3 v2fColor;
out vec2 v2fTextureCoords;

void main()
{
	v2fColor = iColor;
	gl_Position = uProjCameraWorld * vec4 (iPosition, 1.0);
	v2fTextureCoords = iTextureCoords;

}
