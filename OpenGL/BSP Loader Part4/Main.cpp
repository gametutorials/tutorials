//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		BSP Loader 4									 //
//																		 //
//		$Description:	This shows how to check collision with BSP files //
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
bool g_RenderMode = true;
bool g_bTextures  = true;

// These are our defines for the multitexturing functions
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB		 = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = NULL;

// We want to be able to turn lightmaps on and off with the left mouse button
bool g_bLightmaps = true;

// This is our gamma level.  This is read from the config file.
float g_Gamma = 10;

// This is our global frustum class, which is used to cull BSP leafs
CFrustum g_Frustum;

// This will store how many faces are drawn and are seen by the camera
int g_VisibleFaces = 0;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This is the fourth tutorial of our Quake 3 BSP Loader series.  In this version
// we implement collision between the camera and the world.  Yes!  Finally!  You
// can make your game a reality, which cool collision detection.  This is probably
// one of the most difficult things to program in 3D for a game.  Sure, after our
// tutorials you will see that it's not so hard to get the camera colliding with
// the world data, but there is so many little bugs and problems with the collision
// as you create your game that it makes it a real problem for you.  You might even
// find yourself doing a little "walking through walls" in this tutorial series.
// In almost every game you can find some little glitch with the collision.  It's
// hard to track down because it's very rare that it happens.  But, we'll show you
// some general collision detection and then you can adapt it to what you want to
// do with your own project.  The collision that we will do in this first tutorial
// will be just basic running into the walls.  You won't slide off the walls like
// in 3D games, but we will cover "sliding" in the next tutorial with bounding box
// collision detection.
// 
// * What's new in this tutorial? *
//
// Well, we add a couple more structures for brushes in our QuakeBSP.h file.  
// This allows us to load in information about the collision boundaries for
// the world.  These boundaries (or planes) are stored in the "brushes".  We also
// use texture information to check if we are running into a wall or something that is
// transparent like a sprite or water, which we don't want to collide with of course.
// We then do some checks with a bounding sphere and the brushes for the level.
// If our sphere collides with the level data, then we stop and don't allow the user
// to go any further.
//
// * DigiBen, Give it too us simply - How does this collision work? *
//
// Well, if you're gonna be pushy, I'll just get to the juice.  We have a camera
// right?  The camera is stored with just some points/vectors.  You don't want to
// just collide against the world with your camera's position.  Since we are doing
// a third person type of view, we assume we are a character.  That means the
// dimensions of a character are much larger than a single point (unless you're a
// cheater and you create a character that is a single point so no one can shoot
// you).  Usually in games there is a bounding sphere, box or cylinder that surrounds
// the character to quickly check collision against the level and other characters.
// This gets rid of checking all the character's vertices every time.  The fastest
// collision is probably a bounding sphere because it only uses a center point and
// a radius to check against the world.  This is the test we will use in this tutorial.
// The next tutorial will use an AABB (Axis Aligned Bounding Box).  This is much more
// realistic for a character, since characters are usually taller than they are more
// wide.  AABB are still really fast, but require just a bit more code to make work.
// 
// So, anyway... let's explain the collision.  So, we have our position and a radius.
// We are checking the characters sphere against a bunch of planes that are stored
// in the BSP brushes.  These brushes hold the bounding volumes for the world data.
// Well, when the player moves he/she creates a vector: The original position and
// the new position that it will try and move.  This vector can also be called a "ray".
// We just check this ray to see if it collides with any of the brushes' planes.
// If it does collide (meaning that when we pass in the ray start and end points into
// the "plane equation: Ax + By + Cz - distance = 0 : PlaneNormal.x*point.x +
// PlaneNormal.y*point.y + PlaneNormal.z*point.z - PlaneDistanceFromOrigin = 0, but
// instead of 0 it will be a positive number if the point is in front of the plane and 
// a negative number if it's behind the plane - so check if the start and end point
// are oppositve signs and that's a collision), then we can only advance the camera
// enough to be against the plane, but not going through it.  Since we aren't working
// with vertice data, we need to not just check on a plane, but make sure that we are in
// fact inside the brushes' boundaries.  These brushes make up a convex volume, which 
// means that there is no caps in the hull of the brush.  It is a solid object with planes
// that make up that object, like a box.  Since planes are infiniate, we could be 400
// million miles from the actual level data, but be colliding with one of it's brushes.
// To get around this, we need to make sure either the end point is behind all of the
// brushes, or the ray (movement vector) collided with a corner or some other outside
// point of the brush's boundaries.  That is what the whole tutorial is pretty much
// about, doing those calculations.  At first the calculations and tons of checks
// might get overwhelming, but it's better understand on paper if you do an example
// for yourself.
//
// Enough talking about collision, let's look at the actual code.

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Position the camera to the starting point since we have
	// not read in the entities yet, which gives the starting points.
	g_Camera.PositionCamera( 80, 320, 55,	80, 320, 155,	0, 1, 0);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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

	// Calculate our frustum to check the world data again for PVS and Portal Rendering
	g_Frustum.CalculateFrustum();

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

			case VK_F1:									// Check if we hit the space bar
		
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
// 
// Nothing was really added in this file.  The camera position was changed for
// the Level.bsp file.  The biggest addition to this tutorial was series was
// of course in Quake3BSP.cpp file.  The beef of this tutorial is discussed in
// this file.
// 
// * What's Next? *
//
// The next tutorial for the Quake3BSP tutorial series will focus on using
// AABB (axis aligned bounding box) to surround the player.  We will then show
// how to make the player slide along the walls.  The tutorial after that will
// show how to have gravity with jumping and walking up and down stairs/slopes.
// This is what you really want to learn right!?!?!?  I thought so :)
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//




