//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Frames Per Second								 //
//																		 //
//		$Description:	Demonstrates calculating the frames per second	 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file

// This code was taken from the Color triangle tutorial.  The tutorial demonstrates
// how to calculate the amount of frames your program renders a second (fps).  There is
// a spinning triangle in the center of the screen to demonstrate something going on.  The
// frames per second are displayed ONLY in window mode in the window's title bar.


bool  g_bFullScreen = true;								// Set full screen as default
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
/////	This function Handles the main game loop
/////
///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

WPARAM MainLoop()
{
	MSG msg;

	while(1)											// Do our infinite loop
	{													// Check if there was a message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
        { 
			if(msg.message == WM_QUIT)					// If the message wasn't to quit:
				break;
            TranslateMessage(&msg);						// Find out what the message does
            DispatchMessage(&msg);						// Execute the message
        }
		else											// if there wasn't a message
		{ 
			RenderScene();								// Render the scene every frame so it calculates the frame rate
        } 
	}

	return(msg.wParam);									// Return from the program
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// CALCULATE FRAMES PER SECOND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function calculates the frames we render each second and displays it to the window title
/////
///////////////////////////////// CALCULATE FRAMES PER SECOND \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CalculateFrameRate()
{
	// Below we create a bunch of static variables because we want to keep the information
	// in these variables after the function quits.  We could make these global but that would
	// be somewhat messy and superfluous.  Note, that normally you don't want to display this to
	// the window title bar.  This is because it's slow and doesn't work in full screen.
	// Try using the 3D/2D font's.  Check out other tutorials which do this later.

	static float framesPerSecond    = 0.0f;							// This will store our fps
    static float lastTime			= 0.0f;							// This will hold the time from the last frame
	static char strFrameRate[50] = {0};								// We will store the string here for the window title

	// Here we get the current tick count and multiply it by 0.001 to convert it from milliseconds to seconds.
	// GetTickCount() returns milliseconds (1000 ms = 1 second) so we want something more intuitive to work with.
    float currentTime = GetTickCount() * 0.001f;				

	// Increase the frame counter
    ++framesPerSecond;

	// Now we want to subtract the current time by the last time that was stored.  If it is greater than 1
	// that means a second has passed and we need to display the new frame rate.  Of course, the first time
	// will always be greater than 1 because lastTime = 0.  The first second will NOT be true, but the remaining
	// ones will.  The 1.0 represents 1 second.  Let's say we got 12031 (12.031) from GetTickCount for the currentTime,
	// and the lastTime had 11230 (11.230).  Well, 12.031 - 11.230 = 0.801, which is NOT a full second.  So we try again
	// the next frame.  Once the currentTime - lastTime comes out to be greater than a second (> 1), we calculate the
	// frames for this last second.
    if( currentTime - lastTime > 1.0f )
    {
		// Here we set the lastTime to the currentTime.  This will be used as the starting point for the next second.
		// This is because GetTickCount() counts up, so we need to create a delta that subtract the current time from.
	    lastTime = currentTime;
		
		// Copy the frames per second into a string to display in the window title bar
		sprintf(strFrameRate, "Current Frames Per Second: %d", int(framesPerSecond));

		// Set the window title bar to our string
		SetWindowText(g_hWnd, strFrameRate);

		// Reset the frames per second
        framesPerSecond = 0;
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
	int i=0;	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix
	
		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we create a static variable that will count forever.  This is used for the rotation degree.
	static float rotY = 0;

	// Pass in our current rotation value to glRotatef() for rotation around the Y axis (0, 1, 0)
	glRotatef(rotY, 0, 1, 0);

	// Increase the rotation by 2 degrees
	rotY += 2;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Call our new function to count and calculate the frames for each second.
	// This needs to be called every time the screen renders so we get a correct frame rate.
	// Because of its static variables inside, we don't need any globals or outside variables.
	CalculateFrameRate();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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
// Pretty easy huh?  This tutorial just shows a simple way to calculate the frames per second.
// The best way to do this would be to use QueryPerformanceCounter().  QueryPerformanceCounter()
// is the most accurate way to check the system clock, but isn't absolutely necessary.
//
// Keep in mind that you don't ideally want to show the frames per second in the title bar.
// This slows down the application a bit, so it is best to use a font that is rendered in the view port.
//
// Another thing to recognize is that OpenGL will only render 60 frames per second if your monitor's MHz
// is set to 60 MHz, etc...  If you want to have higher frames rates you will need to increase your
// monitor frequency.  This is because OpenGL waits for the synchronization of the monitor before rendering
// each frame.  
//
// Once again, a tutorial that every graphics programmer must have.
//
// Enjoy!
//
//
// © 2000-2005 GameTutorials
