//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen			DigiBen@GameTutorials.com		 //
//																		 //
//		$Program:		Quaternions										 //
//																		 //
//		$Description:	Shows the basics of rotation using quaternions	 //
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
// This tutorial is and introduciton to quaternions.  This tutorial shows you how to 
// implement rotations using quaternions.  Just the word itself sends your mind into visions 
// of Albert Einstein spending days writing on multiple tiers of chalk boards.  Does 
// quaternions deal with something like quantum physics?  Do I need a PHD in math to 
// understand them?  Do I need to know them to do 3D game programming?  Did the Easter Bunny 
// ever have to learn this stuff?
// 
// Well, the answer to all these questions is no, except the Easter Bunny one.  Though
// quaternions are extremely useful for interpolating between 2 angles, they aren't the
// only way to do it, just one of the best.  So no, you don't HAVE to know about them, but if 
// you are dealing with 3D graphics programming, it's crazy not to understand a bit about them.
//
// Around the eighteenth century, a man named W. R. Hamilton discovered quaternions and that 
// they could be used as a 4D extension to complex numbers. Following this discovery, it was 
// proven that quaternions could also be used for rotations and orientations in three dimensions. 
// It wasn't until 1985 that quaternions were used in computer graphics.
//
// In essence, quaternions are used for rotations.  One of the most popular applications for
// quaternions is in character bone/skeletal animation.  Think of a quaternion as a 4D vector:
//
// q = (x, y, z, w)    or    q = w + xi + yj + zk  // i,j and k being imaginary numbers
// 
// Another representation of a quaternion is:
// 
// q = [w, v]
// 
// "v" represents a vector (x, y, z), and "w" is a scalar.  Just when you think you are
// starting to understand what a quaternion is, a stumbling block is placed in front of you.
// That is, you can't think of the vector (x, y, z) as a 3 dimensional vector, but a vector
// in 4 dimensions.  You can't visualize this because we can't really comprehend a fourth
// dimension.  This is one of those things that you need to just accept, and move on (while  
// playing Quake 3 and barreling your rail gun into your opponent's chest, which if it happens
// to be the Angelina Jolie version of Lara Croft, a big one at that).
//
// When talking about quaternions, I will be using the first representation.  Before I go
// on to more of the math, I will address the reasons why we use quaternions.
// 
// Before quaternions, Euler Angles were primarily what was used to represent a rotation.
// Euler angles are defined as an angle for each axis like so:
//
// (x, y, z) or (yaw, pitch, roll) or (heading, pitch, roll)
//
// These are the same, just different representations.  "Heading" is sometimes preferred
// to "yaw" because of the Y abbreviation which confuses yaw with the y-axis.  Each angle
// has it's own rotational matrix.  That means, if each axis rotation is written with matrices 
// B, C, and D, then the general matrix A can be defined as:
//
// A = BCD
//
// with B C D described as:
//
// B = [  1       0           0 ]	 C = [ cos(p)  0     sin(p) ]  D = [  cos(roll) sin(roll) 0 ]
//	   [  0  cos(yaw) -sin(yaw) ]	     [ 0       1          0 ]      [ -sin(roll) cos(roll) 0 ]
//	   [  0  sin(yaw)  cos(yaw) ]	     [[-sin(p) 0     cos(p) ]      [  0         0         1 ]
//
// Keep in mind that the order that the matrices are multiplied together is important.
//
// The famous problem with euler angles is the potential for gimbal lock.  This happens 
// when a rotation "overrides" another rotation, which limits your degrees of freedom.
// Quaternions solve this problem, as well as allow a smooth rotation when interpolating
// between 2 rotations.  Though axis angle rotation may not suffer from gimbal lock, when
// interpolated, they can cause unnatural abrupt movements between the rotations.  One of the 
// simplest form of quaternion interpolation is called:
//
// SLERP (Spherical Linear Interpolation - Strange acronym isn't it?)
//
// When would we need to interpolate between rotations?  Like I mentioned above, the perfect
// example is when doing bone animation.  Each key frame is usually stored with a rotation
// matrix, which can be converted to a quaternion.  If you have the current frame and the
// next frame's rotation stored in a quaternion, the interpolation can easily be performed,
// which will result in a new interpolated quaternion, depending on time t of course.  Check
// out the MD3 Animation tutorial on our site to see an example of SLERP in action.
//
// There are many aspects that deal with quaternions, but we will only focus on 
// converting an angle axis rotation to a quaternion, then converting a quaternion to
// a rotational matrix to be applied to the current model view matrix.
//
// * PERSONAL NOTE *
// My intentions for this tutorial is not to write a sweet robust quaternion class, but
// to SLOWLY ease you into an introduction to quaternions.  For most of us on the planet
// earth, you can't learn quaternions in one sitting.  It's something that needs spoon
// feeding.  The reason for this is because not many people can understand quaternions.
// The math behind them is very complex, that it's not something that an average programmer
// can wrap their head around, unless they are fairly educated or excellent at math.  I would
// be lying if I said I fully understood them, but I do understand what they are used for and
// what they can do.  For those of you who use cosine and sine functions often, you are
// fairly comfortable with what they do, and when to use them, but if you were asked to sit
// down and write those function, many probably couldn't, at least with out a lot of research.
// The same thing applies to quaternions, you don't need to understand all the math to understand
// when to use them.  Don't get me wrong, I encourage anyone who wants to delve further and
// learn the identities and formulas that simplify to what we use here.  It can only help you,
// not hinder you.  I'll post some of my favorite links at the bottom of this tutorial for
// those who want to dive deeper into quaternions.  I want to stress this for those of you who 
// want to jump out a window because they think they will never be able to be a game programmer 
// without understanding them. Also I want to stress, that if you are one of those newbies that 
// like to jump ahead in programming just to perform milestones for your own ego, without 
// learning all the programming basics in the middle, stop reading now because your kind is 
// destroying the game industry :)  
//
// This tutorial shows a line being rotated along the y-axis using quaternions.  There is also
// a sphere being rotated around the tip of that line.  
//
// The controls are:
// 
// LEFT ARROW - Moves the sphere left along the line segment
// RIGHT ARROW - Moves the sphere right along the line segment
// UP ARROW - Increases the radius that the sphere rotates around the line
// DOWN ARROW - Decreases the radius that the sphere rotates around the line
// ESC - Quits the program
//
// It is assumed that by reading this tutorial you already understand what a matrix is. 
// This tutorial was taken from the base of the quadric tutorial.  Though it's not necessary 
// to understand this tutorial, it would be beneficial if quadrics were understood.  Note, 
// when we mention quaternions, it should also be assumed that we mean unit quaternions.  
// If the quaternion is not a normalized quaternion, that opens up Pandora's box of math :)
//
//

// This	is the array of 2 vertices that will hold our line segment coordinates		
CVector3 g_vLine[2];

// This is the center of our sphere
CVector3 g_vPosition;


////////////////////////////// CREATE FROM AXIS ANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function converts an axis and angle rotation to a quaternion
/////
////////////////////////////// CREATE FROM AXIS ANGLE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CQuaternion::CreateFromAxisAngle(float X, float Y, float Z, float degree) 
{ 
	// This function takes an angle and an axis of rotation, then converts
	// it to a quaternion.  An example of an axis and angle is what we pass into
	// glRotatef().  That is an axis angle rotation.  It is assumed an angle in 
	// degrees is being passed in.  Instead of using glRotatef(), we can now handle
	// the rotations our self.

	// The equations for axis angle to quaternions are such:

	// w = cos( theta / 2 )
	// x = X * sin( theta / 2 )
	// y = Y * sin( theta / 2 )
	// z = Z * sin( theta / 2 )

	// First we want to convert the degrees to radians 
	// since the angle is assumed to be in radians
	float angle = float((degree / 180.0f) * PI);

	// Here we calculate the sin( theta / 2) once for optimization
	float result = (float)sin( angle / 2.0f );
		
	// Calcualte the w value by cos( theta / 2 )
	w = (float)cos( angle / 2.0f );

	// Calculate the x, y and z of the quaternion
	x = float(X * result);
	y = float(Y * result);
	z = float(Z * result);
}


////////////////////////////// CREATE MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function converts a quaternion to a rotation matrix
/////
////////////////////////////// CREATE MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\*	

void CQuaternion::CreateMatrix(float *pMatrix)
{
	// Make sure the matrix has allocated memory to store the rotation data
	if(!pMatrix) return;

	// This function is a necessity when it comes to doing almost anything
	// with quaternions.  Since we are working with OpenGL, which uses a 4x4
	// homogeneous matrix, we need to have a way to take our quaternion and
	// convert it to a rotation matrix to modify the current model view matrix.
	// We pass in a 4x4 matrix, which is a 1D array of 16 floats.  This is how OpenGL
	// allows us to pass in a matrix to glMultMatrixf(), so we use a single dimensioned array.
	// After about 300 trees murdered and 20 packs of chalk depleted, the
	// mathematicians came up with these equations for a quaternion to matrix converion:
	//
	//     ¦        2     2												 ¦
    //     ¦ 1 - (2y  + 2z )   2xy + 2zw         2xz - 2yw			0	 ¦
    //     ¦															 ¦
    //     ¦                          2     2							 ¦
    // M = ¦ 2xy - 2zw         1 - (2x  + 2z )   2zy + 2xw			0	 ¦
    //     ¦															 ¦
    //     ¦                                            2     2			 ¦
    //     ¦ 2xz + 2yw         2yz - 2xw         1 - (2x  + 2y )	0	 ¦
    //     ¦															 ¦
	//     ¦															 ¦
	//     ¦ 0				   0				 0					1	 |													 ¦
	//     ¦															 ¦
	// 
	// This is of course a 4x4 matrix.  Notice that a rotational matrix can just
	// be a 3x3 matrix, but since OpenGL uses a 4x4 matrix, we need to conform to the man.
	// Remember that the identity matrix of a 4x4 matrix has a diagonal of 1's, where
	// the rest of the indices are 0.  That is where we get the 0's lining the sides, and
	// the 1 at the bottom-right corner.  Since OpenGL matrices are row by column, we fill
	// in our matrix accordingly below.
	
	// First row
	pMatrix[ 0] = 1.0f - 2.0f * ( y * y + z * z ); 
	pMatrix[ 1] = 2.0f * (x * y + z * w);
	pMatrix[ 2] = 2.0f * (x * z - y * w);
	pMatrix[ 3] = 0.0f;  

	// Second row
	pMatrix[ 4] = 2.0f * ( x * y - z * w );  
	pMatrix[ 5] = 1.0f - 2.0f * ( x * x + z * z ); 
	pMatrix[ 6] = 2.0f * (z * y + x * w );  
	pMatrix[ 7] = 0.0f;  

	// Third row
	pMatrix[ 8] = 2.0f * ( x * z + y * w );
	pMatrix[ 9] = 2.0f * ( y * z - x * w );
	pMatrix[10] = 1.0f - 2.0f * ( x * x + y * y );  
	pMatrix[11] = 0.0f;  

	// Fourth row
	pMatrix[12] = 0;  
	pMatrix[13] = 0;  
	pMatrix[14] = 0;  
	pMatrix[15] = 1.0f;

	// Now pMatrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
}


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes OpenGL and all of the applications relevant data
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
	
	// Initialize our line segment along the X axis
	g_vLine[0] = CVector3(-0.5f,  0,   0);
	g_vLine[1] = CVector3( 0.5f,  0,   0);

	// Initialize our position of the sphere, which will be used as the given point
	g_vPosition = CVector3(0.5f, 0.1f, 0);

	// Here, we turn on a light and enable lighting.  We don't need to
	// set anything else for lighting because we will just take the defaults.
	// We also want color, so we turn that on too.  This gives the sphere a bit
	// of dimension.

	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

		// 	    Position          View	     Up Vector
	gluLookAt(0, 1, 2,  0, 0, 0,   0, 1, 0);			// This determines where the camera's position and view is

	// This sets our line width to %250, since 1 is %100 for normal line width
	glLineWidth(2.5f);

	// Turn off lighting so our line isn't effected by it
	glDisable(GL_LIGHTING);

	// Create a static variable to hold the increasing rotation
	static float rotation = 0;

	// Create a 4x4 matrix to be used to convert a quaternion to a matrix
	float matrix[16];

	// Create a quaternion to rotate the line segment around the y-axis
	CQuaternion qRotationY;
	
	// Now we need to assign a rotation to our quaternion.  Since a quaternion
	// isn't intuitive to us, we want to assign a rotation by an axis and angle.
	// This means, we give it an axis and an angle to rotate around that axis.
	// This is what glRotatef() takes as well.

	// Assign the y-axis and the current rotation to our quaternion.  This will then
	// be converted to (x, y, z and w).  At first you might think of the x, y, z and
	// w as the axis and angle, but it's not really.  It's a 4D spherical rotation.
	qRotationY.CreateFromAxisAngle(0, 1, 0, rotation);

	// Once we have our desired rotation stored in a quaternion, in order for us to
	// do anything useful with it, we need to convert it to a matrix.  This matrix
	// will then be multiplied by our current model view matrix, which performs the rotation.

	// Pass in our local matrix and store the rotation 
	// matrix to be applied to our current matrix.
	qRotationY.CreateMatrix(matrix);

	// To apply our rotation, we multiply the new quaternion matrix by the current matrix.
	glMultMatrixf(matrix);

	// After applying the rotation, we now want to draw the line segment

	glBegin (GL_LINES);									// This is our BEGIN to draw
		glColor3ub(255, 255, 0);						// Make the Left vertex YELLOW
		glVertex3f(g_vLine[0].x, g_vLine[0].y, g_vLine[0].z);

		glColor3ub(255, 0, 0);							// Make the Right vertex RED
		glVertex3f(g_vLine[1].x, g_vLine[1].y, g_vLine[1].z);
	glEnd();											// This is the END of drawing

	// Turn lighting back on for the sphere
	glEnable(GL_LIGHTING);								

	// Instead of calculating the sphere ourselves, we are going to use quadrics.
	// Check out the tutorial on quadrics if this is confusing for you.

	// Allocate a quadric object to use as a sphere
	GLUquadricObj *pObj = gluNewQuadric();				// Get a Quadric off the stack

	// Here we create a quaternion that will be used to rotate the sphere around the line
	CQuaternion qSphereRotation;

	// Set the rotation of this quaternion to rotate around the x-axis with a faster rotation
	qSphereRotation.CreateFromAxisAngle(1, 0, 0, rotation * 8);

	// Once more, fill the matrix with the quaternion rotation matrix
	qSphereRotation.CreateMatrix(matrix);

	// Apply this matrix to our current matrix, which happens to still have the original
	// rotation that was applied from the line segment.  This is how we get the sphere
	// to rotate around the line, wherever it goes.
	glMultMatrixf(matrix);

	// Turn the sphere green
	glColor3ub(0, 255, 0);

	// Move the sphere to it's current position.  This can be changed by the arrow keys.
	// The higher the y position of the sphere, the large the radius of rotation the sphere
	// has around the line.
	glTranslatef(g_vPosition.x, g_vPosition.y, g_vPosition.z);

	// Draw the quadric as a sphere with the radius of .05 and a 15 by 15 detail.
	// To increase the detail of the sphere, just increase the 2 last parameters.
	gluSphere(pObj, .05f, 15, 15);

	// Free the Quadric
	gluDeleteQuadric(pObj);								

	// Increase the rotation degree
	rotation++;

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
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Below we check what the user types in.  If they use the arrow keys
	// then we want to move the sphere around (LEFT RIGHT UP and DOWN keys)
	// The UP/DOWN keys increase the radius it rotates around the line, where
	// the LEFT/RIGHT keys move the sphere along the line.

	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:							// Check if we hit the ESCAPE key.
				PostQuitMessage(0);					// Tell windows we want to quit
				break;

			case VK_UP:								// Check if we hit the UP ARROW key.
				g_vPosition.y += 0.01f;				// Increase the rotation radius
				break;

			case VK_DOWN:							// Check if we hit the DOWN ARROW key.
				g_vPosition.y -= 0.01f;				// Decrease the rotation radius
				break;

			case VK_LEFT:							// Check if we hit the LEFT ARROW key.
				g_vPosition.x -= 0.01f;				// Move the sphere left along the line
				break;

			case VK_RIGHT:							// Check if we hit the RIGHT ARROW key.
				g_vPosition.x += 0.01f;				// Move the sphere right along the line
				break;
		}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

		break;

    case WM_CLOSE:									// If the window is closed
        PostQuitMessage(0);							// Tell windows we want to quit
        break; 
     
    default:										// Return by default
        lRet = DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;									// Return by default
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Did you catch any of that?  I hope it wasn't too much.  I think the only thing
// that should be confusing is where the equations come from?  What I suggest is
// getting a decent math book and working through it if you so desire.  I listed
// a bunch of links at the bottom of this page for extra reference.  This is just the
// first quaternions tutorial of the series, but in the mean time, while we add more,
// take a look around the internet.
// 
// This tutorial was the simplest way to introduce quaternions that I could think of.
// When I started learning about them, I just wanted to know how to rotate something
// using quaternions.  The next step, is how to figure out how to convert a quaternion
// to a matrix, then use Spherical Linear Interpolation (or SLERP).
// 
// Every decent 3D library should have a decent quaternions class, so don't sell them
// short.  Though they are not absolutely necessary, they are what the industry is turning
// to because of their smooth functionality.
// 
// If you want to see the use of quaternions in character animation, check out our MD3
// animation tutorial.
//
// The links below are the ones I found to be most useful when learning quaternions:
// 
// This is an excellent article from gamedev that describes quaternions pretty well
// http://gamedev.net/reference/articles/article1095.asp
// 
// Here is a gamasutra article about quaternions and their usage in the game industry
// http://www.gamasutra.com/features/19980703/quaternions_01.htm
// 
// The best matrix and quaternion FAQ around, plus a bunch of source code
// http://gamedev.net/reference/articles/article1691.asp
// 
// This site is a great page for the math behind quaternions.  There is even a proof
// on how the rotation of a quaternion works.
// http://freefall.freehosting.net/articles/quaternions_1.html
//
// One of the more famous documents from Ken Shoemake that goes into great detail
// http://www.daimi.au.dk/~mbl/cgcourse/wiki/_files/shoemake94.pdf
//
// If you read this and find anything wrong, please email us so that we can fix it,
// otherwise, let us know if it was helpful or not.
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
