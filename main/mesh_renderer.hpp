#include "main.hpp"



namespace
{
	// Mesh rendering
	void mesh_renderer(
		GLuint vao,
		size_t vertexCount,
		State_ const& state,
		GLuint textureObjectId,
		GLuint programID,
		Mat44f projCameraWorld,
		Mat44f model2World,
		Mat44f relativeto,
		Mat33f normalMatrix
		//Mat44f localTransform
	);
	
}