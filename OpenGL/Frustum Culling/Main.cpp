//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Frustum Culling									 //
//																		 //
//		$Description:	Demonstrates checking if shapes are in view		 //
//																		 //
//***********************************************************************//


// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file
#include "camera.h"

// This tutorial was built from the base of the Quadric tutorial.

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()
float g_DT;												// This stores the elapsed time between the current and last frame


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This tutorial demonstrates how to tell if an object is inside of your frustum or not.
// What is a frustum?  A frustum is the camera's view.  If something does NOT appear
// on the screen, then it is not inside of the frustum.  Think of it as a skewed box.
// It is skewed because the farther away, the more you can see.  It has 6 sides.
// There is the right, left, top, bottom, near and far.  Anything rendered outside
// of these planes, is not shown.  You need to know how to test this because it speeds
// up your programs, especially when there are a lot of objects, and/or a large world.
// You don't want to send you whole world down the 3D pipeline, that would kill your
// frame rate, so you want to test to see if the object would even NEED to be drawn
// before you send it to be rendered.  Certainly this is useless if you have very few objects,
// depending on their size of course.
//
// CONTROLS:
//		MOUSE MOVEMENT - Looks around
//      C, SPACE BAR, Mouse LEFT CLICK- Turns culling ON/OFF
//      +/- - Increase and decreases the amount of spheres
//		ESCAPE - quits

// You can tell how many spheres are being rendered out of how 
// many possible in the windows title bar.  You will notice the huge difference
// from 1000 possible spheres when frustum culling is turned off.  It barely moves.

#define MAX_SPHERES		1000							// This holds the maximum amount of spheres.  (use + and - to change amount)
#define MAX_DISTANCE	30								// This is the distance that the spheres disappear on either side
#define SPHERE_SPEED	0.2f							// This is the speed the spheres move
#define MAX_RADIUS		5								// This is the spheres radius*10 (really it's 0.5)

// The camera object we have here is from the camera tutorial series
CCamera	g_Camera;										// This is our global camera object

CFrustum g_Frustum;										// This is our global frustum object

bool g_bIgnoreFrustum = false;							// We want to check against the frustum initially

// You can change this variable by the +/- keys.
int g_MaxSpheres = MAX_SPHERES / 2;						// Set the spheres on screen to the max/2 allowed.

Sphere g_Spheres[MAX_SPHERES];							// This is our structure that holds the sphere data

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	ShowCursor(false);									// Turn the cursor off for camera movement

	g_Camera.PositionCamera(0, 0, 1,	0, 0, 0,	0, 1, 0);

	// Here we turn on lighting, but just leave the defaults so at
	// least we have some good shading on the spheres.
	glEnable(GL_LIGHT0);								// Turn on a light
	glEnable(GL_LIGHTING);								// Turn on lighting

	// Once lighting is turned on, we need to enable this to use color.
	glEnable(GL_COLOR_MATERIAL);						// Allow us to use color for the spheres

	srand(GetTickCount());								// Seed the random numbers

	// This is where we initialize the sphere's data.  We randomly assign color
	// and place them around the camera.

	for(int i = 0; i < MAX_SPHERES; i++)
	{
		// Assign the radius and position - No farther away than 30 from the camera
		g_Spheres[i].radius = (rand() % MAX_RADIUS) * 0.1f;
		g_Spheres[i].xPos = (rand() % (MAX_DISTANCE * 10)) * 0.1f;
		g_Spheres[i].yPos = (rand() % (MAX_DISTANCE * 10)) * 0.1f;
		g_Spheres[i].zPos = (rand() % (MAX_DISTANCE * 10)) * 0.1f;

		// We have a 50/50 chance of putting the sphere on the left side and behind the camera.
		// This is because we are centered at the origin
		if(rand() % 2) g_Spheres[i].xPos = -g_Spheres[i].xPos;
		if(rand() % 2) g_Spheres[i].yPos = -g_Spheres[i].yPos;
		if(rand() % 2) g_Spheres[i].zPos = -g_Spheres[i].zPos;

		// Assign the sphere's color randomly
		g_Spheres[i].r = rand() % 256;					// Get a random red color
		g_Spheres[i].g = rand() % 256;					// Get a random green color
		g_Spheres[i].b = rand() % 256;					// Get a random blue color
	}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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
		g_DT = desiredFPS;
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

				// Render the scene every frame, and also check for mouse movement
				g_Camera.Update();							// Update the camera data
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


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	int spheresRendered = 0;							// This will hold how many spheres are being rendered
	char strText[255]={0};								// This will hold the window title info

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

	// Give OpenGL our camera position
	g_Camera.Look();

	// We don't need to calculate this every frame, only when the camera view changes.
	// I just did it every frame anyway.  In this case it isn't a big deal.
	g_Frustum.CalculateFrustum();						// Calculate the frustum each frame

	// If you are unfamiliar with Quadrics, see the tutorial on Quadrics at www.GameTutorials.com.
	// They basically allow you to draw circles and cylinders fast and easily.

	GLUquadricObj *pObj = gluNewQuadric();				// Get a Quadric off the stack
	
	// Loop through all of our allowed spheres and render them to the screen if in the frustum.
	for(int i = 0; i < g_MaxSpheres; i++)				// g_MaxSpheres varies.
	{
		g_Spheres[i].zPos += SPHERE_SPEED;				// Increase the Z position of the sphere.

		// Below we check if the sphere needs to be draw or not.  If g_bIgnoreFrustum is TRUE,
		// it draws it regardless (which is SLOOOooOoW).  We just pass in the (X, Y, Z)
		// and the radius of the sphere to find out if it is inside of the frustum.

		if(g_bIgnoreFrustum || g_Frustum.SphereInFrustum(g_Spheres[i].xPos, g_Spheres[i].yPos, g_Spheres[i].zPos, g_Spheres[i].radius))
		{
			// Set the sphere's color
			glColor3ub(g_Spheres[i].r, g_Spheres[i].g, g_Spheres[i].b);

			// Create a new scope before positiong the sphere so we don't effect the other spheres.
			glPushMatrix();

				// Position the sphere on the screen at it's XYZ position.
				glTranslatef(g_Spheres[i].xPos, g_Spheres[i].yPos, g_Spheres[i].zPos);						

				// Create a sphere with the desired radius chosen in the beginning.
				gluSphere(pObj, g_Spheres[i].radius, 20, 20);	// Draw the sphere with a radius of 0.5
			glPopMatrix();								// Close the scope of this matrix

			spheresRendered++;							// Increase the amount of spheres rendered
		}

		// Here we check to see if the sphere went out of our range,
		// If so, we need to set it back again with a new random position.
		if(g_Spheres[i].zPos > MAX_DISTANCE) {
			// Give the sphere a new random position back in the beginning.
			g_Spheres[i].xPos = (rand() % (MAX_DISTANCE * 10)) * 0.1f;
			g_Spheres[i].yPos = (rand() % (MAX_DISTANCE * 10)) * 0.1f;
			g_Spheres[i].zPos = -MAX_DISTANCE;			// Send it to the back again.

			// Give a 50/50 chance for the sphere to be to the left/right or above/below the XY axis.
			// This is because we are centered at the origin
			if(rand() % 2) g_Spheres[i].xPos = -g_Spheres[i].xPos;
			if(rand() % 2) g_Spheres[i].yPos = -g_Spheres[i].yPos;
		}
	}
	
	// Since I didn't want to add more code for a rendered font, I decided to just
	// render the frustum information in the title bar of the window.
	// The information tells you how many spheres were rendered and out of how many.
	// Use +/- to increase and decrease the max spheres tested.

	sprintf(strText, "www.GameTutorials.com - Spheres Rendered: %d / %d", spheresRendered, g_MaxSpheres);
	SetWindowText(g_hWnd, strText);						// Change the window title bar

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground
	gluDeleteQuadric(pObj);								// Free the Quadric
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 0; 
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// We added some more controls to increase the spheres and turn frustum
	// culling ON/OFF.

	case WM_LBUTTONDOWN:								// If the left mouse button was clicked
		g_bIgnoreFrustum = !g_bIgnoreFrustum;	
		break;

	case WM_KEYDOWN:									// If we hit a key
		switch (wParam)									// Check which key we hit
		{
			case VK_ESCAPE:								// If we hit ESCAPE
				PostQuitMessage(0);						// Tell windows we want to quit
				break;

			case VK_ADD:								// If we hit '+'
				g_MaxSpheres += 10;						// Increase the amount of spheres

				if(g_MaxSpheres > MAX_SPHERES)			// If we went over the limit
				{
					g_MaxSpheres = MAX_SPHERES;			// Set the max back to the limit
				}
				break;

			case VK_SUBTRACT:							// if we hit '-'
				g_MaxSpheres -= 10;						// Decrease the amount of spheres

				if(g_MaxSpheres < 0)					// If we went below zero
				{
					g_MaxSpheres = 0;					// Set the amount of spheres back to zero
				}
				break;

			case 'c': case 'C':	case VK_SPACE:			// If we hit 'C' or the Space bar
				g_bIgnoreFrustum = !g_bIgnoreFrustum;		// Turn frustum culling on and off
				break;
			}
		break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

    case WM_CLOSE:										// If the window is closed
        PostQuitMessage(0);								// Tell windows we want to quit
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
// This is the largest tutorial we have created so far.  Not a lot of concept,
// but a lot of code.  The only code you need to look at is in RenderScene()
// and Frustum.cpp.  That is the good stuff :).  The rest of it is just to 
// make it a little more interesting.  I toiled over whether I should just have
// a bunch of spheres going back and forth on the screen, but I eventually decided
// it would be better if there was some camera movement so it didn't look fixed.
// You can use the mouse to move the camera around, as well as left click to turn
// culling on and off.  You can also add and subtract the spheres with '+'/'-'.
// Like Usual, Escape quits.  I took the Camera code from the Camera3 tutorial.
// This tutorial was based off the quadric tutorial.
//
// Let's go over a brief overview of the things we learned here:
//
// 1) First we need to abstract the frustum from OpenGL.  To do that we need the
//    projection and modelview matrix.  To get the projection matrix we use:
//
//			glGetFloatv( GL_PROJECTION_MATRIX, /* An Array of 16 floats */ );
//    Then, to get the modelview matrix we use:
//
//			glGetFloatv( GL_MODELVIEW_MATRIX, /* An Array of 16 floats */ );
//    
//	  These 2 functions gives us an array of 16 floats (The matrix).
//
// 2) Next, we need to combine these 2 matrices.  We do that by matrix multiplication.
//
// 3) Now that we have the 2 matrixes combined, we can abstract the sides of the frustum.
//    This will give us the normal and the distance from the plane to the origin (ABC and D).
//
// 4) After abstracting a side, we want to normalize the plane data.  (A B C and D).
//
// 5) Now we have our frustum, and we can check points against it using the plane equation.
//    Once again, the plane equation (A*x + B*y + C*z + D = 0) says that if, point (X,Y,Z)
//    times the normal of the plane (A,B,C), plus the distance of the plane from origin,
//    will equal 0 if the point (X, Y, Z) lies on that plane.  If it is behind the plane
//    it will be a negative distance, if it's in front of the plane (the way the normal is facing)
//    it will be a positive number.
//
//
// If you need more help on the plane equation and why this works, download our
// Ray Plane Intersection Tutorial at www.GameTutorials.com.
//
// That's pretty much it with frustums.  There is a lot more we could talk about, but
// I don't want to complicate this tutorial more than I already have.
//
// I want to thank Mark Morley for his tutorial on Frustum.  Most of everything I got
// here comes from his teaching.  If you want more in-depth, visit his tutorial at:
//
// http://www.markmorley.com/opengl/frustumculling.html
//
// Good luck!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
