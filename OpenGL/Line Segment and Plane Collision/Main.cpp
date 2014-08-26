//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		PlaneCollision									 //
//																		 //
//		$Description:	This demonstrates line and plane collision.		 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file


bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This tutorial is adding on the the "Normals" tutorial.
// Below we create our same polygon around the origin.
// This polygon will be the plane we use to check against the line for collision.
// Once again, let me explain what a plane is.  Imagine a piece of paper lying
// flat on a flat surface.  That piece of paper is on a plane.  It goes out forever.
// There are infinate planes.  The flat surface that the piece of paper is resting
// on is on the same plane.  The plane is a horizonal, invisible surface.  In the
// last tutorial we learned about finding the normal to a plane.  The normal is
// a perpendicular vector to that plane, that means it is rising in a direction
// exactly 90 degrees to the plane.  Refer back to last tutorial for a visual of this.
//
//												   *
//								*									*

						  // Left Point	    // Right point	// Top Point     
CVector3 vTriangle[3] = { {-1,  0,     0},  {1,    0,   0}, {0,   1,    0} };

// This is our line that we will be checking against the polygon's plane for collision.
// We position the line going directly through the polygon at first.
CVector3 vLine[2]     = { {0,   0.5f, -0.5f},  {0,   0.5f, 0.5f} };

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

	// This is where all our other initialize would go
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
	
	// Let's set our camera to the left a bit for a better view

		// 	    Position          View	     Up Vector
	gluLookAt(-2.5f, 0.5, 0.5f,  0, 0.5f, 0,   0, 1, 0);	// This determines where the camera's position and view is

	
	// Below we give OpenGL the 3 vertices of our triangle.  Once again, we put them
	// into an array of CVector3 structures so we could dynamically move it around screen.

	glBegin (GL_TRIANGLES);								// This is our BEGIN to draw

		glColor3ub(255, 0, 0);							// Make the Left vertex RED
		glVertex3f(vTriangle[0].x, vTriangle[0].y, vTriangle[0].z);

		glColor3ub(0, 255, 0);							// Make the Right vertex GREEN
		glVertex3f(vTriangle[1].x, vTriangle[1].y, vTriangle[1].z);

		glColor3ub(0, 0, 255);							// Make the Top vertex BLUE
		glVertex3f(vTriangle[2].x, vTriangle[2].y, vTriangle[2].z);
	glEnd();											// This is the END of drawing

	// Below we use our function we just wrote to see if the plane of the
	// triangle and the line intersect.  It will return true if that's the case.

	bool bCollided = IntersectedPlane(vTriangle, vLine);

	// Below we draw the line that the polygon will be colliding with.
	// We will check to see if the line collides with the polygons plane, and if it does, 
	// we will turn the line green to show when it is intersecting the plane.

	glBegin (GL_LINES);									// This is our BEGIN to draw

		if(bCollided)									// If we collided, change the color of the line to illustrate this.
			glColor3ub(0, 255, 0);						// Make the line GREEN if we collided with the triangle's plane
		else
			glColor3ub(255, 255, 0);					// Make the line YELLOW if we didn't collide

		glVertex3f(vLine[0].x, vLine[0].y, vLine[0].z);	// Let's draw the normal centered on the triangle
														// Draw the normal of the polygon from the center of the polygon to better see it
		glVertex3f(vLine[1].x, vLine[1].y, vLine[1].z);	
	glEnd();											// This is the END of drawing

	// That's it, now use the LEFT and RIGHT arrow keys to move it around to see it in action.

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

	// Below we check what the user types in.  If they use the arrow keys
	// then we want to move the triangle around (LEFT RIGHT UP and DOWN keys)

	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:								// Check if we hit the ESCAPE key.
				PostQuitMessage(0);						// Tell windows we want to quit
				break;

			case VK_UP:									// Check if we hit the UP ARROW key.
				vTriangle[0].x += 0.01f;				// Move the left point of the triangle to the left
				vTriangle[1].x += 0.01f;				// Move the right point of the triangle to the left
				vTriangle[2].x += 0.01f;				// Move the top point of the triangle to the left
				break;

			case VK_DOWN:								// Check if we hit the DOWN ARROW key.
				vTriangle[0].x -= 0.01f;				// Move the left point of the triangle to the left
				vTriangle[1].x -= 0.01f;				// Move the right point of the triangle to the left
				vTriangle[2].x -= 0.01f;				// Move the top point of the triangle to the left
				break;


			case VK_LEFT:								// Check if we hit the LEFT ARROW key.
				vTriangle[0].z -= 0.01f;				// Move the left point of the triangle back
				vTriangle[1].z -= 0.01f;				// Move the right point of the triangle back
				vTriangle[2].z -= 0.01f;				// Move the top point of the triangle back
				break;

			case VK_RIGHT:								// Check if we hit the RIGHT ARROW key.
				vTriangle[0].z += 0.01f;				// Move the left point of the triangle forward
				vTriangle[1].z += 0.01f;				// Move the right point of the triangle forward
				vTriangle[2].z += 0.01f;				// Move the top point of the triangle forward
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
// In this tutorial we make a line that turns green if it intersects the plane of
// a polygon.  It turns yellow when there is no intersection.  Use the arrow keys
// to move around the triangle to see when the triangle intersects our line.
// You will notice that it only turns yellow when you move the triangle on the
// left or right of the line.  If you just use up and down, it will still intersect
// the triangle's infinate plane.  The plane doesn't stop at the end of the triangle.
// If we want to do collision JUST with the triangle and not the plane, look at the next
// tutorial on polygon and line collision.
// 
// We now have a IntersectedPlane() function to use anywhere that allows us to find
// if a line is intersecting a plane.  This function is defined in 3DMath.cpp
// 
// Here are the steps explained in 3DMath.cpp to calculate if there is an intersection.
// 
// 1) First we need a polygon (at least 3 points) and a line segment.  We then find
//   the normal of the polygon.
// 
// 2) Once we have the normal, we want to find the distance the plane is from the origin.
//    We use the plane equation (Ax + By + Cz + D = 0) to find the distance.
// 	  We want D, so we turn that into D = -(Ax + By + Cz)
// 
// 3) With the distance, we can now fill in the rest of the plane equation to find
//    the distance a point is from the plane.  So (Ax + By + Cz + D = The distance from the plane)
//    A B C is the plane's normal.X normal.Y normal.Z, and x y z is the point's x y z value.
//    We get the first points distance, then get the second points distance.
// 
//4) If we times the 2 distances of each point together, we either get a positive or negative number.
//    If the number is negative, that means we collided because each distance had to be the opposite value.
//    If the result is positive, that means that the 2 points were on the same side of the plane as each other.
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
       
