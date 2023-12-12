#include <glad.h>
#include <GLFW/glfw3.h>

#include <typeinfo>
#include <stdexcept>

#include <cstdio>
#include <cstdlib>

#include "../support/error.hpp"
#include "../support/program.hpp"
#include "../support/checkpoint.hpp"
#include "../support/debug_output.hpp"

#include "../vmlib/vec4.hpp"
#include "../vmlib/mat44.hpp"
#include "../vmlib/mat33.hpp"

#include "defaults.hpp"

#include "cylinder.hpp"
#include "cone.hpp"
#include "loadobj.hpp"
#include "simple_mesh.hpp"
#include "loadcustom.hpp"

#include "cube.hpp"
#include "texture.hpp"

#include "fontstash.h"

namespace
{
	constexpr char const* kWindowTitle = "COMP3811 - CW2";

	constexpr float kPi_ = 3.1415926f;

	float kMovementPerSecond_ = 5.f; // units per second
	float kMouseSensitivity_ = 0.01f; // radians per pixel
	struct State_ //struct for camera control
	{
		ShaderProgram* prog;

		struct CamCtrl_
		{
			bool cameraActive;
			bool actionZoomIn, actionZoomOut;
			bool actionZoomleft, actionZoomRight;
			bool actionMoveForward, actionMoveBackward;
			bool actionMoveLeft, actionMoveRight;
			bool actionMoveUp, actionMoveDown;

			float phi, theta;
			float radius;
			Vec3f movementVec;

			float lastX, lastY;
		} camControl;

		// Spaceship stuff
		bool moveUp = false;
		float spaceshipOrigin = 0.f;
		float spaceshipCurve = 0.f;
		float acceleration = 0.1f;
		float curve = 0.01f;
	};

	void glfw_callback_error_( int, char const* );
	void glfw_callback_key_( GLFWwindow*, int, int, int, int );

	void glfw_callback_motion_(GLFWwindow*, double, double); //function for mouse motion

	struct GLFWCleanupHelper
	{
		~GLFWCleanupHelper();
	};
	struct GLFWWindowDeleter
	{
		~GLFWWindowDeleter();
		GLFWwindow* window;
	};


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

}
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
		directLight.direction =  Vec3f{ -1.f,-05.f,0.8f };
		directLight.ambient = Vec3f{ 0.25f, 0.25f, 0.25f };
		directLight.diffuse = Vec3f{ 0.26f, 0.26f, 0.26f };
		directLight.specular = Vec3f{ 0.5f, 0.5f, 0.5f };

		PointLight pointLight[NUM_POINT_LIGHTS]{};
		pointLight[0].position = V4toV3( relativeto * Vec4f{ -0.5f, -0.5f, -0.5f , 0.f}) + Vec3f{ 0.f, -0.975f, -50.f };
		pointLight[0].ambient =  Vec3f{ 0.01f, 0.01f, 0.9f };
		pointLight[0].diffuse = Vec3f{ 0.05f, 0.02f, 0.8f };
		pointLight[0].specular = Vec3f{ 0.5f, 0.5f, 0.5f };
		pointLight[0].constant = 1.0f;
		pointLight[0].linear = 0.08f;
		pointLight[0].quadratic = 0.05f;

		pointLight[1].position = V4toV3(relativeto * Vec4f{ -0.5f, -0.5f, 0.5f , 0.f }) + Vec3f{ 0.f, -0.975f, -50.f };
		pointLight[1].ambient = Vec3f{ 0.1f, 0.9f, 0.1f };
		pointLight[1].diffuse = Vec3f{ 0.2f, 0.8f, 0.1f };
		pointLight[1].specular = Vec3f{ 0.5f, 0.5f, 0.5f };
		pointLight[1].constant = 1.0f;
		pointLight[1].linear = 0.08f;
		pointLight[1].quadratic = 0.032f;

		pointLight[2].position = V4toV3(relativeto * Vec4f{ 0.5f, -0.5f, 0.5f , 0.f }) + Vec3f{ 0.f, -0.975f, -50.f };
		pointLight[2].ambient = Vec3f{ 0.9f, 0.1f, 0.1f };
		pointLight[2].diffuse = Vec3f{ 0.9f, 0.2f, 0.1f };
		pointLight[2].specular = Vec3f{ 0.5f, 0.5f, 0.5f };
		pointLight[2].constant = 1.0f;
		pointLight[2].linear = 0.8f;
		pointLight[2].quadratic = 0.032f;


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

namespace
{ 
	SimpleMeshData spaceship() {

		Vec3f color = { 0.35f, 0.35f, 0.3f };

		//Body
		SimpleMeshData coneLeft = make_cone(false, size_t(64), Vec3f{ 1.f, 1.f, 1.f }, make_translation({0.f, 2.5f, 0.f})); 
		SimpleMeshData coneRight = make_cone(false, size_t(64), Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_z(angleToRadians(180))); 
		SimpleMeshData body = concatenate(coneLeft, coneRight); 
		
		// Legs
		SimpleMeshData legOne = make_cylinder(true, size_t(64), color, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_z(angleToRadians(-45)) * make_scaling(2.f, 0.1f, 0.1f));
		SimpleMeshData interimOne = concatenate(body, legOne);
		SimpleMeshData legTwo = make_cylinder(true, size_t(64), color, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_z(angleToRadians(-135)) * make_scaling(2.f, 0.1f, 0.1f));
		SimpleMeshData interimTwo = concatenate(interimOne, legTwo); 
		SimpleMeshData legThree = make_cylinder(true, size_t(64), color, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_y(angleToRadians(90)) * make_rotation_z(angleToRadians(-45)) * make_scaling(2.f, 0.1f, 0.1f));
		SimpleMeshData interimThree = concatenate(interimTwo, legThree);
		SimpleMeshData legFour = make_cylinder(true, size_t(64), color, make_translation({ 0.f, 2.5f, 0.f }) * make_rotation_y(angleToRadians(-90)) * make_rotation_z(angleToRadians(-45)) * make_scaling(2.f, 0.1f, 0.1f));
		SimpleMeshData interimFour = concatenate(interimThree, legFour);
		
		// Feet
		SimpleMeshData footOne = make_cube(Vec3f{ 1.f, 1.f, 1.f }, make_translation({-1.3f, 1.f, 0.0f}) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimFive = concatenate(interimFour, footOne);
		SimpleMeshData footTwo = make_cube(Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 1.3f, 1.f, 0.0f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimSix = concatenate(interimFive, footTwo);
		SimpleMeshData footThree = make_cube(Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.f, 1.f, -1.3f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimSeven = concatenate(interimSix, footThree);
		SimpleMeshData footFour = make_cube(Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.0f, 1.f, 1.3f }) * make_scaling(0.4f, 0.4f, 0.4f));
		SimpleMeshData interimEight = concatenate(interimSeven, footFour);

		// Middle bars and other shiz
		SimpleMeshData connectorOne = make_cylinder(false, size_t(64), color, make_translation({ -0.f, 1.f, 1.2f }) * make_rotation_y(angleToRadians(90)) * make_scaling(2.4f, 0.1f, 0.1f));
		SimpleMeshData interimNine = concatenate(interimEight, connectorOne);
		SimpleMeshData connectorTwo = make_cylinder(false, size_t(64), color, make_translation({ -1.2f, 1.f, 0.f }) * make_scaling(2.4f, 0.1f, 0.1f));
		SimpleMeshData spaceship = concatenate(interimNine, connectorTwo);
//		SimpleMeshData engine = make_cone(true, size_t(64), Vec3f{ 1.f, 1.f, 1.f }, make_translation({ 0.f, 0.55f, 0.f }) * make_rotation_z(angleToRadians(90)) * make_scaling(0.5f, 0.5f, 0.5f));
	//	SimpleMeshData spaceship = concatenate(interimTen, engine);

		// Ickle lickle space ship (so cute!)
		for (int vertices = 0; vertices < spaceship.positions.size(); vertices++) {
			spaceship.positions[vertices] *= 0.18;
		}

		return spaceship;
	}
}


int main() try
{
	// Initialize GLFW
	if( GLFW_TRUE != glfwInit() )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwInit() failed with '%s' (%d)", msg, ecode );
	}

	// Ensure that we call glfwTerminate() at the end of the program.
	GLFWCleanupHelper cleanupHelper;

	// Configure GLFW and create window
	glfwSetErrorCallback( &glfw_callback_error_ );

	glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );
	glfwWindowHint( GLFW_DOUBLEBUFFER, GLFW_TRUE );

	//glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
	glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE );
	glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

	glfwWindowHint(GLFW_DEPTH_BITS, 24);

#	if !defined(NDEBUG)
	// When building in debug mode, request an OpenGL debug context. This
	// enables additional debugging features. However, this can carry extra
	// overheads. We therefore do not do this for release builds.
	glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE );
#	endif // ~ !NDEBUG

	GLFWwindow* window = glfwCreateWindow(
		1280,
		720,
		kWindowTitle,
		nullptr, nullptr
	);

	if( !window )
	{
		char const* msg = nullptr;
		int ecode = glfwGetError( &msg );
		throw Error( "glfwCreateWindow() failed with '%s' (%d)", msg, ecode );
	}

	GLFWWindowDeleter windowDeleter{ window };

	// Set up event handling
	State_ state{};

	//TODO: Additional event handling setup
	//setting up keyboard event handling
	glfwSetWindowUserPointer(window, &state);
	glfwSetKeyCallback(window, &glfw_callback_key_);
	glfwSetCursorPosCallback(window, &glfw_callback_motion_); //!!need to make this frame-rate independent
	//endofTODO

	glfwSetKeyCallback( window, &glfw_callback_key_ );

	// Set up drawing stuff
	glfwMakeContextCurrent( window );
	glfwSwapInterval( 1 ); // V-Sync is on.

	// Initialize GLAD
	// This will load the OpenGL API. We mustn't make any OpenGL calls before this!
	if( !gladLoadGLLoader( (GLADloadproc)&glfwGetProcAddress ) )
		throw Error( "gladLoaDGLLoader() failed - cannot load GL API!" );

	std::printf( "RENDERER %s\n", glGetString( GL_RENDERER ) );
	std::printf( "VENDOR %s\n", glGetString( GL_VENDOR ) );
	std::printf( "VERSION %s\n", glGetString( GL_VERSION ) );
	std::printf( "SHADING_LANGUAGE_VERSION %s\n", glGetString( GL_SHADING_LANGUAGE_VERSION ) );

	// Ddebug output
#	if !defined(NDEBUG)
	setup_gl_debug_output();
#	endif // ~ !NDEBUG

	// Global GL state
	OGL_CHECKPOINT_ALWAYS();

	// TODO: global GL setup goes here
	glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
	//endofTODO

	OGL_CHECKPOINT_ALWAYS();

	// Get actual framebuffer size.
	// This can be different from the window size, as standard window
	// decorations (title bar, borders, ...) may be included in the window size
	// but not be part of the drawable surface area.
	int iwidth, iheight;
	glfwGetFramebufferSize( window, &iwidth, &iheight );

	glViewport( 0, 0, iwidth, iheight );

	//load shader program
	ShaderProgram prog({
		{ GL_VERTEX_SHADER, "assets/default.vert" },
		{ GL_FRAGMENT_SHADER, "assets/default.frag" }
		});

	state.prog = &prog; //set shader program to state
	state.camControl.radius = 10.f; //set initial radius

	auto last = Clock::now();
	float angle = 0.f;

	auto parlahti = load_wavefront_obj("assets/parlahti.obj");
	GLuint vao = create_vao(parlahti);
	std::size_t vertexCount = parlahti.positions.size();

	GLuint textures = load_texture_2d("assets/L4343A-4k.jpeg");

	//----------------------------------------------------------------
	//load shader program for launchpad
	 ShaderProgram prog2({
	 	{ GL_VERTEX_SHADER, "assets/launch.vert" }, 
	 	{ GL_FRAGMENT_SHADER, "assets/launch.frag" } 
	 	}); 

	 // Load the launchpad
	 auto launch = load_wavefront_obj("assets/landingpad.obj");
	 size_t launchVertexCount = launch.positions.size();
	 std::vector<Vec3f> positions = launch.positions;

	 // Move the 1st launch object
	 for (size_t i = 0; i < launchVertexCount; i++)
	 {
		 launch.positions[i] = launch.positions[i] + Vec3f{ 0.f, -0.975f, -50.f }; 
	 }

	 // Create a VAO for the first launchpad
	 GLuint launch_vao_1 = create_vao(launch);
	 // Return positions back to normal
	 launch.positions = positions;

	 // Move the 1st launch object
	 for (size_t i = 0; i < launchVertexCount; i++)
	 {
		 launch.positions[i] = launch.positions[i] + Vec3f{ -20.f, -0.975f, -15.f }; 
	 }
	 // Create a VAO for the first launchpad
	 GLuint launch_vao_2 = create_vao(launch);

	 // SHIP CREATION SECTION
	//-------------------------------------------------------------------


	 // Create the spaceship
	 auto ship = spaceship();
	 size_t shipVertexCount = ship.positions.size();
	 // Store original coordinates 
	 std::vector<Vec3f> shipPositions = ship.positions;

	 // Move the 1st ship
	 for (size_t i = 0; i < shipVertexCount; i++)
	 {
		 ship.positions[i] = ship.positions[i] + Vec3f{ 0.f, -1.125f, -50.f };
	 }

	 // Create VAO for first ship
	 GLuint ship_one_vao = create_vao(ship);
	 // Return positions back to normal
	 ship.positions = shipPositions;

	 // Move the 2nd ship
	 /*
	 for (size_t i = 0; i < shipVertexCount; i++)
	 {
		 ship.positions[i] = ship.positions[i] + Vec3f{ -20.f, -1.125f, -15.f };
	 }

	 // Create VAO for second ship
	 GLuint ship_two_vao = create_vao(ship);
	 */

	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();

	// Main loop
	while( !glfwWindowShouldClose( window ) )
	{
		// Let GLFW process events
		glfwPollEvents();
		
		// Check if window was resized.
		float fbwidth, fbheight;
		{
			int nwidth, nheight;
			glfwGetFramebufferSize( window, &nwidth, &nheight );

			fbwidth = float(nwidth);
			fbheight = float(nheight);

			if( 0 == nwidth || 0 == nheight )
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize( window, &nwidth, &nheight );
				} while( 0 == nwidth || 0 == nheight );
			}

			glViewport( 0, 0, nwidth, nheight );
		}

		//TODO: update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count(); //difference in time since last frame
		last = now;

		angle += dt * kPi_ * 0.3f;
		if (angle >= 2.f * kPi_)
			angle -= 2.f * kPi_;

		Mat44f model2World = kIdentity44f;

		// Animation acceleration 
		Mat44f spaceship2World;
		if (state.moveUp == true) {
			state.spaceshipOrigin = state.spaceshipOrigin + (state.acceleration * dt);
			state.spaceshipCurve = state.spaceshipCurve + (state.curve * dt);
			state.acceleration = state.acceleration * 1.0015f;
			// We want a noticeable curve, so make it higher than the standard acceleration
			state.spaceshipCurve = state.spaceshipCurve * 1.0025f;
			spaceship2World = model2World * make_translation(Vec3f{ 0.0f, state.spaceshipOrigin, state.spaceshipCurve }) ;
		}
		else {
			spaceship2World = model2World;
		}


		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2World)));

		Mat44f Rx = make_rotation_x(state.camControl.theta);
		Mat44f Ry = make_rotation_y(state.camControl.phi);
		Mat44f T = kIdentity44f;

		//camera movement vector is added to current movement
		if (state.camControl.actionMoveForward)
		{
			state.camControl.movementVec.x -= kMovementPerSecond_* dt * sin(state.camControl.phi);
			state.camControl.movementVec.z += kMovementPerSecond_* dt * cos(state.camControl.phi);
			//state.camControl.movementVec += kMovementPerSecond_ * dt * Vec3f{ 0.f,0.f,1.f };
		}
	    if (state.camControl.actionMoveBackward)
		{
			state.camControl.movementVec.x += kMovementPerSecond_* dt * sin(state.camControl.phi);
			state.camControl.movementVec.z -= kMovementPerSecond_* dt * cos(state.camControl.phi);
			//state.camControl.movementVec -= kMovementPerSecond_ * dt * Vec3f{ 0.f,0.f,1.f };
		}
	    if (state.camControl.actionMoveLeft)
		{
			state.camControl.movementVec.x += kMovementPerSecond_ * dt * cos(state.camControl.phi);
			state.camControl.movementVec.z += kMovementPerSecond_ * dt * sin(state.camControl.phi);
			//state.camControl.movementVec += kMovementPerSecond_ * dt * Vec3f{ 1.f,0.f,0.f };
		}
	    if (state.camControl.actionMoveRight)
		{
			state.camControl.movementVec.x -= kMovementPerSecond_ * dt * cos(state.camControl.phi);
			state.camControl.movementVec.z -= kMovementPerSecond_ * dt * sin(state.camControl.phi);
			//state.camControl.movementVec -= kMovementPerSecond_ * dt * Vec3f{ 1.f,0.f,0.f };
		}
	    if (state.camControl.actionMoveUp)
		{
			state.camControl.movementVec -= kMovementPerSecond_ * dt * Vec3f{ 0.f,1.f,0.f };
		}
	    if (state.camControl.actionMoveDown)
		{
			state.camControl.movementVec += kMovementPerSecond_ * dt * Vec3f{ 0.f,1.f,0.f };
		}


		T = make_translation(state.camControl.movementVec);

		Mat44f world2Camera = Rx * Ry * T;

		//world2Camera = make_translation({ 0.f, 0.f, -50.f });

		Mat44f projection = make_perspective_projection(
			60 * kPi_ / 180.f,			//FOV:60 converted to radians
			fbwidth / float(fbheight), //aspect ratio
			0.1f,					  //near plane
			100.f					 //far plane
		);

		Mat44f projCameraWorld = projection * (world2Camera * model2World);

		Mat44f spaceshipModel2World = projection * (world2Camera * spaceship2World);

		//ENDOF TODO

		// Draw scene
		OGL_CHECKPOINT_DEBUG();

		//TODO: draw frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		// Draw the map
		mesh_renderer(vao, vertexCount,  state, textures, prog.programId(), projCameraWorld, model2World, spaceshipModel2World, normalMatrix);

		// Draw the first launchpad
		mesh_renderer(launch_vao_1, launchVertexCount, state, 0, prog2.programId(), projCameraWorld, model2World, spaceshipModel2World, normalMatrix);

		// Draw the second launchpad
		mesh_renderer(launch_vao_2, launchVertexCount, state, 0, prog2.programId(), projCameraWorld, model2World, spaceshipModel2World, normalMatrix);

		// Draw first ship
		mesh_renderer(ship_one_vao, shipVertexCount, state, 0, prog2.programId(), spaceshipModel2World, model2World, spaceshipModel2World, normalMatrix);

		// Draw second ship
		//mesh_renderer(ship_two_vao, shipVertexCount, state, 0, prog2.programId(), spaceshipModel2World, model2World, normalMatrix);

		glBindVertexArray(0);
		//glBindVertexArray(1);

		glUseProgram(0);
		//glUseProgram(1);
		glDeleteBuffers(1, &vao);
		glDeleteBuffers(1, &launch_vao_1);
		glDeleteBuffers(1, &launch_vao_2);
		glDeleteBuffers(1, &ship_one_vao);

		//ENDOF TODO

		OGL_CHECKPOINT_DEBUG();

		// Display results
		glfwSwapBuffers( window );
	}

	// Cleanup.
	//TODO: additional cleanup
	
	return 0;
}
catch( std::exception const& eErr )
{
	std::fprintf( stderr, "Top-level Exception (%s):\n", typeid(eErr).name() );
	std::fprintf( stderr, "%s\n", eErr.what() );
	std::fprintf( stderr, "Bye.\n" );
	return 1;
}

namespace
{
	void glfw_callback_error_( int aErrNum, char const* aErrDesc )
	{
		std::fprintf( stderr, "GLFW error: %s (%d)\n", aErrDesc, aErrNum );
	}

	void glfw_callback_key_(GLFWwindow* aWindow, int aKey, int, int aAction, int)
	{
		if (GLFW_KEY_ESCAPE == aKey && GLFW_PRESS == aAction)
		{
			glfwSetWindowShouldClose(aWindow, GLFW_TRUE);
			return;
		}

		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			// R-key reloads shaders.
			if (GLFW_KEY_R == aKey && GLFW_PRESS == aAction)
			{
				if (state->prog)
				{
					try
					{
						state->prog->reload();
						std::fprintf(stderr, "Shaders reloaded and recompiled.\n");
					}
					catch (std::exception const& eErr)
					{
						std::fprintf(stderr, "Error when reloading shader:\n");
						std::fprintf(stderr, "%s\n", eErr.what());
						std::fprintf(stderr, "Keeping old shader.\n");
					}
				}
			}

			// TODO SHIFT INCREASES SPEED

			if (GLFW_KEY_LEFT_SHIFT == aKey && GLFW_PRESS == aAction)
				kMovementPerSecond_ *= 2.f;
			else if (GLFW_KEY_LEFT_SHIFT == aKey && GLFW_RELEASE == aAction)
				kMovementPerSecond_ /= 2.f;


			// TODO CTRL DECREASES SPEED
			if (GLFW_KEY_LEFT_CONTROL == aKey && GLFW_PRESS == aAction) 
				kMovementPerSecond_ /= 2.f;
			else if (GLFW_KEY_LEFT_CONTROL == aKey && GLFW_RELEASE == aAction)
				kMovementPerSecond_ *= 2.f;


			// Space toggles camera
			if (GLFW_KEY_SPACE == aKey && GLFW_PRESS == aAction)
			{
				state->camControl.cameraActive = !state->camControl.cameraActive;

				if (state->camControl.cameraActive)
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
				else
					glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}

			// Camera controls if camera is active
			if (state->camControl.cameraActive)
			{
				if (GLFW_KEY_W == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveForward = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveForward = false;
				}
				else if (GLFW_KEY_S == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveBackward = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveBackward = false;
				}
				else if (GLFW_KEY_A == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveLeft = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveLeft = false;
				}
				else if (GLFW_KEY_D == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveRight = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveRight = false;
				}
				else if (GLFW_KEY_Q == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveDown = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveDown = false;
				}
				else if (GLFW_KEY_E == aKey)
				{
					if (GLFW_PRESS == aAction)
						state->camControl.actionMoveUp = true;
					else if (GLFW_RELEASE == aAction)
						state->camControl.actionMoveUp = false;
				}
				else if (GLFW_KEY_RIGHT_SHIFT == aKey)
				{
					if (GLFW_PRESS == aAction)
						kMovementPerSecond_ *= 2.f;
					else if (GLFW_RELEASE == aAction)
						kMovementPerSecond_ /= 2.f;
				}
				else if (GLFW_KEY_RIGHT_CONTROL == aKey)
				{
					if (GLFW_PRESS == aAction)
						kMovementPerSecond_ /= 2.f;
					else if (GLFW_RELEASE == aAction)
						kMovementPerSecond_ *= 2.f;
				}
			}

			// Spaceship animation control
			if (GLFW_KEY_F == aKey) {
				state->moveUp = true;
			} else if (GLFW_KEY_R == aKey) {
				state->moveUp = false;
				state->spaceshipOrigin = 0.f;
				state->spaceshipCurve = 0.f;
				state->curve = 0.005f;
				state->acceleration = 0.1f;
			}
		}
	}

	void glfw_callback_motion_(GLFWwindow* aWindow, double aX, double aY)
	{
		if (auto* state = static_cast<State_*>(glfwGetWindowUserPointer(aWindow)))
		{
			if (state->camControl.cameraActive)
			{
				auto const dx = float(aX - state->camControl.lastX);
				auto const dy = float(aY - state->camControl.lastY);

				state->camControl.phi += dx * kMouseSensitivity_;

				state->camControl.theta += dy * kMouseSensitivity_;
				if (state->camControl.theta > kPi_ / 2.f)
					state->camControl.theta = kPi_ / 2.f;
				else if (state->camControl.theta < -kPi_ / 2.f)
					state->camControl.theta = -kPi_ / 2.f;
			}

			state->camControl.lastX = float(aX);
			state->camControl.lastY = float(aY);
		}
	}

}

namespace
{
	GLFWCleanupHelper::~GLFWCleanupHelper()
	{
		glfwTerminate();
	}

	GLFWWindowDeleter::~GLFWWindowDeleter()
	{
		if( window )
			glfwDestroyWindow( window );
	}
}

//different use program 

