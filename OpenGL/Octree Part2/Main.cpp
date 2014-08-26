//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Octree2	 										 //
//																		 //
//		$Description:	Intergrates frustum culling with an octree		 //
//																		 //
//***********************************************************************//


// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our main header file
#include "camera.h"										// This includes our camera header file


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
// 
// By reading this tutorial we assume you have looked at the previous octree tutorial,
// as well as the frustum culling tutorial.  If not we strongly recommend it.
//
// This is the second tutorial of the 3 part series on octrees.  The last tutorial
// focused on creating the octree.  This tutorial adds to the previous tutorial
// by adding the frustum culling.   
//

// The controls are: 

// Left Mouse Button   - Changes the Render mode from normal to wire frame.
// Right Mouse Button  - Turns lighting On/Off
// Left  Arrow Key | A - Rotates the camera left
// Right Arrow Key | D - Rotates the camera right
// Up    Arrow Key | W - Moves the camera along the view vector
// Down  Arrow Key | S - Moves the camera opposite the view vector
// F5				   - Increases the max triangle count by 20 for each node
// F6				   - Decreases the max triangle count by 20 for each node
// +				   - Increases the max subdivision count
// -				   - Decreases the max subdivision count
// Escape - Quits
//
//
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()
float g_DT;												// This stores the elapsed time between the current and last frame

// This is the file name that is to be loaded
#define FILE_NAME	"Terrain.raw"						

// Initialize a global camera object
CCamera	g_Camera;

// Here we initialize our single Octree object.  This will hold all of our vertices
COctree g_Octree;

// This will be our global frustum object that keeps track of our frustum
CFrustum g_Frustum;

// We created a debug class to show us the nodes visually in yellow cubes.
CDebug g_Debug;

// This will store an array of 3D points that we will read from a file.
CVector3 *g_pVertices = NULL;

// This will store the amount of vertices read in from the text file (.raw)
int g_NumberOfVerts = 0;

// The maximum amount of triangles per node.  If a node has equal or less 
// than this, stop subdividing and store the verts in that node
int g_MaxTriangles = 100;

// The maximum amount of subdivisions allowed (Levels of subdivision)
int g_MaxSubdivisions = 0;

// The current amount of end nodes in our tree (The nodes with vertices stored in them)
int g_EndNodeCount = 0;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This stores the amount of nodes that are in the frustum
int g_TotalNodesDrawn = 0;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// Set the current render mode to triangles initially
bool g_bRenderMode = true;

// Turn lighting on initially
bool  g_bLighting     = true;							

// This stores the variable that increases the rotation angle for our terrain (not important)
float g_RotateX = 0;



///////////////////////////////// LOAD VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function loads the vertices of our terrain from a text file (Terrain.raw)
/////
///////////////////////////////// LOAD VERTICES \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void LoadVertices()
{
	// This function reads in the vertices from an ASCII text file (terrain.raw).
	// First, we read in the vertices with a temp CVector3 to get the number of them.
	// Next, we rewind the file pointer and then actually read in the vertices into
	// our allocated CVector3 array g_pVertices[].

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
	g_pVertices = new CVector3 [ g_NumberOfVerts ];

	// Go back to the beginning of the file so we can store the vertices now
	rewind(fp);

	// Create a counter for the index of the g_pVertices[] array
	int index = 0;

	// Read in the vertices that are stored in the file
	for(int i = 0; i < g_NumberOfVerts; i++)
	{
		// Read in the current vertice and at the end add 1 to the index
		fscanf(fp, "%f %f %f\n", &g_pVertices[ index ].x, 
								 &g_pVertices[ index ].y, 
								 &g_pVertices[ index ].z);

		index++;				// Increase our index for the vertex list
	}

	// Close our file because we are done
	fclose(fp);
}


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the game window.
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	// Position our camera
	g_Camera.PositionCamera(0, 30, 120,	0, 0, 0,	0, 1, 0);

	// This loads the vertices for the terrain
	LoadVertices();

	// Calculate the bounding box of our scene.  This will give us a width of
	// the cube that is needed to surround the whole world.  We want to pass in
	// the vertices and the vertex count into this function to find the farthest point
	// from the center of the world.  That will then be our width.  Depending on the
	// world this doesn't always surround it perfectly. In the next tutorial we will fix that.
	g_Octree.GetSceneDimensions(g_pVertices, g_NumberOfVerts);

	// Here we pass in the information to create the root node.  This will then
	// recursively subdivide the root node into the rest of the node.
	g_Octree.CreateNode(g_pVertices, g_NumberOfVerts, g_Octree.GetCenter(), g_Octree.GetWidth());

	// Here, we turn on a light and enable lighting.  We don't need to
	// set anything else for lighting because we will just take the defaults.
	// We also want color, so we turn that on too.  We don't load any normals from
	// our .raw file so we will calculate some simple face normals to get a decent
	// perspective of the terrain.

	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color
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
		g_DT = desiredFPS;
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
				g_Camera.Update();						// Update the camera
				RenderScene();							// Render the scene every frame
			}
			else
			{
				Sleep(1);								// Let other processes work
			}
		} 
	}

	// Here we free our vertice data that was loaded from our .raw file
	delete [] g_pVertices;

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

	// Give OpenGL our camera position
	g_Camera.Look();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Each frame we calculate the new frustum.  In reality you only need to
	// calculate the frustum when we move the camera.
	g_Frustum.CalculateFrustum();

	// Initialize the total node count that is being draw per frame
	g_TotalNodesDrawn = 0;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Here we draw the octree, starting with the root node and recursing down each node.
	// When we get to each of the end nodes we will draw the vertices assigned to them.
	g_Octree.DrawOctree(&g_Octree);

	// Render the cube'd nodes to visualize the octree (in wire frame mode)
	g_Debug.RenderDebugLines();

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground

	char strBuffer[255] = {0};							// Create a character buffer

	// To view our octree information I set the window's title bar to the some basic
	// information such as the max triangles per node, the max subdivisions, 
	// total end nodes and the total drawn end nodes that are currently in the frustum.

	// Display in window mode the current subdivision information
	sprintf(strBuffer, "MaxTriangles: %d     MaxSubdivisions: %d     TotalEndNodes: %d       TotalNodesDraw: %d",
		                g_MaxTriangles,		 g_MaxSubdivisions,		 g_EndNodeCount,		 g_TotalNodesDrawn);

	// Set our window title bar to the subdivision data
	SetWindowText(g_hWnd, strBuffer);
}



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// RECREATE OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function destroys the octree and debug lines then creates a new one
/////
///////////////////////////////// RECREATE OCTREE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RecreateOctree()
{
	// You will not need this function for the octree.  It is just for the tutorial
	// every time we change our subdivision information.

	g_EndNodeCount = 0;						// Reset the end node count

	g_Debug.Clear();						// Clear the list of debug lines
	g_Octree.DestroyOctree();				// Destroy the octree and start again

	// Get the new scene dimensions since we destroyed the previous octree
	g_Octree.GetSceneDimensions(g_pVertices, g_NumberOfVerts);
				
	// Create our new octree with the new subdivision information
	g_Octree.CreateNode(g_pVertices, g_NumberOfVerts, g_Octree.GetCenter(), g_Octree.GetWidth());
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



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
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

	case WM_LBUTTONDOWN:								// If we hit the left mouse button
		
		// Change the rendering mode to lines or triangles depending on it's current status
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

	case WM_RBUTTONDOWN:								// If the right mouse button was clicked.
		
		g_bLighting = !g_bLighting;						// Turn lighting ON/OFF

		if(g_bLighting) {								// If lighting is ON
			glEnable(GL_LIGHTING);						// Enable OpenGL lighting
		} else {
			glDisable(GL_LIGHTING);						// Disable OpenGL lighting
		}
		break;

	case WM_KEYDOWN:									// If we hit a key
		switch (wParam)									// Check which key we hit
		{
			case VK_ESCAPE:								// If we hit ESCAPE
				PostQuitMessage(0);						// Tell windows we want to quit
				break;

			case VK_ADD:								// If we hit '+'
				g_MaxSubdivisions += 1;					// Increase the current max subdivisions

				// Make sure we don't go over 10 subdivisions, otherwise it will get crazy
				if(g_MaxSubdivisions  > 10) g_MaxSubdivisions   = 10;
				
				// Destroy the octree and debug lines and create a new one with the new info
				RecreateOctree();
				break;

			case VK_SUBTRACT:							// If we hit '-'
				g_MaxSubdivisions -= 1;					// Decrease the current max subdivisions

				// Make sure we don't go below zero subdivisions
				if(g_MaxSubdivisions < 0) g_MaxSubdivisions   = 0;
				
				// Destroy the octree and debug lines and create a new one with the new info
				RecreateOctree();
				break;

			case VK_F5:									// If we hit F5
				g_MaxTriangles += 20;					// Increase the max triangle count by 20

				// Make sure we don't go above 2000
				if(g_MaxTriangles > 2000) g_MaxTriangles  = 2000;
				
				// Destroy the octree and debug lines and create a new one with the new info
				RecreateOctree();
				break;

			case VK_F6:									// If we hit F6
				g_MaxTriangles -= 20;					// Decrease the max triangle count by 20

				// Make sure we don't go below 0 (0 would produce as many nodes as triangles)
				if(g_MaxTriangles < 0) g_MaxTriangles  = 0;

				// Destroy the octree and debug lines and create a new one with the new info
				RecreateOctree();
				break;
		}
		break;

    case WM_CLOSE:										// If the window is closed
        PostQuitMessage(0);								// Tell windows we want to quit
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
// This tutorial was an add on to the last octree tutorial which just showed us
// how to create an octree.  There wasn't a lot added, but the power that is provides
// is incredible.  We now have a functional octree that only draws the vertices in
// our view.  There are 2 things we added to this tutorial since the last:
// 
// 1) We added the frustum code (frustum.cpp and frustum.h).  This is all explained
// in our frustum tutorial on our site if you don't understand the code.  Once we
// create our global CFrustum object: g_Frustum, we just need to call this every frame:
// 
// g_Frustum.CalculateFrustum();
// 
// We only really need to call it when the camera moves but you can do those checks 
// yourself.  Then, once the frustum planes are calculated we can use this function to
// check if the end nodes are in our frustum:
//
// g_Frustum.CubeInFrustum(center.x, center.y, center.z, cubeWidth / 2);
// 
// That's it for the frustum!  Simple, yet wonderfully usefull.  We left in the sphere
// and point code in frustum.cpp from the frustum tutorial just so if you wanted to
// include the frustum.cpp file in your application/game you didn't have to copy and
// paste them back in.  Just ingore those for this tutorial though.
// 
// 2) Finally we added a counter (g_TotalNodesDrawn) to tell us how many of the end 
// nodes are being drawn.  This let's us know how well the octree is working.  You 
// can view this information in the window's title bar, along with the other subdivision 
// information.
// 
// Also notice that we took our the rotating.  This is so all the cubes stay axis aligned.
//
// Hopefully by breaking the octree tutorial up into multiple parts it isn't so hard
// to digest.  The next tutorial will focus on reading in a real world with texture
// information, normals, etc...  Then you can actually use this code in a project.
// I also included the HTML octree tutorial with this tutorial as well just in case
// some people didn't want to download the first octree tutorial but wanted to get right
// to the good stuff.  This HTML file will explain in more detail the octree theory, etc...
// 
// Good luck!
// 
//
// Ben Humphrey
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials
