//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		CameraWorldCollision							 //
//																		 //
//		$Description:	Shows how to check if camera and world collide	 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

#include "main.h"										// This includes our main header file

CCamera g_Camera;										// This is our global camera object

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
//
// This tutorial was taken from the Camera3 tutorial (First person camera), and mixed in
// with the sphere - polygon collision tutorial.  The purpose of this application is to
// show colliding and sliding into the walls with sphere collision for the camera and the world.
// If we surround our camera with an invisible radius, say for instance 1, we can then
// check our sphere with every polygon that is near us.  We don't do any world subdivision
// here because we already have so few triangles to check, but this is essential in a game.
// You either want to choose a BSP tree or an octree, depending on your world/level/scene.
// If you haven't looked at the sphere to polygon collision tutorial, I highly recommend it.
// We will not be explaining the concepts of sphere collision in depth as we did in the 
// previous sphere collision tutorial, so it's important that you already understand the
// backbone behind all of this collision code.
// 
// The world data we will be checking against is stored in a text file called "World.raw".
// Only the vertices are stored, with one vertex on each line.  I created the world in 3D
// Studio Max, then exported it to this simple format.  That way you don't need to dig through
// the trenches of model loading code.  I wanted something to walk around in, big enough that
// I didn't want to code the vertices by hand.  I had textures on the walls and floor, but
// in the end decided not to complicate the code any more.  We actually use a cool trick to
// give our scene some depth, without coloring vertices.  If you turn on fog with GL_EXP2
// used as the filter, it creates a radiosity look to the scene (assuming your video card
// supports fog).  Without colors or textures we accomplish putting decent depth to our scene.
//
//

// This is our world file that holds the vertices
#define FILE_NAME "World.raw"

// This will hold the number of vertices that were found in "World.raw"
int g_NumberOfVerts = 0;

// This is our 3D point array that will store all of the world vertices
CVector3 *g_vWorld=NULL;


///////////////////////////////// LOAD VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads the vertices of our terrain from a text file (Terrain.raw)
/////
///////////////////////////////// LOAD VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void LoadVertices()
{
	// This function reads in the vertices from an ASCII text file (World.raw).
	// First, we read in the vertices with a temp CVector3 to get the number of them.
	// Next, we rewind the file pointer and then actually read in the vertices into
	// our allocated CVector3 array g_vWorld[].

	// Create a file pointer and load the model from a file of vertices
	FILE *fp = fopen(FILE_NAME, "r");

	// Make sure we opened the file correctly and it was found
	if(!fp) 
		MessageBox(NULL, "Can't Open File", "Error", MB_OK);

	// Create a temporary variable to hold what we read in from the file
	CVector3 vTemp;

	// Read in the number of vertices that are stored in the file
	while(1)
	{
		// Read in a vertice and get the return value. If it's EOF we are done reading
		int result = fscanf(fp, "%f %f %f\n", &vTemp.x, &vTemp.y, &vTemp.z);
														

		// If we hit End Of File then we are done reading in the vertices, break
		if(result == EOF) 
			break;

		// Increase the vertice count
		g_NumberOfVerts++;
	}

	// Allocate the needed memory to hold the vertices
	g_vWorld     = new CVector3 [ g_NumberOfVerts ];

	// Go back to the beginning of the file so we can store the vertices now
	rewind(fp);

	// Create a counter for the index of the g_vWorld[] array
	int index = 0;

	// Read in the vertices that are stored in the file
	for(int i = 0; i < g_NumberOfVerts; i++)
	{
		// Read in the current vertice and at the end, then add 1 to the index
		fscanf(fp, "%f %f %f\n", &g_vWorld[ index ].x, 
								 &g_vWorld[ index ].y, 
								 &g_vWorld[ index ].z);
											 
		index++;				// Increase our index for the vertex list
	}

	// Close our file because we are done
	fclose(fp);
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Init our camera position

						// Position        View		   Up Vector
	g_Camera.PositionCamera(10, 4, 12,   9, 4, 12,   0, 1, 0);

	// We need to specify our camera's radius in the beginning, I chose 1.
	g_Camera.SetCameraRadius(1);

	// This loads the vertices from the file into our global world array
	LoadVertices();

	// We are going to enable backface culling.  If you are unfamiliar with
	// this term, it basically means that we don't draw 2 side polygons.
	// The only side drawn is the side with the normal coming out of it.
	// by enabling backface culling, it cuts down our fill rate bottleneck.
	// Since our world is composed of 2D boxes, we will never see the inside of
	// the boxes, so there is no need to draw them.  We can change GL_BACK to
	// GL_FRONT for an interesting effect.
	glCullFace(GL_BACK);								// Don't draw the back sides of polygons
	glEnable(GL_CULL_FACE);								// Turn on culling

	glClearColor(0.0, 0.0, 0.0, 1);						// Turn the background color black

	float fogColor[4] = {0.0, 0.0, 0.0, 1.0f};			// Let's make the Fog Color black too

	glFogi(GL_FOG_MODE, GL_EXP2);						// Set The Fog Mode
	glFogfv(GL_FOG_COLOR, fogColor);					// Set The Fog Color
	glFogf(GL_FOG_DENSITY, 0.045f);						// Set How Dense Will The Fog Be
	glHint(GL_FOG_HINT, GL_DONT_CARE);					// Set The Fog's calculation accuracy
	glFogf(GL_FOG_START, 0);							// Set The Fog's Start Depth
	glFogf(GL_FOG_END, 50.0f);							// Set The Fog's End Depth

	glEnable(GL_FOG);									// Enable fog (turn it on)

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	ShowCursor(false);									// Hide the cursor (NEW)
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
				g_Camera.Update();						// Update the camera's movement and view
				RenderScene();							// Render the scene every frame
			}
			else
			{
				Sleep(1);								// Let other processes work
			}
		} 
	}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Before the program ends, we need to free the world data that was allocated
	delete [] g_vWorld;									

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// To calculate our collision detection with the camera, it just takes one function
	// call from the client side.  We just pass in the vertices in the world that we
	// want to check, and then the vertex count.  
	g_Camera.CheckCameraCollision(g_vWorld, g_NumberOfVerts);

	// Position our camera
	g_Camera.Look();

	// Since we have the vertices for the world in the correct order, let's create
	// a loop that goes through all of the vertices and passes them in to be rendered.

	glBegin(GL_TRIANGLES);								// This is our BEGIN to draw the polys

		// Go through all the vertices and draw them
		for(int i = 0; i < g_NumberOfVerts; i += 3)
		{
			glVertex3f(g_vWorld[i].x, g_vWorld[i].y, g_vWorld[i].z);
			glVertex3f(g_vWorld[i+1].x, g_vWorld[i+1].y, g_vWorld[i+1].z);
			glVertex3f(g_vWorld[i+2].x, g_vWorld[i+2].y, g_vWorld[i+2].z);
		}
	
	glEnd();											// This is the END of drawing		
	
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
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this is we are not in full screen
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
// As you'll notice, there wasn't much we did in this file, except get the vertices
// from the file and pass them in to be drawn.  This is the beauty of our camera
// collision function, CheckCameraCollision().  All we have to provide it with is an array
// of vertices and the vertex count.  It then does the rest.
//
// So where do you go from here?  Try making a level with incline.  See if you can
// add some simple gravity to make you fall off if you go over the edge.  Also try
// implementing jumping.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//

