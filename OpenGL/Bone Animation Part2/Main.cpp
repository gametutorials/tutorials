//********************************************************************************//
//																				  //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -				  //
//																				  //
//		$Author:		DigiBen		digiben@gametutorials.com					  //
//																				  //
//		$Program:		GT Loader Part 2										  //
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

// This is the second tutorial in the Loader series.  In this version we will
// be loading the material and texture coordinate information.
//
// * Additions *
//
// There isn't a lot of code added to this tutorial, but there is a lot of
// little things that we need to add all over to make it so we can handle the
// texture mapping information for our model.
//
// We first need to add the appropriate variables and lists to our structures
// and classes.  There is now a numOfMaterials in t3DObject since we allow for
// multiple textures per object now.  Lists for the texture coordinates and the
// texture indices were also added to our loader class.  Only one function was 
// added to load the texture-map files: LoadModelTextures().
//
// There are not really any new concepts in this tutorial since it's just basic
// loading and texture mapping.  The only thing that is new is the setup to
// handle ability to have a unique texture for every face if desired.
//
// * GTF file format *
//  
// Below is the new format with the added material data:
//
// * "GT3D" (4 characters)
// * Exporter Version (1 float)
// * Number of Objects/Meshes (1 int)
// *	Number of Materials (1 int)
// *	Material Name ( Number of Materials * char[MAX_FILE_NAME])
// *	Number of Vertices (1 int)
// *	Vertices ( Number of Vertices * CVector3)
// *	Number of Indices (1 int)
// *	Indices ( Number of Indices * int)
// *	Number of UV Coordinates (1 int)
// *	UV Coords ( Number of UV Coords * CVector3)
// *	Number of Texture Indices (1 int)
// *	Texture Indices ( Number of Texture Indices * int)
//
// 
// Nothing new was added to this file.  Please refer to GTF.cpp/.h and
// Main.h for the additions.  The next tutorial will be a bit massive
// tutorial as we go over the bone animation and weighting system.  The
// math needed in order to perform this task might be a bit daunting.
//
// * Application Controls *
//
// ESC   - Quits the program
// W     - Turns wireframe on and off
// LEFT  - Rotates the model left
// RIGHT - Rotates the model right
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


bool  g_bFullScreen = true;				// Set full screen as default
HWND  g_hWnd;							// This is the handle for the window
RECT  g_rRect;							// This holds the window dimensions
HDC   g_hDC;							// General HDC - (handle to device context)
HGLRC g_hRC;							// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;					// This holds the global hInstance for UnregisterClass() in DeInit()

UINT g_Texture[MAX_TEXTURES] = {0};		// This holds the texture info, referenced by an ID

char g_szModel[] = "Vick.gtf";			// This is the model that we will be loading
char g_szModelPath[] = "Model";			// This is the directory that the file is located

CModelGTF g_Model;						// This is our global GTF character model instance

float g_RotateX		  =    0.0f;		// This is the current value at which the model is rotated
float g_RotationSpeed =    0.1f;		// This is the speed that our model rotates. 
float g_TranslationZ  =  -90.0f;		// This stores our distance away from the model
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
// Nothing new was added to this file.  Be sure to check out 
// the next tutorial that loads animation.
//
//
// © 2000-2006 GameTutorials