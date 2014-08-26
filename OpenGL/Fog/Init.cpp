//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Fog												 //
//																		 //
//		$Description:	This Draws rotating pyramids with adjustable fog.//
//																		 //
//***********************************************************************//

#include "main.h"


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

	// Check if we didn't receive a good return message From the function
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

///////////////////////////////// SET UP PIXEL FORMAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function sets the pixel format for OpenGL.
/////
///////////////////////////////// SET UP PIXEL FORMAT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool bSetupPixelFormat(HDC hdc) 
{ 
    PIXELFORMATDESCRIPTOR pfd; 
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

	glViewport(0,0,width,height);						// Make our viewport the whole window
														// We could make the view smaller inside
														// Our window if we wanted too.
														// The glViewport takes (x, y, width, height)
														// This basically means, what are our drawing boundaries

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

														// Calculate The Aspect Ratio Of The Window
														// The parameters are:
														// (view angle, aspect ration of the width to the height, 
														//  the closest distance to the camera before it clips, 
				  // FOV		// Ratio				//  the farthest distance before it stops drawing).
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height, .5f ,150.0f);

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

    g_hRC = wglCreateContext(g_hDC);					// This creates a rendering context from our hdc
    wglMakeCurrent(g_hDC, g_hRC);						// This makes the rendering context we just created the one we want to use

	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// This function clears the backbuffer and turns it to a desired color.
	// Since we are using blue fog, we want the background color to be blue.
	// The first 3 values are R G B from 0.0 to 1.0 (0 to 255 ratio) with the last an alpha value.

	glClearColor(0, 0, 1, 1);							// Set the background color to blue

	// Below is the initialization of our fog.  First off, we need to choose
	// a fog color.  Since there is a potentional for an RGBA color, we need to
	// give it 4 values.  The first 3 values are the Red Green and Blue color value.
	// The last value is the alpha.  The default is (0, 0, 0, 0) so we want to change that.
	// We put our colors in an array because glFogfv takes them that way.
	// We chose blue for our fog color.

	float fogColor[4] = {0.0f,0.0f,1.0f,1.0f};			// Let's make the Fog Color BLUE

	// Now that we have our 4 colors, let's set the fog mode.  I chose GL_EXP2, but the default
	// is GL_EXP.  There is also a GL_LINEAR.  Try them all to get a feel for the type of fog
	// you are looking for.  They are computed each with different equations.  I think the
	// exponential ones look the best. (f = e^(- density * z)) (Z is the eye or camera position)

	glFogi(GL_FOG_MODE, GL_EXP2);						// Fog Mode

	// We pass in our array to set the fog color
	glFogfv(GL_FOG_COLOR, fogColor);					// Set Fog Color

	// We also have a option to change the fog thickness, or "density".
	// The density is default 1.0, but we will start with 0.2 (g_FogDensity).
	glFogf(GL_FOG_DENSITY, g_FogDensity);				// How Dense Will The Fog Be

	// The "FOG_HINT" is how accurate the fog is calculate.  We don't really care,
	// so we say so :)  GL_DONT_CARE let's openGL choose either per vertex or per pixel fog.
	// Otherwise, we have the option to choose GL_NICEST or GL_FASTEST if desired.
	glHint(GL_FOG_HINT, GL_DONT_CARE);					// The Fog's calculation accuracy

	// This is the important part.  This sets the start of the fog from our screen where 
	// the fog density will effect our world, coming from our camera.  Everything outside 
	// of our START and END will appear the solid color of our fog.  Think of it as our 
	// fall off distance.  I usually set this distance to be the same as our fall off
	// distance (Where OpenGL stops drawing far polys) set in gluPerspective().
	// The default is 0 so we don't need to set it, but you should know how
	glFogf(GL_FOG_START, 0);							// Fog Start Depth

	// This is the END of the start distance.  The default is 1.0, but we want it
	// to go 10 units into the screen.  Remember this is a distance, not an actuall coordinate.
	// Basically, if we have anything past 10 units from the camera it will be covered in fog.
	glFogf(GL_FOG_END, 10.0f);							// Fog End Depth
	
	// Finally, once we initialize all of our fog data, we need to tell OpenGL to turn on fog.
	// To turn fog off, you can call glDisable(GL_FOG);

	glEnable(GL_FOG);									// This enables our OpenGL Fog

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	SizeOpenGLScreen(width, height);					// Setup the screen translations and viewport
}


///////////////////////////////// DE INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function cleans up and then posts a quit message to the window
/////
///////////////////////////////// DE INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DeInit()
{
	if (g_hRC)											
	{
		wglMakeCurrent(NULL, NULL);						// This frees our rendering memory and sets everything back to normal
		wglDeleteContext(g_hRC);						// Delete our OpenGL Rendering Context	
	}
	
	if (g_hDC) 
		ReleaseDC(g_hWnd, g_hDC);						// Release our HDC from memory
		
	if(g_bFullScreen)									// If we were in full screen
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
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
		g_bFullScreen = false;
	
	// Create our window with our function we create that passes in the:
	// name, width, height, any flags for the window, if we want fullscreen of not, and the hInstance.
	hWnd = CreateMyWindow("www.GameTutorials.com - Fog", SCREEN_WIDTH, SCREEN_HEIGHT, 0, g_bFullScreen, hInstance);

	// If we never got a valid window handle, quit the program
	if(hWnd == NULL) return TRUE;

	// INIT OpenGL
	Init(hWnd);													

	// Run our message loop and after it's done, return the result
	return MainLoop();						
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//	
// We initialized our fog in this file in InitializeOpenGL().  It's VERY simple and intuitive.
// Here are the steps to take when generating OpenGL fog:
//
// 1) We need to set the fog equation (How the fog is generated)
//    with this function:
//
//			glFogi(GL_FOG_MODE, GL_EXP2);				// This is an exponentional method (nice)
//
// 2) Next, we need to pass in a array of size [4] that holds our color:
//
//			glFogfv(GL_FOG_COLOR, fogColor);			// Sets Fog Color
//
// 3) After that, we need to set how thick the fog is:
//
//			glFogf(GL_FOG_DENSITY, g_FogDensity);		// How dense the fog will be
//
// 4) Next, we want to set how perfect the fog is.  Unless have a particular preference, do this:
//
//			glHint(GL_FOG_HINT, GL_DONT_CARE);			// Let openGL choose the quality of the fog
//
// 5) We need to give openGL a distance into the screen when the fog starts and stops:
//
//			glFogf(GL_FOG_START, 0);					// Fog Start Depth
//			glFogf(GL_FOG_END, 10.0f);					// Fog End Depth
//	
//	6) Lastly, we just need to turn the ON switch for the fog:
//
//			glEnable(GL_FOG);							// Turns on fog (glDisable(GL_FOG) turns it off)
// 
// That's it!  Fog is pretty straightforward.  Lighting is just as basic.  We will discuss 
// lighting next.
//
// 
// DigiBen
// www.GameTutorials.com
//
//
