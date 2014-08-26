//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		Triangle										 //
//																		 //
//		$Description:	Init OpenGL and draw a triangle to the screen	 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h>										// Header File For The OpenGL32 Library
#include <gl\glu.h>										// Header File For The GLu32 Library

#define SCREEN_WIDTH 800								// We want our screen width 800 pixels
#define SCREEN_HEIGHT 600								// We want our screen height 600 pixels
#define SCREEN_DEPTH 16									// We want 16 bits per pixel

bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

// The window proc which handles all of window's messages.
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// This is our main rendering function prototype.  It's up here for now so MainLoop() can call it.
void RenderScene();

///////////////////////////////// CHANGE TO FULL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This changes the screen to FULL SCREEN
/////
///////////////////////////////// CHANGE TO FULL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void ChangeToFullScreen()
{
	DEVMODE dmSettings;									// Device Mode variable

	memset(&dmSettings,0,sizeof(dmSettings));			// Makes Sure Memory's Cleared

	// Get current settings -- This function fills in our settings.
	// This makes sure NT and Win98 machines change correctly.
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dmSettings))
	{
		// Display error message if we couldn't get display settings
		MessageBox(NULL, "Could Not Enum Display Settings", "Error", MB_OK);
		return;
	}

	dmSettings.dmPelsWidth	= SCREEN_WIDTH;				// Selected Screen Width
	dmSettings.dmPelsHeight	= SCREEN_HEIGHT;			// Selected Screen Height
	
	// This function actually changes the screen to full screen.
	// CDS_FULLSCREEN gets rid of the start Bar.
	// We always want to get a result from this function to check if we failed.
	int result = ChangeDisplaySettings(&dmSettings,CDS_FULLSCREEN);	

	// Check if we didn't receive a good return message from the function
	if(result != DISP_CHANGE_SUCCESSFUL)
	{
		// Display the error message and quit the program
		MessageBox(NULL, "Display Mode Not Compatible", "Error", MB_OK);
		PostQuitMessage(0);
	}
}

///////////////////////////////// CREATE MY WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a window, but doesn't have a message loop
/////
///////////////////////////////// CREATE MY WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, bool bFullScreen, HINSTANCE hInstance)
{
	HWND hWnd;
	WNDCLASS wndclass;
	
	memset(&wndclass, 0, sizeof(WNDCLASS));				// Init the size of the class
	wndclass.style = CS_HREDRAW | CS_VREDRAW;			// Regular drawing capabilities
	wndclass.lpfnWndProc = WinProc;						// Pass our function pointer as the window procedure
	wndclass.hInstance = hInstance;						// Assign our hInstance
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// General icon
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);		// An arrow for the cursor
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);	// A white window
	wndclass.lpszClassName = "GameTutorials";			// Assign the class name

	RegisterClass(&wndclass);							// Register the class
	
	if(bFullScreen && !dwStyle) 						// Check if we wanted full screen mode
	{													// Set the window properties for full screen mode
		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ChangeToFullScreen();							// Go to full screen
		ShowCursor(FALSE);								// Hide the cursor
	}
	else if(!dwStyle)									// Assign styles to the window depending on the choice
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	g_hInstance = hInstance;							// Assign our global hInstance to the window's hInstance

	// Below, we need to adjust the window to it's true requested size.  If we say we
	// want a window that is 800 by 600, that means we want the client rectangle to
	// be that big, not the entire window.  If we go into window mode, it will cut off
	// some of the client rect and stretch the remaining which causes slow down.  We fix this below.

	RECT rWindow;
	rWindow.left	= 0;								// Set Left Value To 0
	rWindow.right	= width;							// Set Right Value To Requested Width
	rWindow.top	    = 0;								// Set Top Value To 0
	rWindow.bottom	= height;							// Set Bottom Value To Requested Height

	AdjustWindowRect( &rWindow, dwStyle, false);		// Adjust Window To True Requested Size

														// Create the window
	hWnd = CreateWindow("GameTutorials", strWindowName, dwStyle, 0, 0,
						rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
						NULL, NULL, hInstance, NULL);

	if(!hWnd) return NULL;								// If we could get a handle, return NULL

	ShowWindow(hWnd, SW_SHOWNORMAL);					// Show the window
	UpdateWindow(hWnd);									// Draw the window

	SetFocus(hWnd);										// Sets Keyboard Focus To The Window	

	return hWnd;
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
			if(msg.message == WM_QUIT)					// If the message was to quit
				break;
            TranslateMessage(&msg);						// Find out what the message does
            DispatchMessage(&msg);						// Execute the message
        }
		else											// If there wasn't a message
		{ 
			// Do computationally expensive things here.  We want to render the scene
			// every frame, so we call our rendering function here.  Even though the scene
			// doesn't change, it will bottle neck the message queue if we don't do something.
			// Usually WaitMessage() is used to make sure the app doesn't eat up the CPU.
			RenderScene();
        } 
	}

	return(msg.wParam);									// Return from the program
}

///////////////////////////////// SET UP PIXEL FORMAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets the pixel format for OpenGL.
/////
///////////////////////////////// SET UP PIXEL FORMAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd = {0}; 
    int pixelformat; 
 
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);			// Set the size of the structure
    pfd.nVersion = 1;									// Always set this to 1
														// Pass in the appropriate OpenGL flags
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
    pfd.dwLayerMask = PFD_MAIN_PLANE;					// We want the standard mask (this is ignored anyway)
    pfd.iPixelType = PFD_TYPE_RGBA;						// We want RGB and Alpha pixel type
    pfd.cColorBits = SCREEN_DEPTH;						// Here we use our #define for the color bits
    pfd.cDepthBits = SCREEN_DEPTH;						// Depthbits is ignored for RGBA, but we do it anyway
    pfd.cAccumBits = 0;									// No special bitplanes needed
    pfd.cStencilBits = 0;								// We desire no stencil bits
 
	// This gets us a pixel format that best matches the one passed in from the device
    if ( (pixelformat = ChoosePixelFormat(hdc, &pfd)) == FALSE ) 
    { 
        MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
	// This sets the pixel format that we extracted from above
    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE;										// Return a success!
}

//////////////////////////// RESIZE OPENGL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function resizes the viewport for OpenGL.
/////
//////////////////////////// RESIZE OPENGL SCREEN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void SizeOpenGLScreen(int width, int height)			// Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero error
	{
		height=1;										// Make the Height Equal One
	}

	glViewport(0,0,width,height);						// Make our viewport the whole window.
														// We could make the view smaller inside
														// Our window if we wanted too.
														// The glViewport takes (x, y, width, height).
														// This basically means, what are our drawing boundaries
														// on the screen in client coordinates (you can have like 4
														// viewports rendering different things in one window, like 3DS Max.

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

														// Calculate The Aspect Ratio Of The Window
														// The parameters are:
														// (view angle, aspect ratio of the width to the height, 
														//  the closest distance to the camera before it clips, 
				  // FOV		// Ratio				//  the farthest distance before it stops drawing).
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, 1 ,150.0f);

	// * Note * - The farthest distance should be at least 1 if you don't want some
	// funny artifacts when dealing with lighting and distance polygons.  This is a special
	// thing that not many people know about.  If it's less than 1 it creates little flashes
	// on far away polygons when lighting is enabled.

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

///////////////////////////////// INITIALIZE GL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles all the initialization for OpenGL.
/////
///////////////////////////////// INITIALIZE GL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void InitializeOpenGL(int width, int height) 
{  
    g_hDC = GetDC(g_hWnd);								// This sets our global HDC
														// We don't free this hdc until the end of our program
    if (!bSetupPixelFormat(g_hDC))						// This sets our pixel format/information
        PostQuitMessage (0);							// If there's an error, quit

	// We need now to create a rendering context AFTER we setup the pixel format.
	// A rendering context is different than a device context (hdc), but that is
	// what OpenGL uses to draw/render to.  Because OpenGL can be used on
	// Macs/Linux/Windows/etc.. It has it's on type of rendering context that is
	// The same for EACH operating system, but it piggy backs our HDC information.
    g_hRC = wglCreateContext(g_hDC);					// This creates a rendering context from our hdc
    wglMakeCurrent(g_hDC, g_hRC);						// This makes the rendering context we just created the one we want to use.

	SizeOpenGLScreen(width, height);					// Setup the screen translations and viewport
}

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

	// *Hint* We will put all our game/program init stuff here.
	// Some things include loading models, textures and network initialization.
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
	glLoadIdentity();									// Reset The View
	
		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is

	// The position has an X Y and Z.  Right now, we are standing at (0, 0, 6).
	// The view also has an X Y and Z.  We are looking at the center of the axis (0, 0, 0).
	// The up vector is 3D too, so it has an X Y and Z.  We say that up is (0, 1, 0).
	// This can pretty much stay the same, unless you start rotating your position around the
	// x or z-axis.  For moving left, right and forward you should be okay to leave it at (0,1,0).
														// Below we say that we want to draw triangles:		
	glBegin (GL_TRIANGLES);								// This is our BEGIN to draw
					glVertex3f(0, 1, 0);				// Here is the top point of the triangle
		glVertex3f(-1, 0, 0);	glVertex3f(1, 0, 0);	// Here are the left and right points of the triangle
	glEnd();											// This is the END of drawing

	// I arranged the functions like that in code so you could visualize better
	// where they will be on the screen.  Usually they would each be on their own line.
	// The code above draws a triangle to those points and fills it in.
	// You can have as many points inside the BEGIN and END, but it must be in three's.
	// Try GL_LINES or GL_QUADS.  Lines are done in 2's and Quads done in 4's.

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground
}

///////////////////////////////// DE-INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function cleans up and then posts a quit message to the window
/////
///////////////////////////////// DE-INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DeInit()
{
	if (g_hRC)											
	{
		wglMakeCurrent(NULL, NULL);						// This frees our rendering memory and sets everything back to normal
		wglDeleteContext(g_hRC);						// Delete our OpenGL Rendering Context	
	}
	
	if (g_hDC) 
		ReleaseDC(g_hWnd, g_hDC);						// Release our HDC from memory
		
	if(g_bFullScreen)									// If we were in full screen:
	{
		ChangeDisplaySettings(NULL,0);					// Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	UnregisterClass("GameTutorials", g_hInstance);		// Free the window class

	PostQuitMessage (0);								// Post a QUIT message to the window
}


///////////////////////////////// WIN MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles registering and creating the window.
/////
///////////////////////////////// WIN MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	HWND hWnd;

	// Check if we want full screen or not
	if(MessageBox(NULL, "Click Yes to go to full screen (Recommended)", "Options", MB_YESNO | MB_ICONQUESTION) == IDNO)
		g_bFullScreen = FALSE;
	
	// Create our window with our function we create that passes in the:
	// name, width, height, any flags for the window, if we want fullscreen of not, and the hInstance.
	hWnd = CreateMyWindow("www.GameTutorials.com - First OpenGL Program", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	// If we never got a valid window handle, quit the program
	if(hWnd == NULL) return TRUE;

	// INIT OpenGL
	Init(hWnd);													

	// Run our message loop and after it's done, return the result
	return MainLoop();						
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
		if(!g_bFullScreen)								// Don't worry about this if we are in full screen (otherwise may cause problems)
		{												// LoWord=Width, HiWord=Height
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
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
// (Be sure to include opengl32.lib glu32.lib in your Link tab
//  of the menu Project->Settings. These are libraries that OpenGL needs to use.
//  You might have to download them from www.opengl.org.
//  You can check out our video tutorials on how to include libraries).
//
// That's all there is to initializing OpenGL!
// Now that you can display a triangle to the screen,
// you can go onto many other cool things from here.
// A lot of this (most of it) might be confusing the first time.
// The only stuff you should REALLY understand is the RenderScene() function.
// This function is what draws everything.  The rest of the stuff is init junk
// that you will very rarely ever look at again.  You might add a line here
// and there, but mostly it stays the same.  In the next tutorials, we will
// eventually just stick it in another .cpp file so we can just focus on the
// important stuff that we will most likely be changing a lot.  This will take
// the stress of being overwhelmed from the other stuff.  Like I said,
// if you can understand that you have a camera, and it points in a direction,
// and that you plot points to make a triangle, you are good for now :)
//
// Here are the basic steps to initializing OpenGL.
//
// 1) If you want to go full screen, give the option to.  Granted, the full
//    screen code doesn't have anything to do with OpenGL, but I would always
//    encourage giving a full screen option.  It goes faster when it's in full
//    screen mode, which helps out on slower computers.
//
// 2) Create our window with the full screen information.  We need to check if
//    the user wants full screen first because our window properties depend on it.
//
// 3) Next, we need to setup the pixel format.  This allows us to use openGL in 
//    our window.  We can also specify double buffering, if we want to draw to the
//    window, or a bitmap, and if we want to allow GDI function calls.  This is
//    necessary if we want to do anything with OpenGL.  You will most likely never
//    need to change the code in SetupPixelFormat().  It's a one time coded thing :)
//
// 4) After setting the pixel format, we need to create and set a rendering context.
//    This is done with the 2 functions wglCreateContext() and wglCreateContext().
//
// 5) Lastly, we need to setup our screen translations.  If we didn't use OpenGL 
//    this part would be complicated and tricky.  Luckily, OpenGL keeps us from
//	  all the math needed to do the translations.  We just tell OpenGL how large
//    our viewport is (usually the whole screen) and which perspective we want to look
//	  at in our world. After that, we are all set and we just need to DRAW SOMETHING!
//
// We plotted 3 points in this tutorial.  3 points make up a triangle.
// We used the GL_TRIANGLES flag to tell OpenGL to render triangles.
// 
// Like so (cleaner):
//
//	glBegin (GL_TRIANGLES);	
//		glVertex3f(0, 1, 0);	// Top point			
//		glVertex3f(-1, 0, 0);	// Bottom right point
//		glVertex3f(1, 0, 0);    // Bottom left point
//	glEnd();
//
//  If we wanted 2 triangles, we could say:
//
//	glBegin (GL_TRIANGLES);	
//		glVertex3f(0, 1, 0);	// Top point			
//		glVertex3f(-1, 0, 0);	// Bottom right point
//		glVertex3f(1, 0, 0);    // Bottom left point
//
//		glVertex3f(0, 1, 1);	// Top point			
//		glVertex3f(-1, 0, 1);	// Bottom right point
//		glVertex3f(1, 0, 1);    // Bottom left point
//	glEnd();
//
// Notice that we rendered the points in a different order than
// in the RenderScene() function.  In our first tutorial we won't
// get too much into the order, but just know that there is a
// clockwise and counter clockwise way to draw triangles.  In our
// RenderScene() function we used counter clockwise, because we
// draw the left side before the right side of the triangle.
// Above we draw using clockwise.  Usually you will find that
// many people will use clockwise.  You can tell OpenGL not to
// render the back's of triangles (speeds up rendering).  This
// is called Z-Buffering.  You can set in OpenGL either CW
// (ClockWise) or CCW (Counter ClockWise).  Don't worry about
// that too much right now though.
//
//  Here is a horrible attempt to draw the 3D axis's
//
//				Y
//				|
//				|
//				|________ X
//				 \
//				   \
//					Z
//
//  This is drawn in a rotated manner so you can see the Z axis.
//  Otherwise it would be coming out of the screen and you couldn't see it.
//  Positive Z comes out of the screen towards us, where -z goes into the screen.
//  Positive X is on the right, negative X is on the left.
//  Positive Y goes straight up, where negative Y goes straight down
//
//  So here is what our triangle looks like with the axis:
//				Y
//				|
//			   /|\
//			  /_|_\______ X
//				 \
//				   \
//					Z
//
// As you can see, our triangle is half on the negative X side.
// and half on the positive X side.  It is also 0 on the Z axis,
// so really, it's seen as 2D more than 3D because we
// aren't using the third dimension, which is Z.
//
// The next tutorial will be a lot smaller in comments because I will
// chop off all the stuff that I already explained, so then we can just focus
// on the RenderScene() function, rather than all the Init stuff. (Blah!)
//
// Once again, if it seems overwhelming, it will get easier and MUCH more fun! 
//
// As for messing around with this tutorial, try changing camera positions
// to see how it affects your view.  Also, try plotting in other points and
// other triangles/lines/Quads (rectangles) to see what you can create so far.
//
// One last note about this code.  It might be a good idea, if you don't
// understand anything about 3D or matrices (matrix math),  go on the internet
// and do a little investigating and learning.  It couldn't hurt.  We will teach you
// all the main math you will need to know, but the more versed you are in math the better.
// If you are not good at math, or at least you don't know anything over algebra/geometry,
// you are going to get a little frustrated.  You will want to be pretty comfortable
// with cosine and sine, as well as plane and vector math.  You will want to know what
// a vector is, and what purpose it serves. 
//
// However, don't get scared if you are not a math person, I wasn't really when I first started
// doing 3D programming, but you learn REALLY fast, and it's fun when you use math
// to do something cool.  Yah, you heard me, math can be fun!
//
// 3D programming is by FAR the coolest thing to program.
//
//
// © 2000-2006 GameTutorials
