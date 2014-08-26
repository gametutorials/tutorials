//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		BSP Loader2										 //
//																		 //
//		$Description:	Loads and renders lightmaps for the level		 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
// Keep in mind we need to ignore "libc.lib" in our project settings in VC++ 2005
#pragma comment(lib, "jpeg.lib")
#pragma comment(lib, "winmm.lib")

#include "main.h"								// This includes our main header file
#include "Camera.h"								// Include our camera header file
#include "Quake3Bsp.h"							// Include our quake3 header file

CCamera g_Camera;								// This is our global camera object

bool  g_bFullScreen = true;						// Set full screen as default
HWND  g_hWnd;									// This is the handle for the window
RECT  g_rRect;									// This holds the window dimensions
HDC   g_hDC;									// General HDC - (handle to device context)
HGLRC g_hRC;									// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;							// This holds the global hInstance for UnregisterClass() in DeInit()

UINT g_Texture[MAX_TEXTURES] = {0};				// This holds the texture info, referenced by an ID


// This is our object that loads and handles the .bsp file loading and rendering
CQuake3BSP g_Level;

// These tell us if we are rendering triangles and the textures.
// We added a texture boolean so in the next tutorial you can see
// the difference of just lightmaps or just textures or both.
bool g_RenderMode = true;
bool g_bTextures  = true;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This is the second tutorial of our Quake 3 BSP Loader series.  In this version
// we implement the lightmaps over top of the textures using multitexturing.  If
// your card doesn't support multitexturing, you can change the code to do 2 passes
// instead.  If your card doesn't do multitexturing, you shouldn't be doing 3D
// programming anyway :)  If you can't run the program because you get an error
// with not supporting multitexturing, make sure your resolution and bits per pixel
// are at a normal value.  Some video cards only support certain settings.
//
// So, what is a lightmap?  A lightmap is a second texture that is used to
// apply over a normal texture to create realism.  These texture are precomputed
// in hours and hours of work on the CPU.  The average level renders in about
// 9 to 13 hours.  The result is incredible though.  I use hardware multitexturing,
// so if your card can't support it, either get a decent 15 dollar one, or change
// the code to do 2 passed.  Some cards might complain and say that they don't
// support multitexturing, but try going into full screen mode, or vice versa.
//
// Lightmaps are stored in the actual .bsp file, so we need to extract them out.
// This is done simply by following the lump structure and passing the RGB data
// into a function we create to turn it into a texture map.  Before doing this,
// we change the gamma intensity up about 1000 percent.  If we didn't change the
// gamma, the level would be really dark.  Quake3 actaully messes with the hardware
// to change the gamma, but if you mess with the lightmaps it works out just great.
//
// In our config file, added a new section called [Gamma].  This allows you to change
// the gamma levels by just editing the .ini file.  It will give you an idea of what
// different values look like.  It's currently set at 10 right now, with 1 being
// no change.
//
// If you haven't done any multitexturing before in OpenGL, check out our tutorial
// on the topic at www.GameTutorials.com.  
//
//

// These are our defines for the multitexturing functions
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;

// We want to be able to turn lightmaps on and off with the left mouse button
bool g_bLightmaps = true;

// This is our gamma level.  This is read from the config file.
float g_Gamma = 10;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// INIT WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the application
/////
///////////////////////////////// INIT WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	// Here we open the config file and init some variables
	ifstream fin("Config.ini");
	string strLevel = "";
	string strTemp = "";

	// Check if the file was not found or could not be opened
	if(fin.fail())
	{
		// Display an error message and quit the program if file wasn't found
		MessageBox(g_hWnd, "Could not find Config.ini file!", "Error", MB_OK);
		PostQuitMessage(0);
		return;
	}

	// Read in the name of the level that will be loaded 
	fin >> strLevel >> strLevel;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// Now we need to read in the gamma level for our lightmaps
	fin >> strTemp  >> g_Gamma;

	// Initialize the multitexturing function pointers
	glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)	 wglGetProcAddress("glActiveTextureARB");
    glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)   wglGetProcAddress("glClientActiveTextureARB");

	// Here we make sure that the functions were loaded properly
	if(!glActiveTextureARB || !glClientActiveTextureARB)
	{
		// Display an error message and quit
		MessageBox(g_hWnd, "Your video card doesn't support multitexturing", "Error", MB_OK);
		PostQuitMessage(0);
	}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Close the file
	fin.close();

	// Here we load the level and get the result (true == success)
	bool bResult = g_Level.LoadBSP(strLevel.c_str());

	// Make sure the file was found and we received a success
	if(bResult == false)
	{
		// Quit the application
		PostQuitMessage(0);
		return;
	}

	// Position the camera to the starting point since we have
	// not read in the entities yet, which gives the starting points.
	g_Camera.PositionCamera( 80, 288, 16,	80, 288, 17,	0, 1, 0);

	// Turn on depth testing and texture mapping
	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_TEXTURE_2D);

	// Enable front face culling, since that's what Quake3 does
	glCullFace(GL_FRONT);
 	glEnable(GL_CULL_FACE);
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
		elapsedTime -= desiredFPS; // Adjust the elapsed time

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
				g_Camera.Update();						// Update the camera information
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

void RenderScene() 
{
	int i = 0;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

	// Tell OpenGL where to look from our camera info
	g_Camera.Look();

	// Render the level to the screen
	g_Level.RenderLevel(g_Camera.Position());

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
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are in window mode
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);					// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	case WM_LBUTTONDOWN:								// If we hit the left mouse button
		g_bLightmaps = !g_bLightmaps;					// Turn lightmaps on/off

		if(!g_bLightmaps)								// If we don't want lightmaps
		{	
			glActiveTextureARB(GL_TEXTURE1_ARB);		// Turn the second texture off
            glDisable(GL_TEXTURE_2D);
		}
		break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	case WM_RBUTTONDOWN:								// If we hit the right mouse button
		g_bTextures = !g_bTextures;						// Turn texture mapping on/off

		if(!g_bTextures)								// If we don't want texture mapping
		{
			glActiveTextureARB(GL_TEXTURE0_ARB);
            glDisable(GL_TEXTURE_2D);					// Disable texture mapping
		}
		break;


	case WM_KEYDOWN:

		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;

			case VK_SPACE:								// Check if we hit the space bar

				g_RenderMode = !g_RenderMode;			// Change the rendering mode

				// Change the rendering mode to and from lines or triangles
				if(g_RenderMode) 				
				{
					// Render the triangles in fill mode		
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
				}
				else 
				{
					// Render the triangles in wire frame mode
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
				}
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
// With such a little amount of code to add to our rendered, lightmaps give a
// tremendous improvement to the realism of the level.  All that really needed
// to be done was to load in the lightmaps RGB values, change their gamma levels,
// convert them to a texture map and just assign them as a second pass!
//
// Simple, but Sweeeeeet.
//
// The controls for this tutorial are:
//
// w, s, UP_ARROW, DOWN_ARROW - Move the camera forward and backward
// a, d, RIGHT_ARROW, LEFT_ARROW - Strafe the camera left and right
// Mouse movement - Moves the view for first person mode
// Left  Mouse Button - Turns lightmaps on and off
// Right Mouse Button - Turns textures on and off
// Space Bar - Turns wireframe on and off
// ESCAPE - Quits the program
// 
// Remember to refer back to the HTML file that comes with this tutorial to
// learn more about the file format.  I skipped a bunch of information because
// it's already in that file.
//
// Be sure to include to correct libraries listed at the top and in the read me.
//
// Let me know what you think of this tutorial, I would love to hear your
// feedback and any cool projects you create with it.
//
// Enjoy!
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//




