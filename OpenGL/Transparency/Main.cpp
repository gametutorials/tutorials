//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Transparency									 //
//																		 //
//		$Description:	This creates a see through cube with blending	 //
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


// This tutorial is taken from the Rotating Cube tutorial and added upon.
// The point of this tutorial is to show you how to use OpenGL's blending
// capabilities to create see through polygons.  In a later tutorial we will
// use this to load in particle masks to create particle effects.  If you have
// a bitmap that has a black background, it will ignore the black and create a transparency.
// More will be discussed on that at a later time though.  In this tutorial we have
// 2 cubes.  The largest of the cube is transparent, while the smaller cube inside
// is rendered like normal.  It's a neat little effect.  You can better see the result
// with texture mapping, but I didn't want to assume you have done texture mapping yet.


///////////////////////////////// CREATE CUBE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This create a cube at X Y Z being the bottom left corner.
/////   The width and height depend on the radius passed in.
/////
///////////////////////////////// CREATE CUBE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateCube(float x, float y, float z, int radius, int color)
{
	glBegin(GL_QUADS);		
		
		// Since the last tutorial on rotating a cube, I have added
		// an extra parameter to this function to change the color to something
		// different.  I could have just copied this function and did whole new
		// color values, but I didn't want to bog you down with more code.  To get
		// the best effect for this though, you should copy this function and create
		// another one that has crazy colors like orange, blue and red.  Then you can
		// really see the difference between the 2 cubes.  Basically all this extra
		// parameter does is switch where the colors are attached to the vertices,
		// but we don't change the base colors that we used in the last tutorial.

		// These vertices create the Back Side
		glColor3ub(color, color, 255 - color);		 glVertex3f(x, y, z);
		glColor3ub(255 - color, color, 255 - color); glVertex3f(x, y + radius, z);
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y + radius, z); 
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y, z);

		// These vertices create the Front Side
		glColor3ub(color, color, 255 - color);		 glVertex3f(x, y, z + radius);
		glColor3ub(255 - color, color, 255 - color); glVertex3f(x, y + radius, z + radius);
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y + radius, z + radius); 
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y, z + radius);

		// These vertices create the Bottom Face
		glColor3ub(color, color, 255 - color);		 glVertex3f(x, y, z);
		glColor3ub(255 - color, color, 255 - color); glVertex3f(x, y, z + radius);
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y, z + radius); 
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y, z);
			
		// These vertices create the Top Face
		glColor3ub(color, color, 255 - color);		 glVertex3f(x, y + radius, z);
		glColor3ub(255 - color, color, 255 - color); glVertex3f(x, y + radius, z + radius);
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y + radius, z + radius); 
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y + radius, z);

		// These vertices create the Left Face
		glColor3ub(color, color, 255 - color);		 glVertex3f(x, y, z);
		glColor3ub(255 - color, color, 255 - color); glVertex3f(x, y, z + radius);
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x, y + radius, z + radius); 
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x, y + radius, z);

		// These vertices create the Right Face
		glColor3ub(color, color, 255 - color);		 glVertex3f(x + radius, y, z);
		glColor3ub(255 - color, color, 255 - color); glVertex3f(x + radius, y, z + radius);
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y + radius, z + radius); 
		glColor3ub(color, 255 - color, 255 - color); glVertex3f(x + radius, y + radius, z);

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

	// Only 4 lines of code where changed in this function.  First, we added
	// another cube, but this one is half the size as the original.  Second, we
	// added the glEnable()/glDisable() and glBlendFunc() functions.  More explained below.

	glRotatef(rotX, 1.0f, 0.0f, 0.0f);					// Rotate the cubes around the X-axis
	glRotatef(rotY, 0.0f, 1.0f, 0.0f);					// Rotate the cubes around the Y-axis	
	glRotatef(rotZ, 0.0f, 0.0f, 1.0f);					// Rotate the cubes around the Z-axis

	// This will be the smaller cube inside of the bigger cube.  You notice we
	// pass in 255 as the color value at the end.  That basically just creates the
	// colors in the opposite position as the other cube so it looks a bit better.

	CreateCube(-0.5f, -0.5f, -0.5f, 1, 255);			// Create the smaller cube inside the bigger one

	// This turns blending on.  Anything drawn after this will be transparent.

	glEnable(GL_BLEND);									// Turn on blending

	// This actually chooses the blending mode we want.  Without this, nothing would happen.
	// Basically, the first parameter specifies how the red, green, blue, 
	// and alpha source-blending factors are calculated.  The next parameter specifies how
	// the red, green, blue, and alpha destination-blending factors are calculated.  We chose
	// a ONE to ONE ratio basically.  There is a equation for each of the (R, G, B, A) values.
	// In a later tutorial we will examine some other flags that we could pass in, like GL_SRC_COLOR.
	// Look in MSDN for further explanations on each of these flags.

	glBlendFunc(GL_ONE, GL_ONE);						// Make our transparency a 1:1 ratio

	CreateCube(-1, -1, -1, 2, 0);						// This creates our larger transparent cube

	// The last step is to disable blending.  This assures that nothing else will
	// be blended from here on.  This finishes the scope of our blending.

	glDisable(GL_BLEND);								// Disable blending from here on out

	rotX += 0.3f;										// Increase the X rotation
	rotY += 0.3f;										// Increase the Y rotation
	rotZ += 0.8f;										// Increase the Z rotation

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	SwapBuffers(g_hDC);									// Swap the backbuffer to the foreground
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LONG    lRet = 1; 
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
 
    case WM_DESTROY:									// If the window is destroyed
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
// This tutorial shows you how to create transpareny polygons.  This effect is really
// cool once you put texture mapping in.  This will allow you to create water effects,
// windows, particles, and tons of other see through applications.
// 
// Let's go over the steps to create transparencies.
// 
// 1) To enable blending, we first must call:
//
//	       glEnable(GL_BLEND);
// 
// 2) Just enabling blending doesn't do a thing, we actually need to set our blending 
//    technique.  In this tutorial, we just chose a 1 to 1 ratio.  To do this we called:
//
//         glBlendFunc(GL_ONE, GL_ONE);
//
// 3) Next, we render anything to the screen and it will be effected with the transparency.
//
// 4) To disable blending we call:
//
//         glDisable(GL_BLEND);
//
// That's it!  Simple eh?  If we disable blending, we just need to enable it again.  We don't
// have to call glBlendFunc() again unless we want to change the blending factors.
//
// If you haven't checked out the texture mapping tutorial, I would suggest getting that down.
// Once you learn that, go create some cool water effect.  You can increase the UV coordinates
// slowly over a large QUAD with a water texture on it and enable blending.  It looks neat.  
// Eventually you will want to make the water move so you will need to increase the poly count
// in that quad.  Maybe we will write a tutorial on this.
//
// Though as simple as this is, I hope the tutorial helps you to get started with transparency.
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

