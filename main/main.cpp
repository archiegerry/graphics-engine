#include "spaceship.hpp"

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
	glEnable(GL_SCISSOR_TEST);

	

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

	//-------------------------------------------------------------------


	// Load the map OBJ file
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


	 // Move the ship
	 for (size_t i = 0; i < shipVertexCount; i++)
	 {
		 ship.positions[i] = ship.positions[i] + Vec3f{ 0.f, -1.125f, -50.f };
	 }

	 // Create VAO for the ship
	 GLuint ship_one_vao = create_vao(ship);

	 Mat44f spaceshipModel2World;

	 //-------------------------------------------------------------------
	 // SHIP CREATION SECTION END

	//-------------------------------------------------------------------
	// POINT SPRITE CREATION
	// loadTexture();
	 //setupSpriteBuffers();
	 ShaderProgram prog3({
			 { GL_VERTEX_SHADER, "assets/points.vert" },
			 { GL_FRAGMENT_SHADER, "assets/points.frag" }
	 });
	 loadTexture();
	 setupSpriteBuffers();

	// POINT SPRITE CREATION END
	//-------------------------------------------------------------------


	// Other initialization & loading
	OGL_CHECKPOINT_ALWAYS();

	// Main loop
	while( !glfwWindowShouldClose( window ) )
	{
		// Let GLFW process events
		glfwPollEvents();

		//generateSprites(Vec3f{ 1.f, 1.f, 1.f }, 10);
		
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

			int halfWidth = nwidth / 2;

			if (state.split == false) {
				glViewport(0, 0, nwidth, nheight);
				glScissor(0, 0, nwidth, nheight);
			}
			else {
				glViewport(0, 0, halfWidth, nheight);
				glScissor(0, 0, halfWidth, nheight);
			}

		}

		//TODO: update state
		auto const now = Clock::now();
		float dt = std::chrono::duration_cast<Secondsf>(now - last).count(); //difference in time since last frame
		last = now;
		 
		angle += dt * kPi_ * 0.3f;
		if (angle >= 2.f * kPi_)
			angle -= 2.f * kPi_;

		Mat44f model2World = make_rotation_y(0);
		Mat44f spaceship2World = model2World;
		if (state.moveUp == true) {
			// Acceleration parameters
			state.spaceshipOrigin += (state.acceleration * dt);

			// How quickly up
			state.acceleration *= 1.0025;

			// Curve once certain height is reached
			if (state.spaceshipOrigin > 0.5f) {
				state.spaceshipCurve += (0.05 * dt);
				state.spaceshipCurve *= 1.005;
			}

			// Align spaceship with travel direction
			float angleX = std::atan2(state.spaceshipCurve, state.spaceshipOrigin);

			// Must translate the shape back to starting point to apply transformations
			Mat44f translationToOrigin = make_translation(Vec3f{ 20.f, 1.125f, 15.f });
			Mat44f xRotationMatrix = make_rotation_x(angleX);
			// Then must be translated back to launchpad
			Mat44f originToTranslation = make_translation(Vec3f{ -20.f, -1.125f, -15.f });
			Mat44f translationMatrix = make_translation(Vec3f{ 0.0f, state.spaceshipOrigin, state.spaceshipCurve });
			spaceship2World = translationMatrix * originToTranslation * xRotationMatrix * translationToOrigin * model2World;
			generateSprites(Vec3f{ 0.f, state.spaceshipOrigin, state.spaceshipCurve } + Vec3f{ -20.208f, -1.f, -15.f }, 10, Vec3f{ 0.f, -state.spaceshipOrigin, -state.spaceshipCurve });
			generateSprites(Vec3f{ 0.f, state.spaceshipOrigin, state.spaceshipCurve } + Vec3f{ -19.792f, -1.f, -15.f }, 10, Vec3f{ 0.f, -state.spaceshipOrigin, -state.spaceshipCurve });
			generateSprites(Vec3f{ 0.f, state.spaceshipOrigin, state.spaceshipCurve } + Vec3f{ -20.f, -1.f, -15.208f }, 10, Vec3f{ 0.f, -state.spaceshipOrigin, -state.spaceshipCurve });
			generateSprites(Vec3f{ 0.f, state.spaceshipOrigin, state.spaceshipCurve } + Vec3f{ -20.f, -1.f, -14.792f }, 10, Vec3f{ 0.f, -state.spaceshipOrigin, -state.spaceshipCurve });
		}


		Mat33f normalMatrix = mat44_to_mat33(transpose(invert(model2World)));

		Mat44f Rx = make_rotation_x(state.camControl.theta);
		Mat44f Ry = make_rotation_y(state.camControl.phi);
		Mat44f T = kIdentity44f;

		if (state.mode == 0)
		{
			//camera movement vector is added to current movement
			if (state.camControl.actionMoveForward)
			{
				state.camControl.movementVec.x -= kMovementPerSecond_ * dt * sin(state.camControl.phi);
				state.camControl.movementVec.z += kMovementPerSecond_ * dt * cos(state.camControl.phi);
				//state.camControl.movementVec += kMovementPerSecond_ * dt * Vec3f{ 0.f,0.f,1.f };
			}
			if (state.camControl.actionMoveBackward)
			{
				state.camControl.movementVec.x += kMovementPerSecond_ * dt * sin(state.camControl.phi);
				state.camControl.movementVec.z -= kMovementPerSecond_ * dt * cos(state.camControl.phi);
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
		}

		T = make_translation(state.camControl.movementVec);

		Mat44f world2Camera = Rx * Ry * T;

		Mat44f projection = make_perspective_projection(
			60 * kPi_ / 180.f,			//FOV:60 converted to radians
			fbwidth / float(fbheight), //aspect ratio
			0.1f,					  //near plane
			100.f					 //far plane
		);

		Mat44f projCameraWorld = projection * (world2Camera * model2World);

		Mat44f spaceshipModel2World = projection * (world2Camera * spaceship2World);

		updateSprites(dt);
		updateSpritePositions(sprites);
		//-------------------------------
		//when mode = 1
		//camera i fixed on the ground and follows it in flight 
		if (state.mode == 1)
		{

			Mat44f defaultRotation = kIdentity44f;

			world2Camera = make_rotation_x(90 * (3.1415926 / 180)) * defaultRotation;

			Mat44f t3 = make_translation(Vec3f{ 0.f,0.f,45.f });

			state.camControl.phi = kPi_ * 0;

			float difZ = (-state.spaceshipOrigin);
			float difY = (-state.spaceshipCurve);

			float phiNew = -atan(difY / 5.f);
			float thetaNew = atan(difZ / 5.f);

			Mat44f Ry = make_rotation_y(phiNew);
			Mat44f Rx = make_rotation_x(thetaNew);

			world2Camera = xRotationMatrix * Ry * t3;

		}
		// -------------------------------
		// mode = 2
		//camera mode: fixed distance and follows in flight 
		if (state.mode == 2)
		{
			Vec3f ship3Vf = (Vec3f{ 0.f,0.f,45.f })+Vec3f{ spaceshipModel2World(0,3), -spaceshipModel2World(1,3), spaceshipModel2World(2,3) };
			
			Vec3f ship3vf2 = Vec3f{ 0.f,0.f,45.f } + Vec3f{ 0.f, -state.spaceshipOrigin, -state.spaceshipCurve };



			//Mat44f defaultRotation = kIdentity44f;

			//world2Camera = defaultRotation * defaultRotation;

			//Mat44f initialTranslation = make_translation(Vec3f{0.f, 0.5f, -45.f});
			//Mat44f t2 = make_translation(Vec3f{ 0.f,0.f,45.f });

			//Mat44f moveCam2Ship = make_translation(ship3Vf);//make_translation(Vec3f{sp(0,3), -sp(1,3), sp(2,3)});
			//make_translation(Vec3f{ 0.f, -state.spaceshipOrigin, -state.spaceshipCurve });

			//state.camControl.phi = kPi_ * 0;

			//Mat44f Rx = make_rotation_x(state.camControl.theta);
			//Mat44f Ry = make_rotation_y(state.camControl.phi);

			//camControl.movementVec.y += (state.acceleration * dt);
			//state.camControl.movementVec.x += (state.curve * dt);


			world2Camera = Rx * Ry * make_translation(ship3vf2);

		}

		projCameraWorld = projection * (world2Camera * model2World);

		spaceshipModel2World = projection * (world2Camera * spaceship2World);

		//Mat44f world2Camera = make_translation({ 0.f, 0.f, -10.f });


		//ENDOF TODO

		// Draw scene
		OGL_CHECKPOINT_DEBUG();

		//TODO: draw frame
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderSprites(projCameraWorld, prog3.programId());

	
		// Draw the map
		mesh_renderer(vao, vertexCount, state, textures, prog.programId(), projCameraWorld, model2World, spaceship2World,
			normalMatrix);

		// Draw the first launchpad
		mesh_renderer(launch_vao_1, launchVertexCount, state, 0, prog2.programId(), projCameraWorld, model2World, spaceship2World,
			normalMatrix);

		// Draw the second launchpad
		mesh_renderer(launch_vao_2, launchVertexCount, state, 0, prog2.programId(), projCameraWorld, model2World, spaceship2World,
			normalMatrix);

		// Draw ship
		mesh_renderer(ship_one_vao, shipVertexCount, state, 0, prog2.programId(), spaceshipModel2World, model2World, spaceship2World,
			normalMatrix);

		
		glBindVertexArray(0);
		//glBindVertexArray(1);

		glUseProgram(0);
		//glUseProgram(1);

		//ENDOF TODO

		OGL_CHECKPOINT_DEBUG();

		// Display results
		//glfwSwapBuffers( window );

	//------------------------------------------------------------------------
	//different controls to toggle between the camera !  shift c
		if (state.split == true)
		{
			//the second screen

				// Check if window was resized.
				//float fbwidth1, fbheight1;
				//{
			int nwidth1, nheight1;
			glfwGetFramebufferSize(window, &nwidth1, &nheight1);

			//fbwidth1 = float(nwidth1);
			//fbheight1 = float(nheight1);

			if (0 == nwidth1 || 0 == nheight1)
			{
				// Window minimized? Pause until it is unminimized.
				// This is a bit of a hack.
				do
				{
					glfwWaitEvents();
					glfwGetFramebufferSize(window, &nwidth1, &nheight1);
				} while (0 == nwidth1 || 0 == nheight1);
			}

			int halfWidth1 = nwidth1 / 2;

			glViewport(halfWidth1, 0, halfWidth1, nheight1);
			glScissor(halfWidth1, 0, halfWidth1, nheight1);

			// Draw scene
			OGL_CHECKPOINT_DEBUG();

			//TODO: draw frame
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Draw the map
			mesh_renderer(vao, vertexCount, state, textures, prog.programId(), projCameraWorld, model2World, spaceship2World,
				normalMatrix);

			// Draw the first launchpad
			mesh_renderer(launch_vao_1, launchVertexCount, state, 0, prog2.programId(), projCameraWorld, model2World, spaceship2World,
				normalMatrix);

			// Draw the second launchpad
			mesh_renderer(launch_vao_2, launchVertexCount, state, 0, prog2.programId(), projCameraWorld, model2World, spaceship2World,
				normalMatrix);

			// Draw ship
			mesh_renderer(ship_one_vao, shipVertexCount, state, 0, prog2.programId(), spaceshipModel2World, model2World, spaceship2World,
				normalMatrix);


			glBindVertexArray(0);
			//glBindVertexArray(1);

			glUseProgram(0);
			//glUseProgram(1);

			//ENDOF TODO

			OGL_CHECKPOINT_DEBUG();

		}

		// Display results
		glfwSwapBuffers( window );
		glfwPollEvents();
	}

	// Cleanup.
	//TODO: additional cleanup
	glDeleteVertexArrays(1, &VAO); 
	glDeleteBuffers(1, &VBO); 
	glDeleteProgram(prog3.programId()); 

	glfwTerminate();
	
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


			//C to switch between camera modes 
			if (GLFW_KEY_C == aKey && GLFW_PRESS == aAction)
			{
				//switch between 3 modes
				state->mode += 1;
				//when it gets to the last mode, switch back to original camera 
				if (state->mode == 3)
				{
					state->mode = 0;
				}
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

			//v toggles split screen mode 
			if (GLFW_KEY_V == aKey && GLFW_PRESS == aAction) {
				state->split = !state->split;
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

