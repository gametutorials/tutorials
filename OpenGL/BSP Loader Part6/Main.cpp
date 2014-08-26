//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		DigiBen@GameTutorials.com			 //
//																		 //
//		$Program:		BSP Loader 6									 //
//																		 //
//		$Description:	This adds gravity, jumping and walking up steps	 //
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
// This is the 6th tutorial of our Quake 3 BSP Loader series.  This is the 3rd
// last version of our BSP collision detection series.  In this version we add
// simple gravity, the ability to jump, as well as walk up stairs and slopes.  Up 
// to this point we go fly around our BSP levels, but in this tutorial we take out 
// movement along the Y-Axis by the view vector.  Now, we will be constantly be 
// pushed to the ground by gravity.  
//
// * How Does Gravity Work? *
//
// This is easy, just keep subtracting a gravity value to the Position.y of the 
// camera (or player position).  Since we will collide with the ground, we just use
// collision detection to not let us go through the floor.  This way of doign gravity 
// is simple of course.  We don't do any complex stuff.  The true equation for gravity
// is:  newY = ut + 1/2gt^2;  which means: 
// 
// newY = initialVelocity * time + 0.5 * gravity * (time * time);
//
// For the simple effect that we are looking for, you don't need to use this rigidly.
// However, we encorporate the "time" into our time base movement with g_FrameInterval.
// A true gravity (acceleration) is 9.8, which is why we chose 9.8 for the gravity.
// 
// * How Does Jumping Work? *
// 
// Jumping is also very simple.  We assign a velocity vector to our camera.  The gravity
// is always adding to this velocity vector.  The camera (or player) position is always
// adding this velocity vector to itself.  When the user presses the space bar, we just 
// set the velocity vector's to a high acceleration, which will continue to be added to the 
// camera position.  Our jump velocity is 4 (kJumpAcceleration).  Now, the camera will
// be adding a Y velocity of 4 every frame.  So won't the player eventually hit the cieling?
// Yes, UNLESS we apply gravity to that velocity vector.  The gravity is not applied
// directly to the camera (or player) position, but subtracted from the velocity vector.
// That means the velocity of 4 that is been added every frame will eventually turn to
// a negative velocity, bringing the player back down to the ground.  Simple huh?
// 
// * Okay, Jumping and Gravity is Cake, But What About This Climbing Steps Business? *
// 
// Hmmmm...  I forgot... but I am sure you'll figure it out.  :) *grin*.  Just kidding.
// This is probably the most complex part of the whole thing.  There are many ways to do
// this, but in order to conform to the simple gravity and jumping implementations, I also
// chose any easy method for the climbing of steps/slopes :).  If you notice in Quake,
// when players go up stairs, they "glide" up the stairs.  This means they don't actually
// climb the stairs, but just create some sort of slope type of a path to glide up the stairs.
// I chose to actaully implement climbing of stairs.
// 
// We have a kMaxStepHeight that is 10.0f.  This means that we can't climb over anything
// that is above 10 units in our world.  When our AABB collides with something in front of
// us, we set a flag (m_bTryStep) that tells us that we can try to see if we can step over
// the obstacle.  We just do a loop going through 1 to kMaxStepHeight (10) which tries to
// move over the object at each respective heights.  If we collide again, then we increase
// the test step height until either we don't collide, or reach our max step height.  If we
// didn't collide, then we move to that height (we can step!).  Simple huh?  You can think
// of it as a blind person going to a step and just kicking up the step until they reached
// the top of the step and they know they can step because they didn't kick the step at
// that height.  Another method that would cut down checks would be to send a verticle ray 
// in from of you down to the bottom of the characters feet (the size of the AABB) and if
// it collided, check the height of the intersection point and see if it's too high for
// you to step.  This is just like a blind person taking a big step with their leg and coming
// down on top of the step.  That should work too :)
// 
// That's it!  Not so bad huh?  Of course there is probably many other ways to do these
// methods, but I'll give you some basic ways to implement these effects into your project,
// then you can reasearch more advance methods that will feet your needs.
// 
// Let's dive into the code!
// 
// 

// This is our velocity vector that is added to our camera position for gravity/jumping
CVector3 g_vVelocity = CVector3(0, 0, 0);

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
	static double lastTime = 0.0f;
	double elapsedTime = 0.0;

	// Get current time in seconds  (milliseconds * .001 = seconds)
	double currentTime = GetTickCount() * 0.001; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
	if( elapsedTime > (1.0 / desiredFrameRate) )
	{
		// Reset the last time
		lastTime = currentTime;	

		// Return TRUE, to animate the next frame of animation
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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

			case VK_SPACE:								// Check if the user hits the space bar

				// This is what will create our jump effect.  Before we add our jump
				// velocity, we make sure that we are on the ground.  This makes it
				// so we can't jump infinitely and keep ramming our head into the cieling.
				// This type of realism depends on your project of course.  If we are on
				// the ground, we can then set our velocity.y value to our jump velocity
				// which will be added to our camera (or player) every frame, until the
				// gravity eventually cancels it out.
				if(g_Level.IsOnGround())
					g_vVelocity.y = kJumpAcceleration;
				break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
// In this file we added a define for our velocity vector.  We also changed the
// camera position to place the camera in a different place of the world and then
// put a higher Y value than the floor so that it has a cool falling effect in
// the beginning of the tutorial.  It shows our sweet gravity effect :) 
// 
// 
// * What's Next? *
//
// When you code it, let us know :) *grin*
// 
// As for the collision detection tutorials for BSP files we are finished.
//
// 
// © 2000-2006 GameTutorials




