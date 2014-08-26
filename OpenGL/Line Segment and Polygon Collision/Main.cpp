//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		PolygonCollision								 //
//																		 //
//		$Description:	This demonstrates line and polygon collision.	 //
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

//  This tutorial is built from the "PlaneCollision" tutorial.  Instead of just
//  colliding with a plane, we now can do better collision that checks the whole polygon.
//  You wouldn't do much yet with just plane collision, but now you can actually make
//  somewhat of a decent maze or what ever you want.  There is a lot of math explained
//  in this tutorial, but hopefully it is explained enough so you understand what is going
//  on.  In the next tutorials it will be further explained, as well as online papers will
//  be written with pictures to give a better illustration.  We didn't change much in this file.

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


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	int i=0;	

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix
	
	// Set our camera to the left a bit for a better view

		// 	    Position          View	     Up Vector
	gluLookAt(-2.5f, 0.5, 0.5f,  0, 0.5f, 0,   0, 1, 0);	// This determines where the camera's position and view is

	// Draw our same triangle from the last tutorial.

	glBegin (GL_TRIANGLES);								// This is our BEGIN to draw

		glColor3ub(255, 0, 0);							// Make the Left vertex RED
		glVertex3f(vTriangle[0].x, vTriangle[0].y, vTriangle[0].z);

		glColor3ub(0, 255, 0);							// Make the Right vertex GREEN
		glVertex3f(vTriangle[1].x, vTriangle[1].y, vTriangle[1].z);

		glColor3ub(0, 0, 255);							// Make the Top vertex BLUE
		glVertex3f(vTriangle[2].x, vTriangle[2].y, vTriangle[2].z);
	glEnd();											// This is the END of drawing


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Now, instead of just testing against the plane, we take it a step further
	// and test if we actually hit the polygon.  This is a more usable collision.
	// We give our function the polygon, the line to test with, and the number of vertices of our polygon
	
	bool bCollided = IntersectedPolygon(vTriangle, vLine, 3);

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


	// Below we draw the line that the polygon will be colliding with. (Green is collision)

	glBegin (GL_LINES);									// This is our BEGIN to draw

		if(bCollided)									// If we collided, change the color of the line to illustrate this.
			glColor3ub(0, 255, 0);						// Make the line GREEN if we collided with the triangle's plane
		else
			glColor3ub(255, 255, 0);					// Make the line YELLOW if we didn't collide

		glVertex3f(vLine[0].x, vLine[0].y, vLine[0].z);	// Let's draw the normal centered on the triangle
														// Draw the normal of the polygon from the center of the polygon to better see it
		glVertex3f(vLine[1].x, vLine[1].y, vLine[1].z);	
	glEnd();											// This is the END of drawing

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
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

	// Below we check what the user types in.  If they use the arrow keys
	// then we want to move the triangle around (LEFT RIGHT UP and DOWN keys)

	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:								// Check if we hit the ESCAPE key.
				PostQuitMessage(0);						// Tell windows we want to quit
				break;

			case VK_UP:									// Check if we hit the UP ARROW key.
				vTriangle[0].x += 0.01f;				// Move the left point of the triangle away from you
				vTriangle[1].x += 0.01f;				// Move the right point of the triangle away from you
				vTriangle[2].x += 0.01f;				// Move the top point of the triangle to away from you
				break;

			case VK_DOWN:								// Check if we hit the DOWN ARROW key.
				vTriangle[0].x -= 0.01f;				// Move the left point of the triangle towards you
				vTriangle[1].x -= 0.01f;				// Move the right point of the triangle towards you
				vTriangle[2].x -= 0.01f;				// Move the top point of the triangle to towards you
				break;


			case VK_LEFT:								// Check if we hit the LEFT ARROW key.
				vTriangle[0].z -= 0.01f;				// Move the left point of the triangle to the left
				vTriangle[1].z -= 0.01f;				// Move the right point of the triangle to the left
				vTriangle[2].z -= 0.01f;				// Move the top point of the triangle to the left
				break;

			case VK_RIGHT:								// Check if we hit the RIGHT ARROW key.
				vTriangle[0].z += 0.01f;				// Move the left point of the triangle to the right
				vTriangle[1].z += 0.01f;				// Move the right point of the triangle to the right
				vTriangle[2].z += 0.01f;				// Move the top point of the triangle to the right
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
// In this file we just replaced the call to IntersectedPlane() with IntersectedPolygon()
// and allowed the user to use the PAGE UP and PAGE DOWN keys to further move around the triangle.
// This gives you more ways to test the polygon intersection.  
// Now the triangle can move along the Y axis by using PAGE UP and PAGE DOWN.
// 
// Let's go over the material once more in a brief overview. (Explained more in 3DMath.h)
// 
// 1) Once you find out that the line and the plane intersect, you need to get the intersection
//    point.  In order to get the intersection point, we needed to learn about the dot product.
//    The basic premis of getting the intersection point is finding the distant from a point on
//    the line, either end is fine, then moving from that point along the lines vector.  But,
//    we can't just directly move across that distance, because that is the distance from the plane,
//    it doesn't mean that it's the actual point of intersection.  Take the case that the line is
//    almost parallel with the plane, but is slightly tilted so it intersects a ways down.  Well,
//    the distance from the plane would be very short, where the distance to the intersection point
//    is considerably longer.  To solve this problem, we divide the negated distance by the dot product
//    of the normal and the lines vector.  This then gives us the correct distant to the intersection point.
//
// 2) Once we find the intersection point, we need to test if that point is inside of the polygon.
//    just because we collide with the plane, doesn't mean that we collided with the polygon.
//	  Planes are infinate, so the polygon could be hundred of coordinate units from us.  To test
//    to see if we are inside of the polygon, we create vectors from the intersection point to
//    each vertice of the polygon.  Then, as we do this, we calculate the angle between the vectors.
//    We create 2 vectors at a time, which then create a triangle.  We only care about the inner angle.
//    After we are finished adding up the angles between each vector of the polygon, if the angles
//    add up to 360 degrees, then the point is inside of the polygon.  We create a function called
//    AngleBetweenVectors() which gives us the angle between 2 vectors in radians.  So the angles
//    need to add up to at least 2 * PI.  To get the angle between 2 vectors we found out that
//    if we use this equation (V . W / || V || * || W || ) which is the dot product between
//    vector V and vector W, divided by the magnitude of vector V multiplied by the magnitude of vector W.
//    We then take the arc cosine of that result and it gives us the angle in radians.  If we
//    are working with unit vectors (vectors that are normalized with length of 1) we don't need
//    to do the || V || * || W || part of the equation, but that is the TRUE dot product equation.
//
// 3) After we coded those last 2 steps, we put them into a usable function called IntersectedPolygon().
//    It's simple to use, just pass in an array that makes up the polygon, pass in the line array, 
//    then the vertice count of the polygon.  
//
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
