//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		BSP Loader 3									 //
//																		 //
//		$Description:	Loads bsp tree data and culls portals and leafs	 //
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
#include "Frustum.h"							// Include our frustum culling class

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

// These are our defines for the multitexturing functions
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;

// We want to be able to turn lightmaps on and off with the left mouse button
bool g_bLightmaps = true;

// This is our gamma level.  This is read from the config file.
float g_Gamma = 10;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This is the third tutorial of our Quake 3 BSP Loader series.  In this version
// we implement the PVS (Potential Visibility Set) and frustum culling.  That means
// that we won't need to draw the whole world every frame, but only the world that
// is in our frustum.  To "cull" (not draw) sections of the world, it will take 2  
// parts.  The first part will be checking the PVS sections against the current one
// we are in, then once a visible section is found from our section, we check the
// BSP leaf's bounding box inside of that cluster against the frustum.  If
// it's inside the frustum, then we draw all of those faces assigned to that leaf.
// Be sure to check the HTML Quake 3 Format document that came with this tutorial
// to get more information about how the PVS data is read and works.  This is 
// probably the hardest part about the Quake 3 format to wrap your head around.
// I will try and make it as easy to grasp as possible.
// 
// Basically, when the level is compiled into a .bsp file, the PVS data is 
// precalculated and saved into a large bitset for each cluster.  
// 
// * What's A Cluster? *
// 
// A cluster is essentially a section in the world that contains a bunch of BSP leaf 
// nodes.  There are probably a few names for this, but we will call it a cluster.
// By having clusters that define certain parts of the world, it allows us to cull 
// huge parts of the level.  Each cluster has a list of all the rest of the clusters,
// and if each one can or can't be seen from itself.  For instance, imagine that
// you are in an enclosed room with an open door, and the room constitutes 1 cluster.
// You only need to draw the data in that room, along with the data of the cluster that is
// right outside of door, say in a hallway.  The rest of the world is totally ignored.
// For indoor levels this is incredibly fast.  It allows you to not draw anything
// behind walls and doors and such.  With an octree, you would still be drawing
// data that was behind walls because the nodes bounding boxes would still be in your
// frustum.  You have no way to tell if it can't be seen or not, at least efficiently.
// That is why if you have an outdoor and indoor scene, it's best to try and mix both
// techniques.
//
// Instead of drawing the level by going through all of the faces, we change the rendering
// loop to going through each end leaf in the BSP tree.  We first find the leaf that the
// camera is in by testing the camera position against all of the BSP splitter planes.
// These are all given to us in the .BSP file.  Each leaf has a cluster index given
// to it, so we know which leaf belongs to which cluster.  Once we find the leaf we are in, 
// we then test all the rest of the leaf's clusters against our leaf's cluster to see if
// we even need to consider drawing that leaf and it's faces.  This will make more sense
// once you check out the new code in RenderLevel().
// 
// * What's All This BSP Garbage You Keep Mentioning? *
// 
// If you don't know what a BSP tree is, I suggest looking on the internet for the
// BSP FAQ done by SGI.  It explains every question you might have.  Basically, a
// BSP tree is a Binary Space Partitioning Tree.  The word "binary" denotes that the
// tree only has at most 2 child nodes for ever parent node.  The end nodes, or 
// "leaf nodes" hold the face data, similar to how the octree's end nodes work.  The
// 2 child nodes hold the "front" and "back" nodes.  The BSP also has splitter planes
// associated with each node that were used to determine that nodes front and back nodes.
// The Quake 3 tools create the BSP tree for us, we just need to read in the data.
// Instead of storing pointers to the front back nodes to create a linked list, Quake 3
// stores indices into the node array.  If the index is a negative number, we can then
// convert that index into a leaf index by using the ~ binary operator, or -(i + 1),
// since the first index starts at 0 in the leaf array.
//
// The BSP tree data isn't used so much for rendering as it was in the old Doom I & II
// games, but is used for collision detection.  This will explained later in a future
// tutorial.
//
// There isn't that much new code added to this tutorial (assuming you have already
// seen the frustum culling code before in previous tutorials), so you shouldn't need
// to worry about having a ton of PVS and BSP code being shoved down your throat.
// The Quake 3 tools do most of the work for us, we just need to read in the results.
//
// If you haven't done any frustum culling before in OpenGL, check out our tutorial
// on the topic at www.GameTutorials.com.  
//
//


// This is our global frustum class, which is used to cull BSP leafs
CFrustum g_Frustum;

// This will store how many faces are drawn and are seen by the camera
int g_VisibleFaces = 0;


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
				g_Camera.Update();						// Update the camera info
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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Since we are using frustum culling to only draw the visible BSP leafs,
	// we need to calculate the frustum every frame.  This needs to happen
	// right after we position our camera.  Now the frustum planes can be defined.
	g_Frustum.CalculateFrustum();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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

	case WM_LBUTTONDOWN:								// If we hit the left mouse button
		g_bLightmaps = !g_bLightmaps;					// Turn lightmaps on/off

		if(!g_bLightmaps)								// If we don't want lightmaps
		{	
			glActiveTextureARB(GL_TEXTURE1_ARB);		// Turn the second texture off
            glDisable(GL_TEXTURE_2D);
		}
		break;

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
// Not much was added to this file for this tutorial version.  We did need to
// calculate the frustum each frame, and we also added the visible faces to our
// window's title bar. That way as you move throughout the world you can see how
// much culling is being performed.
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
// © 2000-2005 GameTutorials
//




