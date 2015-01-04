//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera Part 2									 //
//																		 //
//		$Description:	Rotate our existing camera around in the world	 //
//																		 //
//***********************************************************************//

#include "GL/glew.h"									// Include the GLEW library to manage OpenGL extensions
#include "../Headers/Main.h"							// Include our main header for the application


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
Building off of our previous camera tutorial that showed how to move a camera along a view
vector, in this tutorial we make it more useful by adding rotations to the camera.  This
means that if the user hits the LEFT/RIGHT or A/D keys the camera will rotate the view in
that direction.  To do this we need to build a rotation matrix from our Yaw and Pitch angles
and use that matrix to give us our newly rotated view vector.  Previously we were just looking
straight down the negative z-axis, but now when we rotate left or right we change our Yaw
value and then need to recalculate the view vector and then build a final view matrix to pass
to our shaders.  In the last tutorial the view matrix was just an inversed translation matrix,
but now we need to combine that with our rotation matrix to produce a final view matrix.  You
will notice we added an extra triangle behind the camera so when you rotate you can see
something behind you as well.

*Rotation matrix*
There important part of this tutorial is the mathematics used to create a rotation matrix 
and then how to use the inverse of that matrix to generate rotated vectors for our view and 
up vector.  To build a rotation matrix we first create an identity matrix and then use the 
glm::rotate() functions to set the rotations for each axis.  For the case of Pitch we pass 
in our pitch and the x-axis:

mat4 rotationMatrix(1.0f);
rotationMatrix = rotate(rotationMatrix, Pitch, vec3(1, 0, 0));

For the case of Yaw, we pass in our Yaw rotation angle and y-axis:

rotationMatrix = rotate(rotationMatrix, Yaw, vec3(0, 1, 0));

Each call to rotate() will keep adding more rotations to the matrix and return the new 
matrix with the applied rotation.  Nothing too difficult to understand, thanks to the 
glm functions.  If you are interested in the actual math used you can look at our legacy 
OpenGL tutorials as we do this manually.

*Calculate view and up vector from rotation matrix*
Now to recap on the abstraction of our view vector from our rotation matrix, we can simply 
take the inverse of our rotation matrix and then multiply that by the default view vector 
looking down the -z-axis and get the rotated vector:

vec4 viewVector = inverse(GetRotationMatrix()) * vec4(0, 0, -1, 1);

The same thing works for the up vector, which in our case since we aren't changing the pitch 
will stay pointing up:

vec4 upVector = inverse(GetRotationMatrix()) * vec4(0, 1, 0, 1);

*Input for rotation*
Since we already had the LEFT/RIGHT and A/D keys mapped from the previous tutorial, we just
needed to add a switch statement for those in our InputManager::KeyPressed() function.  When
those keys are pressed we just call SetYaw() with our rotation speed and time slice, as well
as the direction.  When we turn left we subtract the rotation speed:

Camera->SetYaw(Camera->GetYaw() - Camera->GetRotationSpeed() * TimeManager::Instance().DeltaTime);

And if we turn right we add the rotation speed:

Camera->SetYaw(Camera->GetYaw() + Camera->GetRotationSpeed() * TimeManager::Instance().DeltaTime);

In the next tutorial we will show how to create a first person shooter camera where the view 
is controlled by the mouse.  Later we will also add strafing and a third-person camera view.
*/

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


Model g_Triangle;										// Our class to handle initializing and drawing our triangle


// This is our own main() function which abstracts the required main() function to run this application.
int GLApplication::GLMain()
{
	// This calls our Initialize() function below which creates the window and triangle
	Initialize();

	// This is our main game loop which will run until we close the window or hit Escape.
	GameLoop();

	// Once we hit Escape this will clean up the application's resources.
	Destroy();

	// Return 0 for success
	return 0;
}


// This function initializes the window, the shaders and the triangle vertex data.
void GLApplication::Initialize()
{
	// Make sure the window manager is initialized prior to calling this and creates the OpenGL context
	if ( !WindowManager || WindowManager->Initialize(ScreenWidth, ScreenHeight, "GameTutorials - Camera Part 2", false) != 0 )
	{
		// Quit the application if the window couldn't be created with an OpenGL context
		exit(-1);
	}

	// Tell OpenGL that we want a 3D viewport the same size as the window
	glViewport(0, 0, ScreenWidth, ScreenHeight);
	
	// This tells OpenGL that we want depth testing so it renders the order correctly
	glEnable(GL_DEPTH_TEST);

	// Initialize the three XYZ points of the triangle as their associated Green, Red and Blue
	// colors.  We give an alpha value of 1.0, which gives the colors an opacity of 100%.
	Vertex3 vertices[] =
	{	//    X 	  Y	    Z      	   R	  G	   B	 A
		{ { -0.6f, -0.6f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { 0.0f, 0.6f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.6f, -0.6f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	// Initialize the triangle with the vertex array and give the length of 3.  This will create our
	// Vertex Buffer Object and Vertex Array Object.  
	g_Triangle.Initialize(vertices, 3, "Shaders/Shader.vertex", "Shaders/Shader.fragment");

	// Create the projection matrix from our camera
	//									 FOV		    Aspect Ratio			   Near / Far Planes
	Camera->SetPerspective(glm::radians(60.0f), ScreenWidth / (float)ScreenHeight, 0.5f, 150.f);

	// Below we call our init function for our camera to give it a position, a yaw and a pitch rotation.
	// 0 for a yaw and pitch will put the camera looking directly down the negative z-axis.

	//					  Position	  Yaw	 Pitch
	Camera->PositionCamera(0, 0, 6,		0,		0);

	// We now pass in the camera to have access to the projection and view matrices
	g_Triangle.SetCamera(Camera);
	
	// Set the position of the first triangle to be at the origin
	g_Triangle.SetPosition(vec3(0, 0, 0));
}


// This is our game loop where all the magic happens every frame
void GLApplication::GameLoop()
{
	// Loop until the user hits the Escape key or closes the window
	while ( WindowManager->ProcessInput(true) )
	{
		// Use our Singleton to calculate our framerate every frame, passing true to set FPS in titlebar
		TimeManager::Instance().CalculateFrameRate(true);

		// This clears the screen every frame to black (color can be changed with glClearColor)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
		// Set the position of the first triangle to be at the origin
		g_Triangle.SetPosition(vec3(0, 0, 0));

		// Render the first triangle
		g_Triangle.Render();

		// Move the position to the right and back one
		g_Triangle.SetPosition(vec3(1, 0, -1));

		// Render the second triangle with the new position
		g_Triangle.Render();

		// Move the position to the left and back one
		g_Triangle.SetPosition(vec3(-1, 0, -1));

		// Render the third triangle with a new position
		g_Triangle.Render();



		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Move the position to behind the camera
		g_Triangle.SetPosition(vec3(0, 0, 10));

		// Render the third triangle with a new position
		g_Triangle.Render();

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Swap the buffers to display the final rendered image on screen
		WindowManager->SwapTheBuffers();
	}
}


// This can be used to free all of our resources in the application.
void GLApplication::Destroy()
{
	// Free the vertex buffers and array objects
	g_Triangle.Destroy();

	// If we have a window manager still allocated then destroy and delete it
	if ( WindowManager )
	{
		WindowManager->Destroy();

		delete WindowManager;
		WindowManager = nullptr;
	}
	
	// If we have the camera still, delete it
	if ( Camera )
	{
		delete Camera;
		Camera = nullptr;
	}
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// We added an extra triangle behind the camera so when the user rotates around
// they can see something behind them too.  Nothing else was changed in this
// file since the last tutorial.
//
// © 2000-2015 GameTutorials