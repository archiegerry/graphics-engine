#include "mesh_renderer.hpp"

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
		SimpleMeshData coneLeft = make_cone(false, size_t(64), pink1, make_translation({ 0.f, 2.5f, 0.f }));
		SimpleMeshData coneRight = make_cone(false, size_t(64), pink1, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_z(angleToRadians(180)));
		SimpleMeshData body = concatenate(coneLeft, coneRight);

		// Legs
		SimpleMeshData legOne = make_cylinder(true, size_t(64), pink, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_z(angleToRadians(-45)) * make_scaling(2.f, 0.1f, 0.1f));
		SimpleMeshData interimOne = concatenate(body, legOne);
		SimpleMeshData legTwo = make_cylinder(true, size_t(64), pink, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_z(angleToRadians(-135)) * make_scaling(2.f, 0.1f, 0.1f));
		SimpleMeshData interimTwo = concatenate(interimOne, legTwo);
		SimpleMeshData legThree = make_cylinder(true, size_t(64), pink, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_y(angleToRadians(90)) * make_rotation_z(angleToRadians(-45)) * make_scaling(2.f, 0.1f, 0.1f));
		SimpleMeshData interimThree = concatenate(interimTwo, legThree);
		SimpleMeshData legFour = make_cylinder(true, size_t(64), pink, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_y(angleToRadians(-90)) * make_rotation_z(angleToRadians(-45)) * make_scaling(2.f, 0.1f, 0.1f));
		SimpleMeshData interimFour = concatenate(interimThree, legFour);

		// Feet
		SimpleMeshData footOne = make_cube(orange1, make_translation({ -1.3f, 1.f, 0.0f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimFive = concatenate(interimFour, footOne);
		SimpleMeshData footTwo = make_cube(orange1, make_translation({ 1.3f, 1.f, 0.0f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimSix = concatenate(interimFive, footTwo);
		SimpleMeshData footThree = make_cube(orange1, make_translation({ 0.f, 1.f, -1.3f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimSeven = concatenate(interimSix, footThree);
		SimpleMeshData footFour = make_cube(orange1, make_translation({ 0.0f, 1.f, 1.3f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimEight = concatenate(interimSeven, footFour);

		// Middle bars and other shiz
		SimpleMeshData connectorOne = make_cylinder(false, size_t(64), orange, make_translation({ -0.f, 1.f, 1.2f }) * make_rotation_y(angleToRadians(90)) * make_scaling(2.4f, 0.1f, 0.1f));
		SimpleMeshData interimNine = concatenate(interimEight, connectorOne);
		SimpleMeshData connectorTwo = make_cylinder(false, size_t(64), orange, make_translation({ -1.2f, 1.f, 0.f }) * make_scaling(2.4f, 0.1f, 0.1f));
		SimpleMeshData spaceship = concatenate(interimNine, connectorTwo);
		//		SimpleMeshData engine = make_cone(true, size_t(64), Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.f, 0.55f, 0.f }) * make_rotation_z(angleToRadians(90)) * make_scaling(0.5f, 0.5f, 0.5f));
			//	SimpleMeshData spaceship = concatenate(interimTen, engine);

				// Ickle lickle space ship (so cute!)
		for (int vertices = 0; vertices < spaceship.positions.size(); vertices++) {
			spaceship.positions[vertices] *= 0.18;
		}

		return spaceship;
	}
