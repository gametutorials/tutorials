//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Normals											 //
//																		 //
//		$Description:	This shows how to find the normal of a polygon.  //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file


bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This part below is important.  We create an array of 3 CVector3's.
// We could have called it CPoint3 or CVertex3, but let's call it it CVector3.
// A CVector3 is a class that has 3 floats, x, y, and z.  We will need a class
// to hold 3D points from now on.  We chose to use a class instead of a structure
// because later we will add to this class and make it a robust vector class, but
// for now we just want 3 floats.  We will store our triangle points in this array below.
// We put a 'v' in front of 'Triangle' to show that it is of type CVector3.
// Notice that we initialize the triangle with the left point first, then right, then the top.
// This is important.  We need to stick to this order.  This is important.  I would recommend
// doing it this way, which is counter-clockwise (considering we are looking at (0, 0, 0) down
// the negative Z axis.  This will make sure that our normals are consistent.  When backface
// culling, the default order is counter-clockwise.  This was just picked as a standard.  
// You can change this in OpenGL if you want though.  Check our glFrontFace() and glCullFace()
// in MSDN for more information on back face culling and how to use it.
// I labeled the points below with an '*' to give you more of a visual understanding of the poly.

//												   *
//								*									*

						  // Left Point	    // Right Point   // Top point
CVector3 vTriangle[3] = { {-1,  0,   0},    {1,    0,   0},	 {0,   1,   0} };

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

	// The rest of our initialization would go here (Models, textures, etc...)
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



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix
	
	// We changed the position of the camera so we could better see the normal at an angle. 
	// This positions the camera off to the left a bit and in front of the triangle.
	// We move our view up a bit so fully see the triangle instead looking down

		// 	    Position          View	     Up Vector
	gluLookAt(-2.5f, 0, -0.5f,  0, 0.5f, 0,   0, 1, 0);	// This determines where the camera's position and view is

	
	// Below we draw the triangle like normal, except we pass in our vTriangle structure.
	// This is so we can move the triangle around and now just have it constant.
	// As we move the triangle around, the "normal" will follow it perfectly.

	glBegin (GL_TRIANGLES);								// This is our BEGIN to draw

		glColor3ub(255, 0, 0);							// Make the Left vertex RED
		glVertex3f(vTriangle[0].x, vTriangle[0].y, vTriangle[0].z);

		glColor3ub(0, 255, 0);							// Make the Right vertex GREEN
		glVertex3f(vTriangle[1].x, vTriangle[1].y, vTriangle[1].z);

		glColor3ub(0, 0, 255);							// Make the Top vertex BLUE
		glVertex3f(vTriangle[2].x, vTriangle[2].y, vTriangle[2].z);
	glEnd();											// This is the END of drawing

	// Now that we displayed the triangle, let's display a line coming from the
	// triangle to show you visually what the normal looks like.  Remember, the
	// normal does NOT have a position, just a direction, but just to help us visualize it,
	// we will draw the normal coming from the middle of the triangle.

	// Get the normal of our triangle by passing in our array or points.
	CVector3 vNormal = Normal(vTriangle);

	// Now, just for show, let's find the center of the triangle.
	// We know that the middle X is zero, so we don't need to calculate the X value.
	CVector3 vCenter = {0};

	// To get the Y value, we just divide the top point's Y value by 2, since we start at 0.
	// Do the same thing for the Z value, just divide the Z value of the top point by 2
	// because we start out at 0 on the Z axis, so only the top of the triangle will be moving.
	// If we weren't so conveniently positioned we would have to do something more modular.
	// To find the center of an object you just add up all the X Y and Z values singularly,
	// then divide each one by the number of vertices, and that gives you the center.
	// IE, after you get the total:  total.x /= totalVertices;  total.y /= totalVertices; total.z /= totalVertices;
	// I decided not to make another function for this for simplicity reasons, but you should :)

	vCenter.y = vTriangle[2].y / 2;						// Get the Y value for the center of the triangle
	vCenter.z = vTriangle[2].z / 2;						// Get the Z value for the center of the triangle.

	// Now let's draw the line coming from the center of our triangle.
	// The first point will be the center point of the triangle that we just calculated.
	// The next point will be the center point + the normal of the triangle.

	glBegin (GL_LINES);									// This is our BEGIN to draw

		glColor3ub(255, 255, 0);						// Make the normal line YELLOW
		glVertex3f(vCenter.x, vCenter.y, vCenter.z);	// Let's draw the normal centered on the triangle
														// Draw the normal of the polygon from the center of the polygon to better see it
		glVertex3f(vCenter.x + vNormal.x, vCenter.y + vNormal.y, vCenter.z + vNormal.z);	
	glEnd();											// This is the END of drawing

	// That's it, now use the LEFT and RIGHT arrow keys to move it around to further see it in action.

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground
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

	// Below we check what the user types in.  If they use the arrow keys we move the 
	// tip of the triangle.  Notice that the normal follows the face of the polygon when moved.

	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:								// Check if we hit the ESCAPE key.
				PostQuitMessage(0);						// Tell windows we want to quit
				break;

			case VK_LEFT:								// Check if we hit the LEFT ARROW key.
				vTriangle[2].z -= 0.01f;				// Move the top of the triangle back
				vTriangle[2].y -= 0.01f;				// Move the top of the triangle down
				break;

			case VK_RIGHT:								// Check if we hit the RIGHT ARROW key.
				vTriangle[2].z += 0.01f;				// Move the top of the triangle forward
				vTriangle[2].y += 0.01f;				// Move the top of the triangle up
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
// We didn't do much code in this file, but we added all our math code to 3DMath.cpp.
// This will come in handy because you can just add the .cpp and .h file to the rest
// of your projects.  You will use this and a lot more math almost every app you do.
// At least, any app that actually DOES something besides test OpenGL API's.  If you
// plan to do any lighting or collision you will definitely need this stuff.
//
// If you run this tutorial you can use the LEFT and RIGHT arrow keys to move around
// the top of the triangle so you can see how the normal will follow the triangle.
// If you still don't understand what a normal is used for, go to the next tutorial
// on collision, and it will make more sense.  Also, look over the lighting tutorial. 
//
// Here is what is repeated in 3DMath.cpp:
// 
// Basically, let me explain the steps again to finding the normal of a polygon.
// 
// 1)  First, if you have 3 points of a polygon, you can find a normal to that plane.
//     The first step you need to do is get 2 vectors from that polygon.  That means
//     that you need to pick to sides of the triangle (order is important) and subtract
//     the points from each other to get the vector of that side.  When it comes to triangles,
//     I usually get the first vector from Point3 - Point1. (V1 = P3 - P1)
//     Next, I choose Vector 2 from Point2 and Point1 (V2 = P2 - P1).  
//
// 2)  Now that we have 2 vectors:  V1 and V2, we can take the cross product of those 
//     2 vectors, with vector1 cross vector2.  Remember, the order is important.  
//     I always go counter-clockwise.  Pick and stick!  This is important because it decides the
//     direction of the normal.  It doesn't matter which direction, as long as you do the same
//     thing for all the rest of your polygons.  You don't want mixed up normal directions.
// 
// 3)  Now we should have the normal (Or direction) from the cross product of our polygon.
//     This normal has a strange length, so let's make the length (or distance from the origin) 1.
//     To do this, we take the magnitude (Deals with the length of the vector) of the normal.
//     With the magnitude, we divide the normal BY that magnitude.  That means the X, Y and Z values
//     get divided by the magnitude scalar.  A scalar is a single number like 1, 1.232, etc...
// 
// That's it!  Three basic steps to finding the normal.  It is nice not to have to worry about
// all that once you have a function that does it all for you.  We call ours "Normal()".  It
// won't matter it we have a polygon of 20 vertices, all we need is just 3 points (the first 3)
// from that polygon.  That is enough to define a plane.  Then we find the normal to that plane.
//
// 
// I hope this help!  Let us know at www.GameTutorials.com if this helped you at all.
//
// Good luck, and get ready for the collision tutorial next!
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
