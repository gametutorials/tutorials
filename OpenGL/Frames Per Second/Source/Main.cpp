//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Program:		Frames Per Second								 //
//																		 //
//		$Description:	Demonstrates calculating the frame rate			 //
//																		 //
//***********************************************************************//

#include "GL/glew.h"									// Include the GLEW library to manage OpenGL extensions
#include "../Headers/Main.h"							// Include our main header for the application


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

/*
In this tutorial we create a TimeManager class that calculates the current frame rate as well 
as provides a DeltaTime which is a time slice to use for making sure any movement is based on 
time and not by frame counts.  We show this by rotating a triangle along the y-axis at a
constant rate.  To explain this easily, if we were to increase a rotation amount of 0.2 degrees
every time we render the scene it would move differently on different computers because some
would have faster or slower computers and see different results.  To get around this, we use
something that doesn't change from computer to computer, and that is time.  Every frame we get
a time slice (DeltaTime) that is the difference between the current time and the last frame's
time.  Now we have a value to multiply a speed by in order to move or rotate an object the
same no matter what system it's on.  We use this for camera movements or animations.

The TimeManager class uses the Singleton design pattern so that it makes sure that only one
instance of the class is ever instantiated.  This way we don't need to pass around the class
but can just access it globally through its TimeManager::Instance() function.  So if we want
to get the current time slice we just do this:

double timeSlice = TimeManager::Instance().DeltaTime;

In order for the DeltaTime to be calculated, we need to call the CalculateFrameRate() function
in the TimeManager class every frame.  We do this in our GameLoop().  We also display the 
current frame rate in the console window if you set writeToConsole to true.

We get to use our rotate functionality in this tutorial and this is done by giving our model
a rotation value in radians for the y-axis by setting the y value of a vec3.  We don't use
degrees, but need to convert our degrees into radians (or 2*3.14159).  Radians are the
standard unit of angular measure, which means when you do rotation mathematics you don't
use degrees, but radians.  1 radian is equal to 57.3 degrees when the radians and arc length
are equal.  Half of a circle is measured in π (PI), and a full circle is measure in 2π. PI
is a huge number which goes on forever, but generally is shortened to 3.14 radians.  GLM
provides a radians() function that takes in degrees.  Checkout the GameLoop() function for
the rotation logic that combines with our time slice to ensure the same experience on every
device.
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
	if ( !WindowManager || WindowManager->Initialize(ScreenWidth, ScreenHeight, "GameTutorials - Frames Per Second", false) != 0 )
	{
		// Quit the application if the window couldn't be created with an OpenGL context
		exit(-1);
	}

	// Tell OpenGL that we want a 3D viewport the same size as the window
	glViewport(0, 0, ScreenWidth, ScreenHeight);
	
	// This tells OpenGL that we want depth testing so it renders the order correctly
	glEnable(GL_DEPTH_TEST);

	// Create the projection matrix with GLM's perspective function, replacing gluPerspective.
	//												  FOV		    Aspect Ratio				Near / Far Planes
	mat4 projectionMatrix = perspective(glm::radians(60.0f), ScreenWidth / (float)ScreenHeight, 0.5f, 150.f);

	// Initialize the three XYZ points of the triangle as their associated Green, Red and Blue
	// colors.  We give an alpha value of 1.0, which gives the colors an opacity of 100%.
	Vertex3 vertices[] =
	{	//    X 	  Y	    Z      	   R	  G	   B	 A
		{ { -0.6f,  -0.6f, 0.0f },  { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ {  0.0f,   0.6f, 0.0f },  { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ {  0.6f,  -0.6f, 0.0f },  { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	// Initialize the triangle with the vertex array and give the length of 3.  This will create our
	// Vertex Buffer Object and Vertex Array Object.
	g_Triangle.Initialize(vertices, 3, "Shaders/Shader.vertex", "Shaders/Shader.fragment");

	// Pass in the projection matrix to our model so we can pass it to the vertex shader
	g_Triangle.SetProjectionMatrix(projectionMatrix);
}


// This is our game loop where all the magic happens every frame
void GLApplication::GameLoop()
{
	// Loop until the user hits the Escape key or closes the window
	while ( WindowManager->ProcessInput(true) )
	{

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Use our Singleton to calculate our framerate every frame, passing true to set FPS in titlebar
		TimeManager::Instance().CalculateFrameRate(true);

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Setup the camera:		  Camera Position			  Target				Up Vector
		mat4 viewMatrix = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

		// This clears the screen every frame to black (color can be changed with glClearColor)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Pass in the view matrix to the model so it can be used in the vertex shader
		g_Triangle.SetViewMatrix(viewMatrix);

		// Set the position of the first triangle to be at the origin
		g_Triangle.SetPosition(vec3(0, 0, 0));


		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Here we want to rotate our triangle 360 degrees every second, so we need to multiply
		// our degrees (in radians) by the time slice that our TimeManager generates each frame.

		// Just to keep our rotation code together here, we create a static variable that stores our
		// rotation value and increments every frame.
		static float yRotation = 0;

		// Convert 360 degrees to radians since that is what GLM uses (degrees are deprecated)
		auto radians360 = glm::radians(360.0f);

		// This multiplies our rotation value by the current time slice since the last frame.
		// This gives us a speed to increment our rotation value by (in radians).
		auto speed = radians360 * TimeManager::Instance().DeltaTime;
		
		// Increase our rotation value for the y-axis every frame
		yRotation += speed;

		// Not necessary, but if you want to restrict the rotation to 360 degrees (or 2π) then
		// check if the yRotiation goes past 360 degrees, wrap it back to 0 again (or near 0).
		if ( yRotation >= radians360 )
			yRotation -= radians360;

		// To rotate our triangle along the y-axis we pass in our rotation value in the y value
		// of the vector, and leave the other values for x and z to 0.
		g_Triangle.SetRotation(vec3(0, yRotation, 0.0f));

		//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Render the first triangle
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
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// We added a TimeManager class which uses the Singleton design pattern to give
// us a time slice every frame, which allows us to have consistent movement on
// any device.  We also added a CalculateFrameRate() that prints out the current
// frames per second to the console window if pass in true.  Keep in mind that 
// if you don't turn off vertical sync then you will likely get locked at your 
// monitor's refresh rate (usually 60 Hz which is 60 frames per second).  To turn 
// VSync off with GLFW we do this:  glfwSwapInterval(0);
//
// On my current system I get about 7000 frames per second, what do you get? It's
// a single triangle so hopefully it's at least 1000 frames per second or you might
// need a new video card :)
//
//
// © 2000-2014 GameTutorials