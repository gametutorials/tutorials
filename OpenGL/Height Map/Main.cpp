//***********************************************************************//
//																		 //
//		- "Talk to me like a 3 year old!" Programming Lessons -			 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Height Map										 //
//																		 //
//		$Description:	This shows how render a height map from a file.  //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

#include "main.h"										// This includes our main header file
#include "camera.h"										// This is our camera header file

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()
float g_DT;												// Global stores our delta time



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// Here we make an array of bytes to hold our height map data.  Since we are reading
// in a .raw file that just stores values from 0 to 255, we can use them as height values,
// with 255 being the highest point, and 0 being the lowest point.

// This holds the height map data
BYTE g_HeightMap[MAP_SIZE*MAP_SIZE];					

// This tells us if we want lines or fill mode
bool  g_bRenderMode = true;									

// This is our global camera object
CCamera g_Camera;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the application
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// Here we read read in the height map from the .raw file and put it in our
	// g_HeightMap array.  We also pass in the size of the .raw file (1024).

	LoadRawFile("Terrain.raw", MAP_SIZE * MAP_SIZE, g_HeightMap);	

	// Here we set the camera in a obscure position to get a 
	// good outside view of the terrain.  

	// Give our camera a decent starting point in the world
	g_Camera.PositionCamera(1200, 300, 1150,  1199, 300, 1150,  0, 1, 0);

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
		g_DT = desiredFPS; // Set delta time
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
				g_Camera.Update();							// Update the camera data
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// Give OpenGL our camera position
	g_Camera.Look();

	// If we pass the g_HeightMap data into our RenderHeightMap() function it will
	// render the terrain in QUADS.  If you are going to make any use of this function,
	// it might be a good idea to put in an (X, Y) parameter to draw it at, or just use
	// OpenGL's matrix operations (glTranslatef() glRotate(), etc...)

	RenderHeightMap(g_HeightMap);						// Render the height map

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

	case WM_LBUTTONDOWN:								// If the left mouse button was clicked
		
		// We want to be able to turn wire frame on and off, so let's switch it
		g_bRenderMode = !g_bRenderMode;

		// Change the rendering mode to and from lines or triangles
		if(g_bRenderMode) 				
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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:								// Check if we hit the ESCAPE key.
				PostQuitMessage(0);						// Tell windows we want to quit
				break;
		}
		break;
 
    case WM_CLOSE:										// If the window is closed
        PostQuitMessage(0);								// Tell windows we want to quit
        break; 
    } 
 
    return DefWindowProc (hWnd, uMsg, wParam, lParam); 	// Return the default
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Not to much extra going on in this file.  We load and make the call to RenderHeightMap().
// You can turn wire frame mode on and off by clicking the left mouse button. The camera
// controls are as normal.  The mouse and the obvious keyboard keys move around.
// 
// Let's go over the steps that we accomplished during this tutorial: (Explained more in Terrain.h)
// 
// 1) First, we need to read the height map from the .raw file.  This is simple because
//    there is no header to a .raw file, it is just the image bits.  This file format
//    isn't what you generally want to use because you have to either know what the
//    size and type are, or guess, but I thought it fitting for this tutorial.
// 
// 2) After we read our height map data, we then needed to display it.  This was
//    also a simple function because we are just making QUADS with a set size.
//    I chose to do 16 by 16 quads, but you can change this to what ever you want.
//    With our height map array, we treated it as a 2D array and did 2 for loops
//    to draw each quad for each row and column.  Instead of doing lighting, I
//    just gave each vertex a green intensity, depending on it's height.  This makes
//   the terrain look like there is lighting applied.  This also makes it easier to
//    see the definition in the terrain until lighting and texture maps are applied.
//    
// That's it!
// 
// If you want to create your own .raw files, you can either use Photoshop and use the 
// Render->Clouds option, then save it as a .raw format, or use Paint Shop Pro.  There are
// probably many more programs that doing it, but those are the most popular ones.  You can
// also create your own in code, then fwrite() the bytes to a .raw file.  That's all it is.
// If you are interested in learning how to generate random height map values, let me know
// and I will be happy to send you some source code.  The technique is called Perlin Noise.
//
// If you have any feedback or suggestions, please send them.  I am always interested.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
    
