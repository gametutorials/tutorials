//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		Matrices										 //
//																		 //
//		$Description:	Shows a basic introduction to OpenGL Matrices.	 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file


bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()


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
			RenderScene();								// Render the scene every frame
		} 
	}

	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// * ///////



///////////////////////////////// DRAW TRIANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function draws a colored triangle around (x, y, z).
/////   The size of this triangle depends on the width and height passed in.
/////
///////////////////////////////// DRAW TRIANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawTriangle(float x, float y, float z, float width, float height)
{
	// Now we can draw a triangle very easily with a certain position.
	// This will come in handy in our RenderScene() function so as not to clutter it.
	// We would rather focus on the matrix code that we are going to learn about.

	// The three points of our triangle are created from the variables passed in.
	// We will make a triangle that is centered around (x, y, z) and with a width and height
	// of the passed in width and height, but on each side, and top and bottom of the triangle.
	// If we wanted to make it a TRUE width and height, we would divide the width and height by 2.
	// i.e. glVertex3f(x + width/2, y - height/2, z);  - But it's not necessary to convey the point.

	// Below we say that we want to draw triangles
	glBegin (GL_TRIANGLES);								// This is our BEGIN to draw

		glColor3ub(255, 0, 0);							// Make the top vertex RED
		glVertex3f(x, y + height, z);					// Here is the top point of the triangle

		glColor3ub(0, 255, 0);							// Make the left vertex GREEN
		glVertex3f(x + width, y - height, z);			// Here is the right point of the triangle

		glColor3ub(0, 0, 255);							// Make the right vertex BLUE
		glVertex3f(x - width, y - height, z);			// Here is the left point of the triangle
	glEnd();											// This is the END of drawing

}



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// * ///////



///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix
	
		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// * ///////

	// Below we will be introduced to using matrices.  Basically what they are, is they
	// hold the rotations, translations, and scaling for the world.  
	// When you call glPushMatrix(), you are now using a different matrix than the previous
	// matrix.  That means that if you call a glTranslatef() or glRotatef() after you
	// pushed on a new matrix, it only effects the polygons that are drawn in the scope of
	// that matrix.  Then, when you are finished with a certain group of polygons, then you
	// call glPopMatrix() and that pops that current matrix off the stack and goes back to
	// the previous matrix being used.  Initially, we use the main world matrix.  If we push
	// on another matrix, and then start doing some rotations and translations or scaling, it
	// doesn't do those operations to the polygons drawn OUTSIDE of that matrix scope.  At the
	// bottom of this file, Look in *Quick Notes* for more explanation on matrices and what they
	// look like.  The nice thing about OpenGL is that we don't have to get tangled up in matrix
	// operations, it does it for us.  On the other hand, I recommend doing it at least once yourself.
	// Otherwise, you will be ignorant to what is really going on, and you aren't always going to have
	// OpenGL to do this stuff.  If you work on a console game, they might have their own SDK.  You have
	// to do a lot more manipulations with matrices in that case.

	// Here we draw a triangle with our own draw function.  This draws a triangle
	// around the origin at (0, 0, 0) with a width and height of (1, 1).
	DrawTriangle( 0, 0, 0, 1, 1);

	// Now we want to draw another triangle, but we want to move it.  Let's draw the triangle
	// at the same position the last triangle was drawn, but let's move it to a new position.

	// First, let's push on a matrix.  We only want to move THIS triangle, so let's use a
	// separate matrix so it doesn't effect any of the other triangles.
	glPushMatrix();											// Push on another matrix to use

		// Now that we have a separate matrix, let's draw another triangle,
		// but let's move it off to the right.  Before we draw the triangle though,
		// we need to translate it to the new position.  This changes the current matrix,
		// and then when the triangle is drawn, it is effected by the translation.
		// Basically what that means is that you can't draw the triangle, and then move it.
		// You need to set the position it will go, then draw it at that position.

		// glTranslate() moves the current position of all the 3D points to a new (X, Y, Z).
		// This translation will move the next triangle back into the screen and off to the right.

		glTranslatef(1, 0, -1);								// This moves everything drawn afterwards to (1, 0, -1)

		// Now we will draw the triangle at the origin, but the current matrix will actually
		// translate the triangle to a different location (back and to the right).
			
		DrawTriangle( 0, 0, 0, 1, 1);

	glPopMatrix();											// Pop off this matrix and go back to the previous one

	// Let's draw another triangle, but off to the left now.
	// Since we are doing more translations, let's push on another matrix.
	// Realistically, we don't have to since this is the last thing being drawn,
	// so if we change the current default world matrix, it won't affect any of the
	// other triangles since they are already drawn, but let's do it just for the sake of practice. :)

	glPushMatrix();											// Here we push on a new matrix to manipulate

		glTranslatef(-1, 0, -1);							// Let's translate the last triangle back and to the left
		DrawTriangle( 0, 0, 0, 1, 1);						// Draw the triangle at the origin (but will appear back and to the left).

	glPopMatrix();											// Pop this matrix off the stack.

	// That is the simplest way to use matrices in OpenGL.  They come in handy in almost
	// everything you do.  The reason why we use a matrix is because it is faster than
	// rotating or translating every point manually.  There is less operations.  it is also
	// easier to work with.  If we want to scale, rotate, AND translate a point, we can do
	// it in one matrix operation, where we would have to do it in 3 if we did it manually.
	// This is explained below in *Quick Notes* a bit more.

	// So, let's see if you understand what is happening here.  What would happen
	// if, in this last triangle drawn, we drew the triangle at (1, 0, 0) instead of (0, 0, 0)?
	// Try and it and see.  Where would it draw it?  The answer is: (0, 0, -1).
	// Why you ask?  Because we translated the triangle over (-1, 0, -1).  It doesn't
	// move the triangle to the X Y and Z position, it offsets it by that amount.
	// So, if we draw the triangle off to the right, but we translated it to the left
	// by one, and back 1... it just moves it back to the center and moves it back by one.
	// Try it if you don't believe me :)  Hopefully this will give you a quick start to
	// understanding how to use matrices.

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// * ///////


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
// Matrices are cool huh?  They let you do a lot for a little amount of code.
// And that's really what we want isn't it? :)
// 
// Here is an example of a 4x4 homogeneous matrix:
// 
// [ 1 0 0 0 ]
// [ 0 1 0 0 ]
// [ 0 0 1 0 ]
// [ 0 0 0 1 ]
//
// Does this look familiar from math class?  Yup, it's the identity matrix.
// 
// The translation for x y and z are stored in these slots:
// 
// [ 1 0 0 x ]
// [ 0 1 0 y ]
// [ 0 0 1 z ]
// [ 0 0 0 1 ]
// 
// The scaling slots are these (scaling of X Y and Z):
//
// [ x 0 0 0 ]
// [ 0 y 0 0 ]
// [ 0 0 z 0 ]
// [ 0 0 0 1 ]
//
// If we put them all together we have 1 matrix that does scaling and rotation:
// 
// [ x 0 0 x2 ]		// X Y Z are the scale value, where X2 Y2 and Z2 are the translation values
// [ 0 y 0 y2 ]
// [ 0 0 z z2 ] 
// [ 0 0 0 1  ]
// 
// This is what it looks like under the scenes.  A matrix is just 4 rows and 4 columns.
// If we were to create a matrix array we would do this:  
//
// float matrix[16]		or		float maxtrix[4][4]
//  
// One thing I want to explain is that, just like scopes in C/C++ { } you can do the same
// thing with matrices.  Here is an example:
// 
// glPushMatrix();
// 
//		glTranslate(0, 1, 0);
//		DrawTriangle(0, 0, 0, 1, 1);
//
//		glPushMatrix();
//
//			glTranslate(0, 1, 0);
//			DrawTriangle(0, 0, 0, 1, 1);
//			
//		glPopMatrix();
//
// glPopMatrix();
//
// The first Triangle would be draw at (0, 1, 0), but where would the second one be drawn?
// Here is the tricky part.  Once again, remember that glTranslate() does not necessarily mean
// it will draw at those coordinates.  Since we already moved the current matrix to draw at (0, 1, 0),
// the matrix that the second triangle is working with is working FROM that last matrix.
// This will then draw the triangle at (0, 2, 0).  
//
// Before we do any translating or rotation, the initial matrix is pure.  that means,
// if we translate anything to a certain (X, Y, Z), it will actually move it to that (X, Y, Z)
// position.  But if we then call that same translation function with the same values, it acts
// more like a delta value because the matrix is changed from then on out, until we call glLoadIdentiy().
// It will actually move the new position to the new X Y and Z STARTING from that initial value.
// Does that make sense?  It's like, if you have:
// 
// int x = 0;
//
// Then, you say:
//
// x += 2;
//
// X was pure before we added 2 to it.  Now x is changed. so if we say:
//
// x += 2;
//
// again, it will not be 2 still, it will now be 4.  That is like how matrices work, but not really.
// The concept is the same though, once you change them, you are then working with a different matrix.
//
// Try using glScalef() and glRotatef() to further understand this subject.
//
// Let us know if this tutorial helped you.
//
//
// © 2000-2005 GameTutorials
