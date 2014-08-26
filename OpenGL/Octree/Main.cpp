//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Octree											 //
//																		 //
//		$Description:	Demonstrates octree space partitioning			 //
//																		 //
//***********************************************************************//


// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file
#include "camera.h"

// *Tutorial Note* 
// It's assumed that you have read or at least understand the camera and 
// frustum tutorial. If not, we suggest you review it so you can better understand the 
// more important stuff.  Though we do not use the frustum code yet, it is good to know.
//
// This tutorial demonstrates how to create and draw an octree.  What is an octree?
// An octree is tree of nodes that stores vertex information about the scene/level/world.
// The purpose of it is to partition space.  This allows you to check what the camera
// is looking at, then only draw that part of the screen.  If you a huge level or world, 
// you don't want to pass OpenGL the vertices every frame right?  The 100's of thousands of
// vertices would really bring your game to a crawl of .02 frames per second.  It's true
// that OpenGL does some of the clipping for you, but it isn't as fast as you can do it
// yourself.  That might sound weird because OpenGL is a BEAST and it was programmed by
// really intelligent SGI guru's right???  Well, the problem isn't with their code, it's
// the fact that you are doing a for/while loop for thousands of vertices every frame.
// Loops are tremendously slow and are on of the main causes of slow applications.  To
// avoid this problem, we need to divide space into a bunch of nodes, then test if we
// are looking at each one of those nodes.  If we are, then we will want to draw them,
// otherwise ignore them.  The whole beauty of an octree is, that you can specify how
// many vertices/triangles that you want to store in each node.  Also be sure to use
// vertex arrays for fast rendering.  That way you avoid the huge slow down of calling
// glVertex*() functions thousands of times per frame.  They also avoid loops.  We 
// don't use them in this tutorial.
//
// A node is basically a cube that is responsible for a certain amount of vertices.  
// If that cube collides with the frustum (our camera's view) then we render that cube's 
// vertices.  If that cube has children nodes, then it does not have any vertices stored 
// in it, just children that have vertices (or children that have children that store 
// vertices, etc..). This could recurse down forever, except that we have a max level of 
// recursion/subdivision and a maximum amount of triangles stored in each node.  
// Once a node gets below the maximum amount of triangles it stops subdividing.  Now you
// see why this is exponentionally faster than testing every vertex/triangle to see if
// it's in the frustum.  Testing one cube against the frustum is so much faster than
// testing it's 1000 or so triangles.  
//
// The maximum amount of triangles for a node can depend on the size of your scene.  
// If you make it too low it could become slow because you have thousands of nodes.  
// Doing that much recursion every frame could possibly slow down your frame rate.  
// It's always good to start with something like 1000 and then tweak it to find the best 
// result that works for your scene/level/world.  Since our Terrain example doesn't have
// a huge amount of triangles in it, I chose to set the max at 100 to get more subdivisions.
// If you refer to the *Quick Notes* down at the bottom I will explain more of the detail
// on exactly how the octree works and how it is created.  There is also a HTML file that
// should be combined with this tutorial to give a more visual explanation on how it works.
//
// Note that this tutorial is the first octree tutorial of a 3 part series.  We
// do not do any frustum culling yet because I didn't want to complicate the octree lesson.
// The next octree tutorial will involve frustum culling, while the last one of this series
// will load a .3ds file that has actual world with texture information.  This will demonstrate
// how to have a more real world approach to octree's with texture and normals information.
//
// This tutorial currently loads a text file full of vertices.  I chose this because though
// it is not realistic, it is the best way not to over complicate the tutorial.  You can
// also easily add your own vertices to it to test your own stuff.  The .raw file was
// created by simply loading in a .3ds file with our .3ds loader code, then instead
// of calling glVertex3f() to render the vertex, there was a fprintf() to print that
// vertex out to a file.  After the first frame we quit the program.  Then you have a
// huge text file full of vertices.
//
// The controls are: 
//
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


bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This is how fast our camera moves
#define SPEED		0.03f								

// This is the file name that is to be loaded
#define FILE_NAME	"Terrain.raw"						

// Initialize a camera object
CCamera	g_Camera;

// Here we initialize our single Octree object.  This will hold all of our vertices
COctree g_Octree;

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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *



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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// Position our camera to where we want to be looking at initially
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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

				// Render the scene every frame, and also check for camera movement

				g_Camera.Update();						// Update the camera
				RenderScene();							// Render the scene if every frame

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

	// Give OpenGL our camera position
	g_Camera.Look();

	// We want the terrain to rotate around the axis to get a better look of the octree.
	
	glRotatef(g_RotateX, 0, 1.0f, 0);					// Rotate the object around the Y-Axis
	g_RotateX += 0.02f;									// Increase the speed of rotation

	// Here we draw the octree, starting with the root node and recursing down each node.
	// When we get to each of the end nodes we will draw the vertices assigned to them.
	g_Octree.DrawOctree(&g_Octree);

	// Render the cube'd nodes to visualize the octree (in wire frame mode)
	g_Debug.RenderDebugLines();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground

	char strBuffer[255] = {0};							// Create a character buffer

	// To view our octree information I set the window's title bar to the some basic
	// information such as the max triangles per node, the max subdivisions and total end nodes.

	// Display in window mode the current subdivision information
	sprintf(strBuffer, "MaxTriangles: %d     MaxSubdivisions: %d     TotalEndNodes: %d",
		                g_MaxTriangles,		 g_MaxSubdivisions,		 g_EndNodeCount);

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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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
// At first an octree (or any space partitioning) can be intimidating.  Once you
// sit down and start coding it, you become to fully understand the wonderful
// benefits and it's simplicity.  If you can create a tree with nodes, you can
// create an octree.  The only complicated part at first might be the splitting
// of polygons that intersect more than one node.  We don't do that in this tutorial
// so it didn't get to complicated and overhauled with code.  It's way to frustrating
// to learn a ton of things at once, instead of the main idea and basic functionality.
// This octree will work fine, except there will possibly be some doubling of vertices
// because we don't split them into separate one.  That is fine, but not idea for a real
// product.  Eventually you want to fix this.  The last octree tutorial will read in
// a fully textured world that will be split correctly.  Also a more modular octree class
// will be created because we are missing UV coordinates, normals, material ID's, and
// more importantly frustum culling.  The next tutorial will tackle the frustum culling.
//
// Let me explain once more briefly the intent of an octree is.  If you have a huge world
// with 10's of thousands of triangles, you don't want to do a for loop and pass those
// all into OpenGL to be rendered.  You only want to pass in the triangles that you can
// see (the camera's view).  To do this, you want to subdivide your world into cubes
// that holds the triangle data for that area/region of the world.  Then, instead of
// checking if every triangle is in your view, you can just check if a cube intersects
// your frustum, meaning it's in the view of your camera.  This is one of the fastest
// way to do this type of space partitioning.  Just one subdivision puts the world
// into 8 cubes which could cut your triangle rendered count down to 1/8 of what it was.
// If just one subdivision could do that, think about 2, 3 or 4 levels of subdivision?
//
// Let's go over the steps to using this octree class.  Once you have your list of vertices
// you can begin.  The first thing you want to do is call this function:
//
// GetSceneDimensions(g_pVertices, g_NumberOfVerts);
//
// This passes in the list of vertices and the vertex count.  This function calculates
// the center point of the world, as well as the width of the cube that is needed to
// create the first node.  Once we have these values stored, we want to create our tree.
// We can call CreateNode() and pass in the starting data for the first node:
//
// CreateNode(g_pVertices, g_NumberOfVerts, g_Octree.GetCenter(), g_Octree.GetWidth());
//
// This takes the vertices, the vertex count, then the starting center and width.
// This will recurse through and create the whole tree, while assigning the vertices
// to the end nodes.  That's it!  You just created the octree.  The last part is drawing
// the octree.  This is simple, you just call:
//
// DrawOctree();
//
// This should be called in your RenderScene() function.  This goes through all the
// nodes until it gets to the end, then it draws the vertices assigned to those end nodes.
// The deconstructor takes care of the clean up by calling DestroyOctree().
//
// The CDebug class has nothing to do with the octree other than the fact that we use
// it to display the nodes visually.  I suggest you keep something like this around so
// you can visualize your subdivided world and get a better idea of how efficient it is.
//
// Be sure to check out the HTML tutorial on the concept of an octree.  It will give
// good visuals on the whole subdivision process so you can get a better grasp of the
// concepts.  That's about it, so enjoy this tutorial and please let us know what you
// do with it.  We are always curious to know where the knowledge goes. :)
//
// Remember, this tutorial is 1 of a 3 part series.  We still have not delved into the
// frustum culling and polygon splitting part of the octree.  Right now your octree does 
// you no good.  This just explains on how to create an octree.  Check out the second 
// tutorial which focuses on the frustum culling.
//
// Ben Humphrey
// Game Programmer
// DigiBen@GameTutorials.com
// www.GameTutorials.com
//
// © 2000-2005 GameTutorials
