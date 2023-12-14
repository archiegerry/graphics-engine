#include "mesh_renderer.hpp"
//#include <gl/GL.h>

struct Sprite {
	Vec3f position;
	Vec3f velocity;
	float lifespan;
};

std::vector<Sprite> sprites;
GLuint texture, VBO, VAO;
int maxSprites = 6000;


void loadTexture() { 
	texture = load_texture_2d("assets/fire.jpg"); 
	glGetError();
}

void setupSpriteBuffers() {
	// Create VAO and VBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, maxSprites * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	// Configure vertex attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind VBO and VAO 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
}

Vec3f randomConicalDirection(Vec3f direction) {
		using namespace std;
		random_device rd;
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0, 1); 

		float theta = 2 * kPi_ * dis(gen);
		float phi = acos(1 - dis(gen) * (1 - cos(45)));

		Vec3f randomDirection;
		randomDirection.x = sin(phi) * cos(theta);
		randomDirection.y = sin(phi) * sin(theta);
		randomDirection.z = cos(phi);

		return randomDirection;
}

void updateSpritePositions(const std::vector<Sprite>& sprites ) {
	std::vector<float> positions;
	positions.reserve(sprites.size() * 3); // Each sprite has 3 float values for position

	for (const auto& sprite : sprites) {
		positions.push_back(sprite.position.x);
		positions.push_back(sprite.position.y);
		positions.push_back(sprite.position.z);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_DRAW); 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void generateSprites(Vec3f spaceshipPosition, int spriteAmount, Vec3f direction) {
	for (int i = 0; i < spriteAmount; i++)
	{
		Sprite sprite;
		// Position
		sprite.position = spaceshipPosition;
		// Direction
		sprite.velocity = randomConicalDirection(direction);
		// How long it lasts
		sprite.lifespan = 0.5f;
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
		[](const Sprite& sprite) { return sprite.lifespan <= 0; }), sprites.end());
}

void renderSprites(Mat44f project2World, GLuint shader) {

	// Sprite renderer 
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f); 
		//glClear(GL_COLOR_BUFFER_BIT); 
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, texture);  
	glUseProgram(shader);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glUniformMatrix4fv(
		0,
		1, GL_TRUE,
		project2World.v);
	glUniform1i(1, 0); 

	// Bind VAO and draw 
	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, sprites.size()); // Draw one point  
	glBindVertexArray(0); 
	glDisable(GL_BLEND);
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
