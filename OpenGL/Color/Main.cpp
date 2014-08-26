//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Color											 //
//																		 //
//		$Description:	Adds color to our OpenGL primitives				 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file

// These are externed in main.h.  Look in main.h for more explanation on externing.
// Basically, it allows Init.cpp to use them too.  We do this for all global variables.

bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

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
}

///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main game loop
/////
///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message was to quit
				break;
            TranslateMessage(&msg);						// Find out what the message does
            DispatchMessage(&msg);						// Execute the message
        }
		else											// if there wasn't a message
		{ 
			// Render the scene every frame
			RenderScene();
        } 
	}

	return(msg.wParam);									// Return from the program
}

///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	int i=0;	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix
	
		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is

	// *COLOR*
	// You will notice below that we call a new function called glColor3ub().
	// This stands for (OpenGL Color with 3 parameters using unsigned bytes).
	// That means that, just like in GDI, we pass in the R G and B values from 0 to 255.
	// As you can see, we color the first point RED, then the next GREEN, then BLUE.
	// Coloring each vertex like this makes a cool blend effect.  This is called interpolation.
	// If we want to color from 0.0 to 1.0 we can use glColor3f() (i.e.  glColor3f(1.0f, 0.0f, 0.0f); = RED)
	// Try commenting out the GREEN and BLUE lines.  What do you think will happen?
	// You got it!  It will turn the triangle RED.  That is how you use color.  Pretty simple huh?

	// Below we say that we want to draw triangles
	glBegin (GL_TRIANGLES);								// This is our BEGIN to draw

		glColor3ub(255, 0, 0);							// Make the top vertex RED
		glVertex3f(0, 1, 0);							// Here is the top point of the triangle

		glColor3ub(0, 255, 0);							// Make the left vertex GREEN
		glVertex3f(-1, 0, 0);							// Here is the left point of the triangle

		glColor3ub(0, 0, 255);							// Make the right vertex BLUE
		glVertex3f(1, 0, 0);							// Here is the right point of the triangle
	glEnd();											// This is the END of drawing

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
			GetClientRect(hWnd, &g_rRect);					// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE)	DeInit();				// Quit if we pressed ESCAPE
		break;
 
    case WM_DESTROY:									// If the window is destroyed
        DeInit();										// Release memory and restore settings
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
// The functions that are in this source file are the ones that we will
// be changing on a regular basis.  The functions left in init.cpp are ones
// we will rarely touch, and when we do, it is maybe a line or so.  From now
// on, the tutorials that follow will use this project format.  It will allow
// us to simply focus on the main.cpp with less code, rather than sifting through
// tons of superfluous initialization code.
//
// So, what do you think?  Color is pretty simple huh?  The 2 functions that you
// will want to use is:  glColor3ub() and glColor3f().
//
// You usually want to use glColor3ub() because it works just like GDI, so it might
// be a little more comfortable with you.  On the other hand, if you want more
// precision you will want to use glColor3f().  This makes it so you can use high
// precision floating point numbers like 0.33333f.  This comes in handy for heightmap
// terrain.  Also, you might want to use this because it could be more intuitive to
// think of the color from 0 to 1 because it's like 0 to 100 percent.  The only problem
// with using this is that floating point calculations are always more computationally
// expensive and you want to avoid them when ever you can.
//
// For example, we could actually use glVertex3i() instead of glVertex3f() since we aren't
// using floating point values for our triangle points, but it's rare that we use just
// whole numbers, so I recommend just sticking with glVertex3f() unless you ONLY use
// integer numbers, which is rare, but faster.  If you are loading a model or dealing with
// cameras, you are always dealing with floating point values.  
//
// In the upcoming tutorials we will actually show you how to move around in this world.
// That is when it starts to get REALLY cooOoOoOoOoOool.  
//
// For now, just start messing around with plotting points and changing colors.
//
//
// © 2000-2005 GameTutorials
