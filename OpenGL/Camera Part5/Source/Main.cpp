//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera Part 5									 //
//																		 //
//		$Description:	Camera Strafing									 //
//																		 //
//***********************************************************************//

#include "GL/glew.h"									// Include the GLEW library to manage OpenGL extensions
#include "../Headers/Main.h"							// Include our main header for the application


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
In this tutorial we added a way for the camera to strafe left and right with the A/D or
LEFT/RIGHT keys.  This completes our camera class for now since you should have most of the
basic things that you need to have decent movement in your 3D scene from a first person 
perspective.  Another thing you might want to add is of course a jump function.  Look for this 
in a future tutorial.

*What is strafing?*
Strafing is when the camera is looking forward and you move perpendicular to your view either
on the right or the left.  This is very popular in first person shooters where you want to be
aiming at something and then be able to dodge left or right from others shooting back at you.
This also makes moving in a 3D scene very fluid and allows for easy views of things as you
navigate around objects in the scene.

*How to code strafing*
How strafing works from a high level is you have a view vector and an up vector for a camera,
and you want to find the vector that is perpendicular (90 degrees) to the right or left of
the camera.  You then want to move the camera along that vector, depending on the strafing
direction that is desired.  There are a couple ways to find that vector, one is if you have
the view and up vector, you can take the cross product (linear algebra) between those two
vectors and it will return the vector that is perpendicular to those 2 vectors.  

rightVector = cross(viewVector, upVector);

In our case, we have a rotation matrix and can easily derive vectors from taking the inverse 
of that matrix and multiplying it by a default right vector of (1, 0, 0), we use vec4 since
the rotation matrix is a 4x4:

vec4 rightVector = inverse(GetRotationMatrix()) * vec4(1, 0, 0, 1);

This essentially allows us to apply rotations to a default right vector and it tell us what
it has been rotated to.  The theory might be a little bit more deep that than, but that is
the best way I can describe it.

So now that we have the right vector, then what do we do with it?  Similar to what we did
in our MoveCamera() function, we just add that vector to our camera's Position, with a
desired speed applied.

vec3 rightVector = GetRight();
Position.x += rightVector.x * speed;
Position.z += rightVector.z * speed;

That's pretty simple, but adds a great deal of fluidity to your camera.  We added back our
A/D and LEFT/RIGHT key checks in KeyPressed, and then similarly with our MoveCamera()
functionality, we pass in the speed applied to a time slice into Camera->Strafe().  Try
and see if you can add a Jump() function to your camera.
*/

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


Model g_Model;										// Our class to handle initializing and drawing our model



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
	if ( !WindowManager || WindowManager->Initialize(ScreenWidth, ScreenHeight, "GameTutorials - Camera Part 5", false) != 0 )
	{
		// Quit the application if the window couldn't be created with an OpenGL context
		exit(-1);
	}

	// Tell OpenGL that we want a 3D viewport the same size as the window
	glViewport(0, 0, ScreenWidth, ScreenHeight);
	
	// This tells OpenGL that we want depth testing so it renders the order correctly
	glEnable(GL_DEPTH_TEST);

	// Initialize enough vertices to make 20 quads (6 verts * 20 quads = 120 vertices)
	Vertex3 vertices[120] = { vec3(0), vec4(1) };
	
	// Loop through 20 quads and build them from the ground to the ceiling
	for ( float i = 0; i < 120; i += 6 )
	{
		// The first half of the quad (triangle 1)

		// The back left vertex -- colored yellow
		vertices[0 + (int)i].xyz = vec3(-5.0f + i / 10, -5.0f + i, -5.0f + i / 10);
		vertices[0 + (int)i].rgba = vec4(1, 1, 0, 1);

		// The back right vertex -- colored red
		vertices[1 + (int)i].xyz = vec3(5.0f - i / 10, -5.0f + i, -5.0f + i / 10);
		vertices[1 + (int)i].rgba = vec4(1, 0, 0, 1);

		// The front right vertex -- colored cyan
		vertices[2 + (int)i].xyz = vec3(5.0f - i / 10, -5.0f + i, 5.0f - i / 10);
		vertices[2 + (int)i].rgba = vec4(0, 1, 1, 1);

		// The second half of the quad (triangle 2)

		// The front right vertex -- colored cyan
		vertices[3 + (int)i].xyz = vec3(5.0f - i / 10, -5.0f + i, 5.0f - i / 10);
		vertices[3 + (int)i].rgba = vec4(0, 1, 1, 1);

		// The front left vertex -- colored blue
		vertices[4 + (int)i].xyz = vec3(-5.0f + i / 10, -5.0f + i, 5.0f - i / 10);
		vertices[4 + (int)i].rgba = vec4(0, 0, 1, 1);

		// The back left vertex -- colored yellow
		vertices[5 + (int)i].xyz = vec3(-5.0f + i / 10, -5.0f + i, -5.0f + i / 10);
		vertices[5 + (int)i].rgba = vec4(1, 1, 0, 1);
	}

	// Initialize the model with the vertex array and give the vertex length of 120
	g_Model.Initialize(vertices, 120, "Shaders/Shader.vertex", "Shaders/Shader.fragment");
		
	// Create the projection matrix from our camera and make the near field closer and the far field farther.
	// This makes it so our tower doesn't get cut off and also doesn't cull geometry right near the camera.
	//									 FOV		    Aspect Ratio			   Near / Far Planes
	Camera->SetPerspective(glm::radians(60.0f), ScreenWidth / (float)ScreenHeight, 0.01f, 1000);

	//					  Position	  Yaw	 Pitch
	Camera->PositionCamera(0, 0, 6,		0,		0);

	// We now pass in the camera to have access to the projection and view matrices
	g_Model.SetCamera(Camera);
	
	// Set the position of the model to be at the origin
	g_Model.SetPosition(vec3(0, 0, 0));
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
	
		// Render the model
		g_Model.Render();

		// Swap the buffers to display the final rendered image on screen
		WindowManager->SwapTheBuffers();
	}
}


// This can be used to free all of our resources in the application.
void GLApplication::Destroy()
{
	// Free the vertex buffers and array objects
	g_Model.Destroy();

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
// In this tutorial we added a little bit of code to allowing camera strafing
// using our new right vector (GetRight()) and Strafe() function.  Nothing new
// was added to this file since the last tutorial.
//
// © 2000-2015 GameTutorials