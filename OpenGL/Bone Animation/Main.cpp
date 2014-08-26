//********************************************************************************//
//																				  //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -				  //
//																				  //
//		$Author:		DigiBen		digiben@gametutorials.com					  //
//																				  //
//		$Program:		GT Loader												  //
//																				  //
//		$Description:	Loads the GT Skeletal Animation format .GTF				  //
//																				  //
//********************************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "Winmm.lib")

#include "main.h"										// This includes our header file
#include "gtf.h"										// Include the GTF header file.


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is the first tutorial in our Loader series.  This tutorial series is done
// in parallel to the Exporter series.  The Exporter tutorial series is located
// in the OpenGL section of the GameTutorials CD.
//
// * Exporter series associated with this Loader series *
//
// In the Exporter series we show how to create a plugin exporter for Autodesk's® 
// 3D Studio Max®.  In the first Exporter tutorial we show how to export just the 
// vertex information, so in this Loader tutorial we will show how to load those 
// vertices.  The next tutorials will show how to load the material and bone animation 
// data that is exported from the Exporter Part1 and Part2 tutorials.
// 
// * This tutorial built from previous model loading tutorials *
//
// If you have gone through the other model loading tutorials on the CD such as
// .OBJ, .3DS, .ASE, .MD2 and .MD3, then you should be familiar with our model
// loading classes.  Most of the code will come from those previous tutorials
// so we will only be going over the new features and code added.
//
// * What exactly is this Loader loading? * 
//
// We created our own .GTF file format to demonstrate exporting data from 3DS Max.
// In this version we just load a basic header and the vertex data.  We also
// load the vertex indices for our face data.  Since we don't load any texture
// information the model will appear white when we load it.  The next tutorial
// will incorporate UV coordinates and material names.
//
// * GTF file format *
//
// We created a basic file format to save our 3D data to (.GTF).  
// Below is the format:
//
// * "GT3D" (4 characters)
// * Exporter Version (1 float)
// * Number of Objects/Meshes (1 int)
// *	Number of Vertices (1 int)
// *	Vertices ( Number of Vertices * CVector3)
// *	Number of Indices (1 int)
// *	Indices ( Number of Indices * int)
//
// That is what we are going to be loading in this tutorial.  Let's go
// through the code and get the basics down before we get into the more
// complicated bone animation.  Nothing is really new in this file, but
// GTF.cpp/.h and Main.h have all the new additions.
//
// * Application Controls *
//
// ESC   - Quits the program
// W     - Turns wireframe on and off
// LEFT  - Rotates the model left
// RIGHT - Rotates the model right
//
//

char g_szModel[] = "Vick.gtf";			// This is the model that we will be loading
char g_szModelPath[] = "Model";			// This is the directory that the file is located

CModelGTF g_Model;						// This is our global GTF character model instance

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


bool  g_bFullScreen = true;				// Set full screen as default
HWND  g_hWnd;							// This is the handle for the window
RECT  g_rRect;							// This holds the window dimensions
HDC   g_hDC;							// General HDC - (handle to device context)
HGLRC g_hRC;							// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;					// This holds the global hInstance for UnregisterClass() in DeInit()

UINT g_Texture[MAX_TEXTURES] = {0};		// This holds the texture info, referenced by an ID


float g_RotateX		  =    0.0f;		// This is the current value at which the model is rotated
float g_RotationSpeed =    0.1f;		// This is the speed that our model rotates. 
float g_TranslationZ  = -90.0f;		// This stores our distance away from the model
bool  g_RenderMode    =    true;		// This tells us if we are in wire frame mode or not


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes OpenGL and other stuff
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	// Load the .GTF file from the model directory
	bool bResult = g_Model.LoadModel(g_szModelPath, g_szModel);

	glEnable(GL_CULL_FACE);								// Turn back face culling on
	glEnable(GL_TEXTURE_2D);							// Enables Texture Mapping
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
}


///////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function keeps our rendering within a specified frame rate
/////
///////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f;	// Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime;		// Get the slice of time
	float desiredFPS = 1.0f / desiredFrameRate;		// Store 1 / desiredFrameRate

	elapsedTime += deltaTime;						// Add to the elapsed time
	lastTime = currentTime;							// Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		elapsedTime -= desiredFPS;					// Adjust our elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main program loop
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
		else											// If there wasn't a message
		{ 
			if(AnimateNextFrame(60))					// Make sure we only render 60 frames per second
			{
				RenderScene();							// Render the scene
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

	// Give OpenGL our position,			then view,			then up vector
	gluLookAt(	0, 25.5f, g_TranslationZ,	0, 25.5f, 0,			0, 1, 0);
	
	// We want the model to rotate around the axis so we give it a rotation
	// value, then increase/decrease it. You can rotate right or left with the arrow keys.

	glRotatef(g_RotateX, 0, 1.0f, 0);			// Rotate the object around the Y-Axis
	g_RotateX += g_RotationSpeed;				// Increase the speed of rotation

	g_Model.DrawModel();

	SwapBuffers(g_hDC);							// Swap the backbuffers to the foreground
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
    case WM_SIZE:								// If the window is resized
		if(!g_bFullScreen)						// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);		// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:								// If we need to repaint the scene
		BeginPaint(hWnd, &ps);					// Init the paint struct		
		EndPaint(hWnd, &ps);					// EndPaint, Clean up
		break;


	case WM_KEYDOWN:							// If we pressed a key

		switch(wParam)							// Check if we hit a key
		{
			case VK_ESCAPE:						// If we hit the escape key
				PostQuitMessage(0);				// Send a QUIT message to the window
				break;

			case VK_LEFT:						// If the LEFT arrow key was pressed
				g_RotationSpeed -= 0.05f;		// Decrease the rotation speed (eventually rotates left)
				break;

			case VK_RIGHT:						// If the RIGHT arrow key is pressed
				g_RotationSpeed += 0.05f;		// Increase the rotation speed (rotates right)
				break;

			case VK_UP:							// If the UP arrow key was pressed
				g_TranslationZ += 2;			// Move the camera position forward along the Z axis
				break;

			case VK_DOWN:						// If the DOWN arrow key is pressed
				g_TranslationZ -= 2;			// Move the camera position back along the Z axis
				break;

			case 'W':

				g_RenderMode = !g_RenderMode;	// Change the rendering mode

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

    case WM_CLOSE:										// If the window is being closed
        PostQuitMessage(0);								// Send a QUIT Message to the window
        break; 
     
    default:											// Return by default
        lRet = (LONG)DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// Nothing new was added to this file except the file name and paths
// for the model.  Check out Loader.cpp for the changes from our previous
// model loading tutorials.
//
// Be sure to check out the next tutorial that loads texture information.
//
// © 2000-2006 GameTutorials