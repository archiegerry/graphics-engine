#include "main.hpp"
#include "mesh_renderer.hpp"

struct DirectLight
{
	Vec3f direction;
	Vec3f ambient;
	Vec3f diffuse;
	Vec3f specular;
};

struct PointLight
{
	Vec3f position;
	Vec3f ambient;
	Vec3f diffuse;
	Vec3f specular;
	float constant;
	float linear;
	float quadratic;
};

#define NUM_POINT_LIGHTS 3

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
	)
	{
		glUseProgram(programID);

		DirectLight directLight = {};
		directLight.direction = Vec3f{ 0.5f,-0.5f,0.8f };
		directLight.ambient = Vec3f{ 0.25f, 0.25f, 0.25f };
		directLight.diffuse = Vec3f{ 0.34f, 0.35f, 0.35f };
		directLight.specular = Vec3f{ 0.45f, 0.35f, 0.35f };

		PointLight pointLight[NUM_POINT_LIGHTS]{};
		pointLight[0].position = Vec3f{ relativeto(0,3),relativeto(1,3),relativeto(2,3) } + Vec3f{ -0.5f, 0.5f, -0.5f } + Vec3f{ 0.f, -0.975f, -50.f };
		pointLight[0].ambient = Vec3f{ 0.01f, 0.01f, 0.9f };
		pointLight[0].diffuse = Vec3f{ 0.05f, 0.02f, 0.8f };
		pointLight[0].specular = Vec3f{ 0.5f, 0.5f, 0.5f };
		pointLight[0].constant = 1.0f;
		pointLight[0].linear = 5.7f;
		pointLight[0].quadratic = 4.8f;

		pointLight[1].position = Vec3f{ relativeto(0,3),relativeto(1,3),relativeto(2,3) } + Vec3f{ 0.5f, 0.5f, -0.5f } + Vec3f{ 0.f, -0.975f, -50.f };
		pointLight[1].ambient = Vec3f{ 0.1f, 0.9f, 0.1f };
		pointLight[1].diffuse = Vec3f{ 0.2f, 0.8f, 0.1f };
		pointLight[1].specular = Vec3f{ 0.5f, 0.5f, 0.5f };
		pointLight[1].constant = 1.0f;
		pointLight[1].linear = 0.07f;
		pointLight[1].quadratic = 0.08f;

		pointLight[2].position = Vec3f{ relativeto(0,3),relativeto(1,3),relativeto(2,3) } + Vec3f{ -0.5f, 0.5f, 0.5f } + Vec3f{ 0.f, -0.975f, -50.f };
		pointLight[2].ambient = Vec3f{ 0.9f, 0.1f, 0.1f };
		pointLight[2].diffuse = Vec3f{ 0.9f, 0.2f, 0.1f };
		pointLight[2].specular = Vec3f{ 0.5f, 0.5f, 0.5f };
		pointLight[2].constant = 1.0f;
		pointLight[2].linear = 0.07f;
		pointLight[2].quadratic = 0.08f;


		GLuint directLightLocation = glGetUniformLocation(programID, "uDirectLight.direction");
		glUniform3fv(directLightLocation, 1, &directLight.direction.x);

		directLightLocation = glGetUniformLocation(programID, "uDirectLight.ambient");
		glUniform3fv(directLightLocation, 1, &directLight.ambient.x);

		directLightLocation = glGetUniformLocation(programID, "uDirectLight.diffuse");
		glUniform3fv(directLightLocation, 1, &directLight.diffuse.x);

		directLightLocation = glGetUniformLocation(programID, "uDirectLight.specular");
		glUniform3fv(directLightLocation, 1, &directLight.specular.x);


		for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
			GLuint pointLightLocation = glGetUniformLocation(programID, ("uPointLights[" + std::to_string(i) + "].position").c_str());
			glUniform3fv(pointLightLocation, 1, &pointLight[i].position.x);
		}
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
			GLuint pointLightLocation = glGetUniformLocation(programID, ("uPointLights[" + std::to_string(i) + "].ambient").c_str());
			glUniform3fv(pointLightLocation, 1, &pointLight[i].ambient.x);
		}
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
			GLuint pointLightLocation = glGetUniformLocation(programID, ("uPointLights[" + std::to_string(i) + "].diffuse").c_str());
			glUniform3fv(pointLightLocation, 1, &pointLight[i].diffuse.x);
		}
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
			GLuint pointLightLocation = glGetUniformLocation(programID, ("uPointLights[" + std::to_string(i) + "].specular").c_str());
			glUniform3fv(pointLightLocation, 1, &pointLight[i].specular.x);
		}
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
			GLuint pointLightLocation = glGetUniformLocation(programID, ("uPointLights[" + std::to_string(i) + "].constant").c_str());
			glUniform1f(pointLightLocation, pointLight[i].constant);
		}
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
			GLuint pointLightLocation = glGetUniformLocation(programID, ("uPointLights[" + std::to_string(i) + "].linear").c_str());
			glUniform1f(pointLightLocation, pointLight[i].linear);
		}
		for (int i = 0; i < NUM_POINT_LIGHTS; ++i) {
			GLuint pointLightLocation = glGetUniformLocation(programID, ("uPointLights[" + std::to_string(i) + "].quadratic").c_str());
			glUniform1f(pointLightLocation, pointLight[i].quadratic);
		}


		GLuint baseColorLocation = glGetUniformLocation(programID, "uBaseColor");
		glUniform3f(
			baseColorLocation, 0.7f, 0.7f, 0.7f);

		Mat44f invProjCameraWolrd = invert(projCameraWorld);

		GLuint invProjCameraWorldLocation = glGetUniformLocation(programID, "uInvProjCameraWorld");
		glUniformMatrix4fv(
			invProjCameraWorldLocation,
			1, GL_TRUE,
			invProjCameraWolrd.v);

		// for camera
		glUniformMatrix4fv(
			0,
			1, GL_TRUE,
			projCameraWorld.v);

		glUniformMatrix4fv(
			2,
			1, GL_TRUE,
			model2World.v
		);

		//for normals
		glUniformMatrix3fv(
			1,
			1, GL_TRUE,
			normalMatrix.v);

		//Vec3f lightDir = normalize(Vec3f{ 0.f, 1.f, -1.f });

		//glUniform3fv(2, 1, &lightDir.x);      // Ambient
		//glUniform3f(3, 0.9f, 0.9f, 0.9f);	  // Diffusion
		//glUniform3f(4, 0.05f, 0.05f, 0.05f);  // Spectral

		glBindVertexArray(vao);
		if (textureObjectId != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureObjectId);
		}
		else
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}

}