//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		Camera5	(strafing)								 //
//																		 //
//		$Description:	Demonstrates camera strafing right and left		 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file
#include "Camera.h"

CCamera g_Camera;										// This is our global camera object

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

///////////////////////////////////////////////////////////////////////////////////////
//
// The code in this tutorial was taken from the Camera4 tutorial.  This is our last camera 
// tutorial.  We might create a bezier curve camera tutorial though in the future.  This 
// tutorial shows how to strafe the camera right or left.  
//

///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the game window.
/////
///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	// Init our camera position

						// Position        View		   Up Vector
	g_Camera.PositionCamera(0, 1.5f, 6,   0, 1.5f, 0,   0, 1, 0);
}


/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function clamps a specified frame rate for consistency
/////
/////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f; // Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime; // Get the slice of time
	float desiredFPS = 1.0f / desiredFrameRate; // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		elapsedTime -= desiredFPS; // Adjust our elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main loop
/////
///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{ 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit
				break;
			TranslateMessage(&msg);						// Find out what the message does
			DispatchMessage(&msg);						// Execute the message
		}
		else											// if there wasn't a message
		{ 		
			if(AnimateNextFrame(60))					// Make sure we only animate 60 FPS
			{


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

				// We now just need to call Update() from the camera object to
				// deal with the input and updating.

				g_Camera.Update();							// Update the camera information
				RenderScene();								// Render the scene every frame

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


			}
			else
			{
				Sleep(1);								// Let other processes work
			}
		} 
	}

	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}


///////////////////////////////// CREATE PYRAMID \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a pyramid with the center being (X, Y, Z).
/////   The width and height depend on the WIDTH and HEIGHT passed in.
/////
///////////////////////////////// CREATE PYRAMID \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreatePyramid(float x, float y, float z, int width, int height)
{
	// Below we create a pyramid (hence CreatePyramid() :).  This entails
	// 4 triangles for the sides and one QUAD for the bottom.  We could have done
	// 2 triangles instead of a QUAD but it's less code and seemed appropriate.
	// We also assign some different colors to each vertex to add better visibility.
	// The pyramid will be centered around (x, y, z).  This code is also used in the
	// lighting and fog tutorial on our site at www.GameTutorials.com.

	// Start rendering the 4 triangles for the sides
	glBegin(GL_TRIANGLES);		
		
		// These vertices create the Back Side
		glColor3ub(255, 0, 0);   glVertex3f(x, y + height, z);					// Top point
		glColor3ub(0, 255, 255); glVertex3f(x - width, y - height, z - width);	// Bottom left point
		glColor3ub(255, 0, 255); glVertex3f(x + width, y - height, z - width);  // Bottom right point

		// These vertices create the Front Side
		glColor3ub(255, 0, 0);   glVertex3f(x, y + height, z);					// Top point
		glColor3ub(0, 255, 255); glVertex3f(x + width, y - height, z + width);  // Bottom right point
		glColor3ub(255, 0, 255); glVertex3f(x - width, y - height, z + width);	// Bottom left point

		// These vertices create the Front Left Side
		glColor3ub(255, 0, 0);   glVertex3f(x, y + height, z);					// Top point
		glColor3ub(255, 0, 255); glVertex3f(x - width, y - height, z + width);	// Front bottom point
		glColor3ub(0, 255, 255); glVertex3f(x - width, y - height, z - width);	// Bottom back point

		// These vertices create the Front Right Side
		glColor3ub(255, 0, 0);   glVertex3f(x, y + height, z);					// Top point
		glColor3ub(255, 0, 255); glVertex3f(x + width, y - height, z - width);	// Bottom back point
		glColor3ub(0, 255, 255); glVertex3f(x + width, y - height, z + width);	// Front bottom point
			
	glEnd();

	// Now render the bottom of our pyramid

	glBegin(GL_QUADS);

		// These vertices create the bottom of the pyramid
		glColor3ub(0, 0, 255); glVertex3f(x - width, y - height, z + width);	// Front left point
		glColor3ub(0, 0, 255); glVertex3f(x + width, y - height, z + width);    // Front right point
		glColor3ub(0, 0, 255); glVertex3f(x + width, y - height, z - width);    // Back right point
		glColor3ub(0, 0, 255); glVertex3f(x - width, y - height, z - width);    // Back left point
	glEnd();
}


///////////////////////////////// DRAW 3D GRID \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function create a simple grid to get a better view of the animation
/////
///////////////////////////////// DRAW 3D GRID \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Draw3DSGrid()
{
	// This function was added to give a better feeling of moving around.
	// A black background just doesn't give it to ya :)  We just draw 100
	// green lines vertical and horizontal along the X and Z axis.

	// Turn the lines GREEN
	glColor3ub(0, 255, 0);

	// Draw a 1x1 grid along the X and Z axis'
	for(float i = -50; i <= 50; i += 1)
	{
		// Start drawing some lines
		glBegin(GL_LINES);

			// Do the horizontal lines (along the X)
			glVertex3f(-50, 0, i);
			glVertex3f(50, 0, i);

			// Do the vertical lines (along the Z)
			glVertex3f(i, 0, -50);
			glVertex3f(i, 0, 50);

		// Stop drawing lines
		glEnd();
	}
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// DRAW SPIRAL TOWERS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This draws a spiral of pyramids to maneuver around
/////
///////////////////////////////// DRAW SPIRAL TOWERS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawSpiralTowers()
{
	const float PI = 3.14159f;							// Create a constant for PI
	const float kIncrease = PI / 16.0f;					// This is the angle we increase by in radians
	const float kMaxRotation = PI * 6;					// This is 1080 degrees of rotation in radians (3 circles)
	float radius = 40;									// We start with a radius of 40 and decrease towards the center

	// Keep looping until we go past the max degrees of rotation (which is 3 full rotations)
	for(float degree = 0; degree < kMaxRotation; degree += kIncrease)
	{
		// Here we use polar coordinates for the rotations, but we swap the y with the z
		float x = float(radius * cos(degree));			// Get the x position for the current rotation and radius
		float z = float(radius * sin(degree));			// Get the z position for the current rotation and radius

		// Create a pyramid for every degree in our spiral with a width of 1 and height of 3 
		CreatePyramid(x, 3, z, 1, 3);
	
		// Decrease the radius by the constant amount so the pyramids spirals towards the center
		radius -= 40 / (kMaxRotation / kIncrease);
	}	
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Instead of calling gluLookAt(), we just call our Look() function from
	// the camera object.  This makes it more object oriented, along with
	// cleaning up RenderScene().  If we were building an engine, you would
	// not want to ever explicitly call a specific graphics API.  This allows
	// you to not have to change client code, but only the code underneath
	// when a new API is desired for rendering, or a code addition/bug fix.
	g_Camera.Look();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Draw a grid so we can get a good idea of movement around the world		
	Draw3DSGrid();


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Draw the pyramids that spiral in to the center
	DrawSpiralTowers();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Swap the backbuffers to the foreground
	SwapBuffers(g_hDC);									
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are in window mode
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);					// Get the window rectangle
		}
        break; 

	case WM_KEYDOWN:

		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;
		}
		break;

    case WM_CLOSE:										// If the window is being closes
        PostQuitMessage(0);								// Send a QUIT Message to the window
        break; 
     
    default:											// Return by default
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This tutorial was taken from the Camera4 tutorial and given additions.  This tutorial
// shows how to strafe the camera right or left.  This might seem easy when you think to
// attempt it, but if you don't know your linear algebra it can be tricky.  Strafing the
// camera is moving the camera 90 degrees left or right from the current view.  In other
// words, it's as if you were side stepping while you look forward.  This is used in most
// first person shooters games, and comes in handy when peering around corners or running
// past a hallway while firing off some rounds.  It's also a great way to move diagonal
// while doing a shootout at close ranges to avoid being hit.
//
// Since we understand what strafing is and what it does, let me actually explain how it works.
// We know that we want to walk in the direction that is 90 degrees from the view vector (with
// the view vector being m_vView - m_vPosition).  So how do we then get the vector that is 90
// degrees from our view vector?  If you know what the cross product is, you can easily see how
// this would be done.  The cross product is a mathematical formula that takes 2 vectors and
// returns the vector 90 degrees from those 2 vectors.  This is how you find the normal to a plane.
// Well, we have a view vector, but what would the other vector be?  Does the up vector come to mind?
// That's it!  We want to take the cross product between the up vector and the view vector.  This
// will return the vector (or direction) that we want to strafe in.  In games like Descent, the
// up vector will change because you can go upside down and spin in crazy directions.  The cross
// product will ensure that we will always strafe correctly no matter what orientation the camera is in.
// Once we have the strafe vector, we need to add it to the position and view points.
// Here are the controls for this tutorial:
//
// w, s, UP_ARROW, DOWN_ARROW - Move the camera forward and backward
// a, d, RIGHT_ARROW, LEFT_ARROW - Strafe the camera left and right
// Mouse movement - Moves the view for first person mode
// ESCAPE - Quits the program
//
// You may notice that we don't use all the methods in the camera class.  I decided to leave
// them in there because it would be nice to just use this in future projects without having to 
// cut and paste code.  That's also why I finally added a camera.h so you don't have the camera 
// class in main.h.
// 
// Enjoy!
//
//
// © 2000-2005 GameTutorials
