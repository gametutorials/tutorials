//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Camera											 //
//																		 //
//		$Description:	Moves a camera in 3D space from user input		 //
//																		 //
//***********************************************************************//

#include "GL/glew.h"									// Include the GLEW library to manage OpenGL extensions
#include "../Headers/Main.h"							// Include our main header for the application


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
Now that we have some basics down, let's start a camera tutorial series with that builds from
just showing basic back and forth movement, to rotation, to first-person mouse movement, strafing
and third-person views.  This tutorial will first show how to setup simple movement in 3D by
allowing the user to use the UP/DOWN (or W/S) keys to move the camera forward and backwards.
To do this, we added a Camera class and an InputManager class to abstract the input from the
WindowManager implementation.  

*New camera method*
In our older OpenGL tutorials we handled cameras in a way where we stored a Position, ViewVector 
(where the camera was looking) and UpVector (the up direction). In our newer OpenGL 4 tutorials 
we will be improving upon that method and instead storing the Position, Yaw and Pitch values.  
This method allows us to easier specify rotation values to set where the camera is looking or 
limit the view easier.  The view and up vectors are then figured out from our Position, Yaw and 
Pitch by using the inverse of the rotation matrix that is built from the Yaw and Pitch angles.

*Position*
If you imagine a camera on the set of a TV show or film, the camera has a position in the set
and it also have a direction that the camera is facing.  The camera can move forward and back,
left and right, also known as strafing, similar to a camera being on a dolly track.  The camera
can also rotate left and right (yaw) and up and down (pitch).  In 3D, we store the position
for the camera in a (x,y,z) point (vec3).  This is our Position variable in our Camera class.

*Yaw*
In 3D a camera by default is looking straight down the negative z axis.  So if we had a camera
position of (0, 0, 0), the view would be at (0, 0, -1).  The horizontal and vertical rotation 
are initially set at 0 degrees.  In aviation, the term for horizontal rotation (a plane turning
left or right) is called yaw, and this also became used in 3D terminology.  Assuming you are 
using a coordinate system where the y-axis is pointing up (OpenGL), this is also called rotating 
along the y-axis.  The yaw rotation is what allows a camera to look left and right, without 
moving the position.  In this tutorial we won't be rotating, but will use this in the next one.

*Pitch*
The pitch refers to the rotation along the x-axis, or in the case of our camera it is what
allows the camera to look up and down.  In aviation this is used to refer to the plane tilting
up or down.  In this tutorial we won't be using rotations, but will use this in part 2.

*Roll*
The roll in aviation refers to the plane rotating along the z-axis.  So you've probably heard
of the term when a plane does a "barrel roll" it is when one wing comes up and the other wing
goes down and spins the airplane in a 360 degree roll and brings it back to normal again. In
3D it would be the same thing if our camera were to spin around the z-axis.  We don't use this
type of rotation much in camera views for video games as much as the yaw and pitch, so we won't
be focusing on this type of rotation.

*Moving the camera*
So if we have a position, how do we move the camera?  We do this by first listening for the user's
input on the keyboard, in our case the UP/DOWN arrow keys or the W/S keys.  We then increase or
decrease the x and z position of the Position along the current direction the user is looking, in
the case of this tutorial, just hard coded to look down the negative z axis.  In our legacy OpenGL
tutorials we would store the view vector and then use vector math to move the position in the
direction of that view.  In the new method, we don't store a view vector but just use angles of
rotations (in radians format) to tell our camera where it is looking at, and so we need to
calculate the view vector from our yaw and pitch angles.  Since we aren't doing any rotations
in this first tutorial, we leave the yaw and pitch set to 0 which has the camera looking
down the negative z-axis with a direction of vec3(0, 0, -1).  So until we start using rotations
in the next tutorial we just hard code our view vector in GetView().  The next tutorials will 
create a rotation matrix and use the inverse of that matrix to determine our current view vector.

Notice that I didn't mention changing the y value of our Position.  This is because we want to
keep the camera on the ground, otherwise if the user looked up and you also moved the y position
it would have the camera start flying into the air like they were flying an airplane.  You can
add that if your game depends needs it, but in our case we will just keep the camera on the floor.

The math to move a position along a vector is shown below from the Camera's MoveCamera():

// Get our normalized view vector (The direction we are facing)
vec3 viewVector = GetView();

// Move our camera position forward or backward along the view vector
Position.x += viewVector.x * speed;		// Add our acceleration to our position's X
Position.z += viewVector.z * speed;		// Add our acceleration to our position's Z

We first get the view vector which is the point the camera is looking, which in this tutorials
is hard coded to vec3(0, 0, -1) since we have no way to rotate it yet.  We call a GetView() to 
get our view vector. This is because we don't store a view vector but need to calculate it.

*InputManager*
Since each API for our WindowManager has a different way to process input such as GLFW, Win32,
SDL, GLUT, etc., we want to abstract the input away from the WindowManager and pass in a standard
set of input codes into an InputManager which knows nothing about the platform or API being used.
In this tutorial we use the KeyPressed() function to take in an InputCode that is an enum for many
of the keyboard keys we care about, and depending on the value we move the camera in the desired
direction.  For instance, if we press the UP arrow key, we will move the camera's Position along
the direction of the view vector, where if we hit the DOWN key we move it in the opposite direction.
Since we want to keep the speed consistent across computers with different speeds, we use our
TimeManager class to get a slice of time and apply a speed to that slice of time:

Camera->MoveCamera(Camera->GetSpeed() * TimeManager::Instance().DeltaTime);

The Camera object is passed to the InputManager so it can move it.

*Projection matrix stored with our camera*
Starting in this tutorial and on, we moved the creation of the projection matrix inside of our
new Camera class with the SetProjection() function.  Also, instead of passing in the projection
and view matrices into our model class (the triangle) we instead pass in the camera and get those
matrices from our camera instead on the fly.
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
	if ( !WindowManager || WindowManager->Initialize(ScreenWidth, ScreenHeight, "GameTutorials - Camera", false) != 0 )
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

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Create the projection matrix from our camera
	//									 FOV		    Aspect Ratio			   Near / Far Planes
	Camera->SetPerspective(glm::radians(60.0f), ScreenWidth / (float)ScreenHeight, 0.5f, 150.f);

	// Below we call our init function for our camera to give it a position, a yaw and a pitch rotation.
	// 0 for a yaw and pitch will put the camera looking directly down the negative z-axis.  We don't
	// do any rotations yet so a different pitch or yaw won't do anything until our next tutorial.

	//					  Position	  Yaw	 Pitch
	Camera->PositionCamera(0, 0, 6,		0,		0);

	// We now pass in the camera to have access to the projection and view matrices
	g_Triangle.SetCamera(Camera);
	
	// Set the position of the first triangle to be at the origin
	g_Triangle.SetPosition(vec3(0, 0, 0));

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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

		// We changed our game loop to stop using the lookAt() function here to get the view matrix,
		// but instead now just pass in the camera to our objects so they can access the projection
		// and view matrix internally.  In a more robust system, we would likely want to pass in a
		// rendering class that would have access to the type of rendering we wanted and the camera.

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

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


	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// If we have the camera still, delete it
	if ( Camera )
	{
		delete Camera;
		Camera = nullptr;
	}

	//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// In this tutorials we show you how to create part of a Camera class to move the
// camera back and forth.  The direction is hard coded to be along the negative 
// z-axis because we haven't programmed any rotations yet, but the next tutorial
// will allow the camera to look left and right and then use that new direction
// to move along.
//
// © 2000-2015 GameTutorials