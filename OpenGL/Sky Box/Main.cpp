//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		SkyBox											 //
//																		 //
//		$Description:	This shows how to create a textured sky box		 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
#pragma comment(lib, "winmm.lib")						// Added for timeGetTime()
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


#include "main.h"										// This includes our header file


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
#include "camera.h"										// This includes our camera class header
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()
float g_DT;												// This stores the elapsed time between the current and last frame


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// This tutorial shows you how to create a sky box.  A sky box is just a 3D box that
// has textures assigned to it which create an illusion that you are actually in
// a intricate world.  This is great because it it's fast to draw and looks cool in
// the background.  The textures used in this tutorial were created by Nick Coombe at
// http://www.planethalflife.com/crinity  | email: crinity@email.com.  I appreciate
// his artistic talent and for letting me use these for this tutorial and on our CD.
// How this works is, we create a box, and then assign the top, bottom, left, right,
// back and front textures to each side of the cube accordingly.  This will make it
// look like we are in a cool world, not an ugly cube :).  I struggled with adding
// camera support to this tutorial, but eventually gave in.  Though it might complicate
// an easy tutorial I figured there is no way to really get the effect without being
// able to look around.  You can use the mouse to look around, and the arrow keys
// to move within the sky box.  This tutorial was taken partly from the RotateCube
// tutorial.  I grabbed the camera and texture code from the TextureMap and Camera3 tuts.

// Below we create the new needed variables for this tutorial

#define SPEED	0.03f									// This is how fast our camera moves

// These defines correspond to an index within the g_Texture[] array.
// We will use these defines to bind the appropriate texture to the
// right side of the box.

#define BACK_ID		0									// The texture ID for the back side of the cube
#define FRONT_ID	1									// The texture ID for the front side of the cube
#define BOTTOM_ID	2									// The texture ID for the bottom side of the cube
#define TOP_ID		3									// The texture ID for the top side of the cube
#define LEFT_ID		4									// The texture ID for the left side of the cube
#define RIGHT_ID	5									// The texture ID for the right side of the cube

UINT g_Texture[MAX_TEXTURES] = {0};						// This holds the texture info, referenced by an ID

CCamera g_Camera;										// This is our global camera object

///////////////////////////////// CREATE SKY BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This creates a box centered around X Y Z.  Instead of colors at
/////   each vertices there is texture maps for each side of the box to 
/////   create the illusion of the sky box.  You can set the width, height and length
/////   of the sky box to create different perspectives for different sky box textures.
/////
///////////////////////////////// CREATE SKY BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateSkyBox(float x, float y, float z, float width, float height, float length)
{
	// This is the most important function of this tutorial.  This function
	// used to just create a silly colored cube in the RotateCube tutorial,
	// but now it creates something beautiful.  You'll notice we added
	// some more parameters to the function.  This way we can change the perspective
	// of the sky box.  It doesn't really look good if it's a perfect cube.  Some
	// textures look better at different ratios.  We assign the sky box textures
	// to each side of the box creating the illusion of a detailed 3D world.
	// You will notice I had to change the texture coordinates for every one
	// to be flipped correctly.  Also, depending on your culling, the vertex
	// order might need to be changed around.  I don't use culling in this tutorial
	// so it will work fine here, but be sure to remember this if you do.

	// Bind the BACK texture of the sky map to the BACK side of the cube
	glBindTexture(GL_TEXTURE_2D, g_Texture[BACK_ID]);

	// Since we want the sky box to be centered around X, Y, and Z for ease,
	// we do a little math to accomplish this.  We just change the X, Y and Z
	// to perform this task.  If we just minus half the width, height and length
	// from x, y and z it will give us the desired result.  Now when we create the
	// box it will center it around (x, y, z)

	// This centers the sky box around (x, y, z)
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

		// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the BACK Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z);
		
	glEnd();

	// Bind the FRONT texture of the sky map to the FRONT side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[FRONT_ID]);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);	
	
		// Assign the texture coordinates and vertices for the FRONT Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z + length);
	glEnd();

	// Bind the BOTTOM texture of the sky map to the BOTTOM side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[BOTTOM_ID]);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
	
		// Assign the texture coordinates and vertices for the BOTTOM Side
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
	glEnd();

	// Bind the TOP texture of the sky map to the TOP side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[TOP_ID]);
	
	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the TOP Side
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height, z);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);
		
	glEnd();

	// Bind the LEFT texture of the sky map to the LEFT side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[LEFT_ID]);
	
	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the LEFT Side
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x,			y + height,	z);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x,			y,			z);		
		
	glEnd();

	// Bind the RIGHT texture of the sky map to the RIGHT side of the box
	glBindTexture(GL_TEXTURE_2D, g_Texture[RIGHT_ID]);

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		

		// Assign the texture coordinates and vertices for the RIGHT Side
		glTexCoord2f(0.0f, 0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f, 1.0f); glVertex3f(x + width, y + height,	z);
	glEnd();
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Below we read in the 6 texture maps used for the sky box.
	// The ID's are important, so don't mix them up.  There is a
	// texture for every side of the box.

	CreateTexture(g_Texture[BACK_ID], "Back.bmp");
	CreateTexture(g_Texture[FRONT_ID], "Front.bmp");
	CreateTexture(g_Texture[BOTTOM_ID], "Bottom.bmp");
	CreateTexture(g_Texture[TOP_ID], "Top.bmp");
	CreateTexture(g_Texture[LEFT_ID], "Left.bmp");
	CreateTexture(g_Texture[RIGHT_ID], "Right.bmp");

	// Give our camera a decent starting point in the world
	g_Camera.PositionCamera( 0, 0, 0,	0, 0, 1,	0, 1, 0);

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
				g_Camera.Update();						// Update the camera data			
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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

	// Here we create our sky box.  It will be centered around (0, 0, 0) with
	// a width of 400, height of 200 and length of 400.  You might wonder why we
	// don't make it a perfect cube?  Well, if we make the height half the size
	// of the width and length it looks better.  Try putting the height to 400 and
	// you will notice that everything looks really close up.  You will want to tweak
	// this ratio most likely for every sky maps you use.  Some look better at different
	// perspectives and ratios.  Try changing the height to 100.  It looks more flat
	// and at a greater distance.  

	// Draw the sky box centered at (0, 0, 0)
	CreateSkyBox(0, 0, 0, 400, 200, 400);

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
    LONG    lRet = 0; 
    PAINTSTRUCT    ps;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
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
 
    case WM_DESTROY:									// If the window is destroyed
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
// This tutorial shows you how to create a sky box.  A sky box is just a box that
// has textures assigned to it which create an illusion that you are actually in
// a intricate world.  This is great because it it's fast to draw and looks cool in
// the background.  There are a lot of these textures all over the web.
// How this works is, we create a box, and then assign the top, bottom, left, right,
// back and front textures to each side of the box accordingly.  This will make it
// look like we are in a cool world, not an ugly box :).  I struggled with adding
// camera support to this tutorial, but eventually gave in.  Though it might complicate
// an easy tutorial I figured there is no way to really get the effect without being
// able to look around.  You can use the mouse to look around, and the arrow keys
// to move within the sky box.  This tutorial was taken partly from the RotateCube
// tutorial.  I grabbed the camera and texture code from the TextureMap and Camera3 tuts.
//
// One IMPORTANT thing when using textures for sky maps is that
// the default GL_TEXTURE_WRAP_S and ""_WRAP_T texture property is GL_REPEAT.
// We need to turn this to GL_CLAMP_TO_EDGE, otherwise it creates ugly seems
// in our sky box.  GL_CLAMP_TO_EDGE does not repeat when bound to an object.
// To set this texture property we added these 2 lines to CreateTexture();
//
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//
// The textures used in this tutorial were created by Nick Coombe at
// http://www.planethalflife.com/crinity  | email: crinity@email.com.  I appreciate
// his artistic talent and for letting me use these for this tutorial and on our CD.
// Check out his site, it is REALLY cool.  There are tons more of these textures.
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
