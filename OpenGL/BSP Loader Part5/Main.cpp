//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		BSP Loader 5									 //
//																		 //
//		$Description:	This implements sliding and AABB collision		 //
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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This is the fifth tutorial of our Quake 3 BSP Loader series.  In this version
// we add sliding to our collision detection.  Also, instead of using sphere
// collision we use AABB (Axis-Aligned Bounding Box) collision.
// 
// * What is Sliding? *
// 
// You notice that in the last tutorial had some pretty rouch collision detection.
// When I say rough, I mean that there was virtually no response when the user ran
// into a wall.  All we did was just stop, and we couldn't move any further.  A much
// more fluid and natural way to handle the collision is to produce a sliding effect.
// That means that when the user collides with any geometry, the user will slide off
// in the direction of the view vector (or movement vector).  This way the user doesn't
// need to back up and move another direction after a collision.  This sliding effect
// is what you will see on most First-Person-Shooter games like Quake.
// 
// So how does it work?  Let's go over the math for sliding:
// 
// 1) TheMoveVector = TheEndPoint - TheNewPointFromCollision;
//
//    - So we grab the movement vector, but backwards.  This is because
//      we want to send a vector backwards from the end point.  Of course,
//      we don't want to send the movement vector, we want to send the
//      projected movement onto the normal of the plane back from the end point.
//      Let's do that next:
// 
// 2) DistanceFromEndPointToIntersection = Dot(TheMoveVector, ThePlaneNormal);
// 
//	  - Here we use the dot product between the movement vector and the normal of
//      the plane that we collided with to get the distance that we need to travel
//      backwards along the normal's vector.  So now we have a distance to scooch
//      is backwards to provide the new point we will slide to.  Let's do the last
//      step:
// 
// 3) TheNewSlidingPoint = TheEndPoint - (ThePlaneNormal * DistanceFromEndPointToIntersection);
// 
// That's it!  We got a new position that throws us back from the end point we tried to
// go, to the new position along the planes vector, which creates a sliding effect.  There
// is one more thing we need to do however, and that is we need to test one more collision
// detection call on the new sliding position.  This is because, if we keep sliding into
// some sort of corner, the player will slide right out of the world.  We need to verify that
// that new sliding position is not colliding through the world geometry.  You do this by just
// passing in the new sliding position and and end position into the Trace() function.
// You will see this in action in Quake3Bsp.cpp in our Trace() function.  
//
// Be sure to note that this is just one way to get the sliding position.  There are tons
// of ways in math to get this new slide vector, so use what ever one you life.
// 
// * What is a AABB? *
// 
// AABB stands for Axis-Aligned Bounding Box.  That means that we have a box that is
// aligned with the axis at the origin (XYZ).  The box isn't tilted or rotated in any way.
// We place an AABB around our camera (or character) to test for more accurate collision
// detection in regards to dimensions.  If you have a tall character, you can't really
// represent the character accurately with a sphere, so we use a box to surround the
// character which will act as a better representation of the character's geometry.
// No matter which direction the player is facing, however, the box will never rotate,
// but will be always aligned with the XYZ axis perfectly, standing straight up.
// 
// With this type of simple geometry we can perform collision detection tests quite
// fast, with not to much more overhead than a sphere test.  To calculate a AABB, you
// don't need all of the corners of the box, but just a bottom corner and a top corner
// that is on the opposite side of the bottom corner.  We call this the Min and Max's
// of the AABB.  With just these 2 points, you can see how we can tell the dimensions of
// the box without having all of the corners.
// 
// Just like we did with the sphere, when checking collision we will use a an offset
// for the AABB that will determine if we collided with the brushes (world geometry).
// 
// ***
// 
// We added some code in Camera.cpp to our function where we check for movement.
// In the last tutorial, you'll notice that if we backed ourself into a wall, the
// view vector would eventually eclipse the camera's position and then face the
// camera backwards.  I add now a way to stop the view vector from going backwards
// behind the camera position if you back into a wall and keep moving back.  This
// will fix some strange behavior in the tutorial.
// 
// There wasn't any changes in this file, but you can go over to Quake3Bsp.cpp and
// see the things we added to allow AABB collision and sliding.	In the next tutorial
// we will go over gravity, jumping and going up stairs/slopes.
// 
// Enjoy!
// 
// 

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
	g_Camera.PositionCamera( 80, 320, 55,	80, 320, 155,	0, 1, 0);

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
// Nothing was added to this file for this version.
// 
// * What's Next? *
//
// In the next tutorial we will go over gravity, jumping and going up stairs/slopes.
// Then we will be finished with the collision tutorials for our BSP Loader series.
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//




