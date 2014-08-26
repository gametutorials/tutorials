//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		RotateCube										 //
//																		 //
//		$Description:	This draws a rotating cube to the screen		 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file


bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// CREATE CUBE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This create a cube at X Y Z being the bottom left corner.
/////   The width and height depend on the radius passed in.
/////
///////////////////////////////// CREATE CUBE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateCube(float x, float y, float z, int radius)
{
	// Here we create 6 QUADS (Rectangles) to form a cube
	// With the passed in radius, we determine the width and height of the cube
	// You will notice, I add some color at each vertice to make it more interesting

	glBegin(GL_QUADS);		
		
		// These vertices create the Back Side
		glColor3ub(0, 0, 255);   glVertex3f(x, y, z);
		glColor3ub(255, 0, 255); glVertex3f(x, y + radius, z);
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y + radius, z); 
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y, z);

		// These vertices create the Front Side
		glColor3ub(0, 0, 255);   glVertex3f(x, y, z + radius);
		glColor3ub(255, 0, 255); glVertex3f(x, y + radius, z + radius);
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y + radius, z + radius); 
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y, z + radius);

		// These vertices create the Bottom Face
		glColor3ub(0, 0, 255);   glVertex3f(x, y, z);
		glColor3ub(255, 0, 255); glVertex3f(x, y, z + radius);
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y, z + radius); 
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y, z);
			
		// These vertices create the Top Face
		glColor3ub(0, 0, 255);   glVertex3f(x, y + radius, z);
		glColor3ub(255, 0, 255); glVertex3f(x, y + radius, z + radius);
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y + radius, z + radius); 
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y + radius, z);

		// These vertices create the Left Face
		glColor3ub(0, 0, 255);   glVertex3f(x, y, z);
		glColor3ub(255, 0, 255); glVertex3f(x, y, z + radius);
		glColor3ub(0, 255, 255); glVertex3f(x, y + radius, z + radius); 
		glColor3ub(0, 255, 255); glVertex3f(x, y + radius, z);

		// These vertices create the Right Face
		glColor3ub(0, 0, 255);   glVertex3f(x + radius, y, z);
		glColor3ub(255, 0, 255); glVertex3f(x + radius, y, z + radius);
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y + radius, z + radius); 
		glColor3ub(0, 255, 255); glVertex3f(x + radius, y + radius, z);

	glEnd();
}

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

	// The rest of our initialization would go here (Models, textures, etc...)
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
				RenderScene();							// Render the scene every frame
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

float rotX = 0.0f, rotZ = 0.0, rotY = 0.0f;

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix
	
		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Below we rotate our cube by the glRotatef() function.
	// The first parameter is the degrees to rotate by (That is why we need to keep the data)
	// We create 3 global variables to store and keep the current rotation so it can be incremental
	// and smooth.  The next 3 parameters is the vector we are rotating by.
	// If we want to rotate around the X axis, we pass in (1, 0, 0)
	// if want to rotate around the Y axis, we pass in (0, 1, 0) and (0, 0, 1) for Z.
	// That is how it knows how to rotate it along which axis.  We are basically give it a rotation vector.
	// You might be thinking, well... where does it change the vertices?  It doesn't.
	// There is a maxtrix that is associated with our projection, rotation, etc...
	// We just times that current matrix by a rotation matrix, which rotates the cube
	// on the screen, it doesn't actually change the vertices.  It is faster to use matrixs
	// than it is to rotate every vertice by itself.  I personally like doing every vertice so I
	// can keep track of where every vertice is in the world, but it is a lot slower that way.

	glRotatef(rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);
	glRotatef(rotZ, 0.0f, 0.0f, 1.0f);

	// This creates our cube starting the bottom left corner at (-1, -1, -1)
	// We give our cube a width and height of 2.  Nice little function eh? :)
	// See if you can figure out how to texture map the cube using the past tutorials.
	CreateCube(-1, -1, -1, 2);

	// Since we center the cube around the origin (0, 0, 0) , it rotates nicely.
	// if we didn't, it would rotate in a weird way, like try changing the z position to 0.

	// Below we just increment our rotation degree by a small amount each frame.
	// This increase the rotation of the cube along each axis.

	rotX += 0.3f;
	rotY += 0.3f;
	rotZ += 0.8f;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground
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

	case WM_KEYDOWN:
		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;
		}
		break;
 
    case WM_CLOSE:										// If the window is closed
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
// Creating a cube is simple isn't it?  Once you know how to draw a QUAD, you know
// how to draw a cube.  it's just 6 quads positioned correctly.
// We created a nifty Function called CreateCube() which can make cubes to our hearts delight :)
// 
// ROTATING:  We learned how to use the glRotatef() function.  This is as simple as it looks.
//            Just pass in the degree you want to rotate, and then place a 1 in the axis
//            you want to rotate around.  Try messing around with glTranslatef(x, y, z);
//
// Hopefully this tutorial helps out enough for you to get your own cubes and rotations up
// and running.  Let us know what you do with this or if you have any feedback about the tutorial.
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
