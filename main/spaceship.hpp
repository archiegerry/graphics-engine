#include "mesh_renderer.hpp"
#include <gl/GL.h>

struct Sprite {
	Vec3f position;
	Vec3f velocity;
	float lifespan;
};

std::vector<Sprite> sprites;
GLuint texture, VBO, VAO;
int maxSprites = 6000;


void loadTexture() { 
	texture = load_texture_2d("assets/white.png"); 
}

void setupSpriteBuffers() {
	// Create VAO and VBO
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	float vertices[] = { 0.0f, 0.0f, 0.0f }; // Single point at the center 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// Configure vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind VBO and VAO 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
}

void updateSpritePositions(const std::vector<Sprite>& sprites ) {

}

void generateSprites(Vec3f spaceshipPosition, int spriteAmount) {
	for (int i = 0; i < spriteAmount; i++)
	{
		Sprite sprite;
		// Position
		sprite.position = spaceshipPosition;
		// Direction
		sprite.velocity = Vec3f{ 0.2f, 0.1f, 0.3f };
		// How long it lasts
		sprite.lifespan = 10.f;
		sprites.emplace_back(sprite);

	}
}

void updateSprites( float dt ) {
	// Update position and TTL
	for (auto& sprite : sprites) {
		sprite.position += sprite.velocity * dt;
		sprite.lifespan -= dt;
	}

	// Remove dead sprites
	sprites.erase(std::remove_if(sprites.begin(), sprites.end(),
		[](const Sprite& s) { return s.lifespan <= 0; }), sprites.end());
}

void renderSprites( Mat44f projCameraWorld, GLuint programID ) {

	
}

inline SimpleMeshData spaceship() {

		Vec3f color = { 0.35f, 0.35f, 0.3f };

		//rgb for pink : 255, 131, 188 (legs)
		Vec3f pink = { 1.f, 0.514f, 0.737f };
		//rgb for pink1 : 204, 0, 103 (body)
		Vec3f pink1 = { 0.8f, 0.f, 0.404f };
		//rgb for orange : 253, 130, 47 (base)
		Vec3f orange = { 0.992f, 0.510f, 0.184f };
		//rgb for orange1 : 249, 75, 0 (cubes)
		Vec3f orange1 = { 0.976f, 0.294f, 0.f };

		//Body
		SimpleMeshData coneLeft = make_cone(true, size_t(64), Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.05f, 4.9f, 0.f }) * make_scaling(0.6f, 1.3f, 1.6f));
		SimpleMeshData coneRight = make_cone(true, size_t(64), Vec3f{ 1.f, 1.f, 1.f }, make_translation({ -0.05f, 4.9f, 0.f }) * make_scaling(0.6f, 1.3f, 1.6f) * make_rotation_z(angleToRadians(180)));
		SimpleMeshData body = concatenate(coneLeft, coneRight);
		SimpleMeshData coneLeft2 = make_cone(true, size_t(64), Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.0f, 7.5f, 0.f }) * make_scaling(0.6f * 2, 1.3f * 1.2f, 1.6f * 1.5));
		SimpleMeshData body2 = concatenate(body, coneLeft2);
		SimpleMeshData coneRight2 = make_cone(true, size_t(64), Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.0f, 7.5f, 0.f }) * make_scaling(0.6f * 2, 1.3f * 1.3f, 1.6f * 1.5) * make_rotation_z(angleToRadians(180)));
		SimpleMeshData body3 = concatenate(body2, coneRight2);

		// Legs
		SimpleMeshData legOne = make_cylinder(true, size_t(32), color, make_translation({ 0.f, 4.5f, 0.f }) * make_rotation_z(angleToRadians(-65)) * make_scaling(4.8f, 0.1f, 0.2f));
		SimpleMeshData interimOne = concatenate(body3, legOne);
		SimpleMeshData legTwo = make_cylinder(true, size_t(32), color, make_translation({ 0.f, 4.5f, 0.f }) * make_rotation_z(angleToRadians(65 + 180)) * make_scaling(4.8f, 0.1f, 0.2f));
		SimpleMeshData interimTwo = concatenate(interimOne, legTwo);
		SimpleMeshData legThree = make_cylinder(true, size_t(32), color, make_translation({ 0.f, 4.5f, 0.f }) * make_rotation_y(angleToRadians(90)) * make_rotation_z(angleToRadians(-45 - 20)) * make_scaling(4.8f, 0.1f, 0.2f));
		SimpleMeshData interimThree = concatenate(interimTwo, legThree);
		SimpleMeshData legFour = make_cylinder(true, size_t(32), color, make_translation({ 0.f, 4.5f, 0.f }) * make_rotation_y(angleToRadians(-90)) * make_rotation_z(angleToRadians(-45 - 20)) * make_scaling(4.8f, 0.1f, 0.2f));
		SimpleMeshData interimFour = concatenate(interimThree, legFour);

		// Feet
		SimpleMeshData footOne = make_cube(Vec3f{ 1.f, 1.f, 1.f }, make_translation({ -1.3f, 1.5f, 0.0f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimFive = concatenate(interimFour, footOne);
		SimpleMeshData footTwo = make_cube(Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 1.3f, 1.5f, 0.0f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimSix = concatenate(interimFive, footTwo);
		SimpleMeshData footThree = make_cube(Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.f, 1.5f, -1.3f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimSeven = concatenate(interimSix, footThree);
		SimpleMeshData footFour = make_cube(Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.0f, 1.5f, 1.3f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimEight = concatenate(interimSeven, footFour);

		// Middle bars and other shiz
		SimpleMeshData connectorOne = make_cylinder(false, size_t(64), color, make_translation({ -0.f, 1.5f, 1.2f }) * make_rotation_y(angleToRadians(90)) * make_scaling(2.4f, 0.1f, 0.1f));
		SimpleMeshData interimNine = concatenate(interimEight, connectorOne);
		SimpleMeshData connectorTwo = make_cylinder(false, size_t(64), color, make_translation({ -1.2f, 1.5f, 0.f }) * make_scaling(2.4f, 0.1f, 0.1f));
		SimpleMeshData spaceship = concatenate(interimNine, connectorTwo);
		//		SimpleMeshData engine = make_cone(true, size_t(64), Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.f, 0.55f, 0.f }) * make_rotation_z(angleToRadians(90)) * make_scaling(0.5f, 0.5f, 0.5f));
			//	SimpleMeshData spaceship = concatenate(interimTen, engine);
		
				// Ickle lickle space ship (so cute!)
		for (int vertices = 0; vertices < spaceship.positions.size(); vertices++) {
			spaceship.positions[vertices] *= 0.18;
		}

		return spaceship;
}
