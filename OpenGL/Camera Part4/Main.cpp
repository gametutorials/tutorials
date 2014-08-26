//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		Camera4											 //
//																		 //
//		$Description:	Rotates the camera in 3rd person mode			 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file

#define kSpeed	0.06f									// This is how fast our camera moves


CCamera g_Camera;										// This is our global camera object

bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()


// This tutorial was taken from the Camera2 tutorial.  So far we had only first
// person camera tutorials, but now we move into the realm of 3rd person views.
// Some games that use this view are "Metal Gear Solid", "Tomb Raider", 
// "My Little Pony's Revenge" (okay.. I made that up).  This can be a tricky
// mode for a game because the feel and movement can be uncomfortable if you
// don't get it just right.  Many people complain about this view.  Another bad
// thing about this view is that it makes you render your character all the time
// which can slow down the frame rate.  In first person shooters you only have
// to render the people you come in contact with, which saves a bit of frame rate.
// A cool thing about this mode is that it lets you see what your character looks
// like and how they react to objects and characters/monsters in the world.  This
// tutorial adds very little code to the second camera tutorial to accomplish this
// result.  All we really did was add the function RotateAroundPoint() to our CCamera class.
// This rotates the camera around a certain point.  What point would that be?  Well, you
// usually want it to be your m_vView point (the place you are looking).  You can then
// set the position of your character at that point as well, which we do.  You might not
// always want to rotate around the view point, so we allow you to pass in your own
// center point to RotateAroundPoint().  This tutorial just creates a simple object
// in a simple world that moves around with you in third person.  The object would be
// replaced with your character, but for the sake of simplicity we create something easy.
// Use the arrow keys to move around.  You will notice when you hit the left or right arrow
// keys the camera will rotate around the object.  Just imagine that is your character.
// This tutorial doesn't do the full third person view like in Tomb Raider, but
// if you understand these basics, you can finish the rest.
//


///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the game window.
/////
///////////////////////////////// INIT GAME WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the window's rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	// Init our camera position

						// Position        View		   Up Vector
	g_Camera.PositionCamera(0, 1.5f, 6,   0, 1.5f, 0,   0, 1, 0);
}


///////////////////////////////// KEY PRESSED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the input more effectively than in the WinProc()
/////
///////////////////////////////// KEY PRESSED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void KeyPressed()
{
	if(GetKeyState(VK_UP) & 0x80) {				// If we hit the UP arrow key
		g_Camera.MoveCamera(kSpeed);			// Move our camera forward by a positive speed
	}

	if(GetKeyState(VK_DOWN) & 0x80) {			// If we hit the DOWN arrow key
		g_Camera.MoveCamera(-kSpeed);			// Move our camera backward by a negative speed
	}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// We made 2 changes from the Camera2 tutorial.  Instead of gCamera.RotateView()
	// we use our new RotateAroundPoint() function.  We pass in the camera's view
	// point.  This will be the point that we rotate our camera position around.
	
	if(GetKeyState(VK_LEFT) & 0x80) {			// If we hit the LEFT arrow key

		// We want to rotate around the Y axis so we pass in a positive Y speed
		g_Camera.RotateAroundPoint(g_Camera.m_vView, kSpeed, 0, 1, 0);	
	}

	if(GetKeyState(VK_RIGHT) & 0x80) {			// If we hit the RIGHT arrow key

		// Use a negative Y speed to rotate around the Y axis
		g_Camera.RotateAroundPoint(g_Camera.m_vView, -kSpeed, 0, 1, 0);	
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
				// When we aren't receiving windows messages we now check for input
				// from the keyboard.  This is a lot better method if we want movement.
				KeyPressed();								// Check for key presses
				RenderScene();								// Redraw the screen
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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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



///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

	// Give openGL our camera position, then camera view, then camera up vector
	gluLookAt(g_Camera.m_vPosition.x, g_Camera.m_vPosition.y,  g_Camera.m_vPosition.z,	
			  g_Camera.m_vView.x,	  g_Camera.m_vView.y,      g_Camera.m_vView.z,	
			  g_Camera.m_vUpVector.x, g_Camera.m_vUpVector.y,  g_Camera.m_vUpVector.z);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Draw a grid so we can get a good idea of movement around the world		
	Draw3DSGrid();

	// Below we create 4 pyramids which will just be objects in the world
	// to orient us.  Nothing special, but I figured I should add them since
	// it was only 4 lines of extra code and they act as pillars.

	// Create a pyramid at (-6, 3, 6) with a width and height of 1 and 6
	CreatePyramid(-6, 3, 6, 1, 6);

	// Create a pyramid at (-6, 3, 6) with a width and height of 1 and 6
	CreatePyramid(6, 3, 6, 1, 6);

	// Create a pyramid at (-6, 3, 6) with a width and height of 1 and 6
	CreatePyramid(6, 3, -6, 1, 6);

	// Create a pyramid at (-6, 3, 6) with a width and height of 1 and 6
	CreatePyramid(-6, 3, -6, 1, 6);

	// Translate the object represented as our character to our view position.
	// This way we will always be looking at the object and it will follow us
	// where ever we go.
	glTranslatef(g_Camera.m_vView.x, 0, g_Camera.m_vView.z);

	// Now we create our object.  It's simply 2 pyramids with one turned upside down.
	// This will always be in the center of our screen because we keep setting it's
	// position to be where we are looking.

	// Create a pyramid at (0, 2, 0) with a width and height of 1
	CreatePyramid(0, 2, 0, 1, 1);

	// Before we create the other pyramid that makes up our object, 
	// we rotate it 180 degrees around the X axis which turns it upside down.
	glRotatef(180, 1, 0, 0);

	// Create a pyramid at (0, -1, 0) with a width and height of 1
	CreatePyramid(0, -1, 0, 1, 1);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
// This tutorial demonstrates how to do a third person camera view.  There is a lot
// more that this view deals with, but this is the basics.  How it's done is really
// simple.  Instead of rotating the view around the position, you just rotate the
// position around the view.  I just took RotateView() and swapped the places that
// m_vView was with m_vPosition and vice versa.  However, we use vCenter because we
// might not always want to use the view, it could be some other point.  That way
// it's modular and doesn't have to just be rotating around the view.  In this
// tutorial we just pass in g_Camera.m_View for the point to rotate around.
// 
// Not much else to say about this, so I hope this helps someone out that might
// be struggling with this.  You can use the arrow keys to move around the world.
// 
// Enjoy!
// 
//
// © 2000-2005 GameTutorials
