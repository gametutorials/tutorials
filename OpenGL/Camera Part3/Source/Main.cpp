//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera Part 3									 //
//																		 //
//		$Description:	Rotate the camera by the mouse					 //
//																		 //
//***********************************************************************//

#include "GL/glew.h"									// Include the GLEW library to manage OpenGL extensions
#include "../Headers/Main.h"							// Include our main header for the application


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
In this tutorial we added a way for us to rotate our camera by moving the mouse around,
similar to how the camera movements would work in a first person game.  To make things more
interesting we build a tower of quads (3D rectangles) to look at.  There is very little
that we added extra in order to accomplish this, partly because we already have our rotation
and view matrices setup to be calculated from our Yaw and Pitch rotations.  Now we just need
to update those Yaw and Pitch values in order to move the camera around freely.  We also go
over how to manage the mouse with GLFW.  Once again, just a few function calls and we have
a working mouse controlled camera.

*How the mouse rotates the camera*
How a mouse controlled camera works is we check the current mouse x and y offsets every
frame and then take the delta movements from the last frame.  We then add a rotation to
the Yaw (xOffset) and Pitch (yOffset) values.  So if you were to move your mouse to the right
it would increase the mouseX position and then we would need to add that value to the Yaw with
a MouseSpeed setting for sensitivity.  These rotation changes are in radians so we use a
MouseSpeed of 0.001 to multiply by the mouseX and mouseY so it doesn't rotate 180 degrees 
if you move 3 pixels to the right.  We don't need to multiply by a time slice because it's
really how fast you move your mouse that will make the difference, not your CPU speed.

It's really as simple as this:
Yaw += xOffset * MouseSpeed;
Pitch += yOffset * MouseSpeed;

We can also easily cap the angle the camera can look by just checking the pitch or yaw
go above or below a certain angle (in radians).

So there are 2 ways you can get the mouse x and y deltas.  You can force the cursor to
be in the middle of the screen every frame and then subtract the new mouse position from
the center of the screen (or last mouse position).  This would give you a xOffset and
yOffset to work with.  This is what we did in our legacy OpenGL tutorials, but this time
we will do it a little easier where we don't need to store the center or last position,
but just force the cursor position to be at (0, 0) every frame and then we get the
current mouse location and then reset it back to (0,0) after we are done.  This gives
us a perfect delta for the new mouse position.

*GLFW mouse management*
In GLFW, we can hide the mouse cursor, get the current location and then also set its
position.  We will use these functions in this tutorial so that the user isn't seeing
the mouse cursor flicker back and forth every frame in the top left corner of the screen.
The get and set functions will also allow us to force the cursor to (0,0), get the delta
position for that frame, then reset it back to (0, 0).  Here is how we hide the cursor:

glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

We do this in our Initialize() function of our GLFWManager class.  We then want to set
the cursor position to (0, 0) so we can start getting the mouse position deltas:

glfwSetCursorPos(Window, 0, 0);

To get the current mouse position we use this with a couple local double variables:

glfwGetCursorPos(Window, &mouseX, &mouseY);

So every frame in our ProcessInput() function we will first get the cursor position,
send the new mouse offsets to our InputManager->MouseMoved(mouseX, mouseY), then reset
the cursor position back to (0, 0).

*InputManager::MouseMoved()*
We created a new function called MouseMoved() in our InputManager and all it does is
just pass those new mouse positions down to our Camera->SetViewByMouse(mouseX, mouseY).

*Camera::SetViewByMouse()*
We added one more function to our Camera class to handle the rotations by the mouse,
called SetViewByMouse().  This takes in the x and y offsets from the current mouse
position and then as we showed above, adds those to our Yaw and Pitch.

NOTE: We changed our Vertex3 class to use glm:vec3 and glm:vec4 for the position and
color properties.  This makes it easier to work with going forward.

That's it!  Since we already are building our rotation matrix each frame, the Yaw and
Pitch value changes will already be combined into our view matrix.  The next tutorial
will show you how to add strafing to the camera, so LEFT/RIGHT and A/D keys will move
perpendicular to the camera's view like in most any first person game.
*/

Model g_Model;										// Our class to handle initializing and drawing our model

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
	if ( !WindowManager || WindowManager->Initialize(ScreenWidth, ScreenHeight, "GameTutorials - Camera Part 3", false) != 0 )
	{
		// Quit the application if the window couldn't be created with an OpenGL context
		exit(-1);
	}

	// Tell OpenGL that we want a 3D viewport the same size as the window
	glViewport(0, 0, ScreenWidth, ScreenHeight);
	
	// This tells OpenGL that we want depth testing so it renders the order correctly
	glEnable(GL_DEPTH_TEST);


	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Below, instead of drawing triangles we draw 3D rectangles, or quads as they are called.
	// OpenGL has mostly evolved into just using triangles so quads aren't really supported in
	// the future, so to make a quad we need to build it from 2 triangles.

	// Below there is about 20 quads drawn upwards to form a tower type shape.  I slowly make
	// each quad smaller and it makes an interesting effect once it goes negative.

	// Initialize enough vertices to make 20 quads (6 verts * 20 quads = 120 vertices).
	// Notice that we converted our Vertex3 into a struct that uses vec3 and vec4's for
	// the xyz and rgba values.  This makes it easier to initialize and use going forward.
	// We set all vertices to position 0 and color 1 (white)
	Vertex3 vertices[120] = { vec3(0), vec4(1) };
	
	// Loop through 20 quads and build them from the ground to the ceiling.  The i is a float
	// so we can do division with i without having to cast every single one.  It takes 6
	// vertices to make a quad (2 triangles).  These will be lying flat instead of standing up.
	// The i/10 added or subtracted to the x and z positions gives the interesting effect of the
	// quads getting smaller as they go up and then going from smaller to larger again.
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

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
	

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Render the model
		g_Model.Render();

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
// In this file we added a little more interesting objects to look at by building
// a tower of quads that get smaller and then bigger as they reach the top.  This
// is done by drawing 40 triangles to make up 20 quads.  You can move around and
// go look at the tower.  You start inside the bottom so step back until you can
// see it.  Instead of our g_Triangle object we created a g_Model object instead.
// You'll notice that we also use glm::vec3 and glm::vec4 objects instead of an
// array of floats for our Vertex3 properties.
//
// © 2000-2015 GameTutorials