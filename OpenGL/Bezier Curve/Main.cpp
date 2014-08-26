//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Bezier Curve									 //
//																		 //
//		$Description:	Moves a sphere along a bezier curve				 //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file


bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial shows you how to create a bezier curve.  This is a VERY handy
// bit of knowledge to have, especially as we see how popular curved surfaces
// in games are becoming (starting with Quake).  Some examples of when we would
// use a curve is for Tunnels, camera movements, etc... To demonstrate using a bezier
// curve, we will create a sphere that we can use the RIGHT and LEFT arrow keys
// to move a across a rendered curve.  Creating a bezier curve is simple, but
// the math at first can be intimidating.  Basically, a bezier curve has at least
// 3 points.  The first point is the starting point of the curve.  The second point
// is the control point of the curve.  A control point "controls" the arc of the curve.
// The third point is the end point of the curve.  In this tutorial we will use 4 control
// points (including the start and end point).  The first and last point will be where
// the curve starts and stops, and the middle 2 points will be used to shape the curve.
// 

#define BALL_SPEED	0.02f								// This is how fast our camera moves
#define MAX_STEPS   25.0f								// This is the amount of steps we want to draw the curve

// We create a class that has an X, Y and Z.  We will use this point class
// to store our XYZ points of the curve.

struct CVector3 										// Remember, we use a C in front to show it's a class
{
public:
	float x, y, z;										// We just want a float for a X Y and Z.
};

float g_CurrentTime = 0.0f;								// This is the current position of the sphere along the curve (0 to 1)

CVector3 g_vStartPoint    = {-4,  0,  0};				// This is the starting point of the curve
CVector3 g_vControlPoint1 = {-2,  3,  2};				// This is the first control point of the curve
CVector3 g_vControlPoint2 = { 2, -3, -2};				// This is the second control point of the curve
CVector3 g_vEndPoint	  = { 4,  0,  0};				// This is the end point of the curve

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
	
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Just to give the sphere more realism, we enable the default
	// lights for shading.  First we turn on a light, turn lighting on,
	// then enable coloring.  We need to enable color functions like glColor3f()
	// since lighting is on.  If we don't all objects will be white.

	glEnable(GL_LIGHT0);								// Turn on this light
	glEnable(GL_LIGHTING);								// Turn lighting on
	glEnable(GL_COLOR_MATERIAL);						// Since lighting is on, allow glColor*() functions

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

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
		elapsedTime -= desiredFPS; // Adjust the elapsed time

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


///////////////////////////////// POINT ON CURVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns an XYZ point along the curve, depending on t (0 to 1)
/////
///////////////////////////////// POINT ON CURVE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

CVector3 PointOnCurve(CVector3 p1, CVector3 p2, CVector3 p3, CVector3 p4, float t)
{
	float var1, var2, var3;
    CVector3 vPoint = {0.0f, 0.0f, 0.0f};

	// Here is the juice of our tutorial.  Below is the equation for a 4 control point
	// bezier curve:
	// B(t) = P1 * ( 1 - t )^3 + P2 * 3 * t * ( 1 - t )^2 + P3 * 3 * t^2 * ( 1 - t ) + P4 * t^3 
	// Yes I agree, this isn't the most intuitive equation, but it is pretty straight forward.
	// If you got up to Trig, you will notice that this is a polynomial.  That is what a curve is.
	// "t" is the time from 0 to 1.  You could also think of it as the distance along the
	// curve, because that is really what it is.  P1 - P4 are the 4 control points.
	// They each have an (x, y, z) associated with them.  You notice that there is a lot of
	// (1 - t) 's?  Well, to clean up our code we will try to contain some of these
	// repetitions into variables.  This helps our repeated computations as well.

	// Store the (1 - t) in a variable because it is used frequently
    var1 = 1 - t;
	
	// Store the (1 - t)^3 into a variable to cut down computation and create clean code
    var2 = var1 * var1 * var1;

	// Store the t^3 in a variable to cut down computation and create clean code
    var3 = t * t * t;

	// Now that we have some computation cut down, we just follow the equation above.
	// If you multiply and simplify the equation, you come up with what we have below.
	// If you don't see how we came to here from the equation, multiply the equation
	// out and it will become more clear.  I don't intend to go into any more detail
	// on the math of a bezier curve, because there are far better places out there
	// with graphical displays and tons of examples.  Look in our * Quick Notes *
	// for an EXCELLENT web site that does just this.  It derives everything and has
	// excellent visuals.  It's the best I have seen so far.
    vPoint.x = var2*p1.x + 3*t*var1*var1*p2.x + 3*t*t*var1*p3.x + var3*p4.x;
    vPoint.y = var2*p1.y + 3*t*var1*var1*p2.y + 3*t*t*var1*p3.y + var3*p4.y;
    vPoint.z = var2*p1.z + 3*t*var1*var1*p2.z + 3*t*t*var1*p3.z + var3*p4.z;

	// Now we should have the point on the curve, so let's return it.
    return(vPoint);				
}


///////////////////////////////// DRAW SPHERE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders a sphere to a given XYZ and with a given radius
/////
///////////////////////////////// DRAW SPHERE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawSphere(float x, float y, float z, float radius)
{
	// To use Quadrics, we need to create a new one first.  This is done below.
	// The GLUquadricObj type is defined with the GLU32 library and associated header file.

	GLUquadricObj *pSphere = gluNewQuadric();			// Get a Quadric off the stack

	// Let's put on a matrix so what ever we do it doesn't effect the rest of
	// the objects we will display.

	glPushMatrix();										// Push on a new matrix

		glTranslatef(x, y, z);							// Move the sphere to the desired (x, y, z)

		// Draw the a sphere with a given radius and a width and height detail 
		// of 15 (15 by 15 is a good level of detail.  The lower the detail the 
		// more jagged the sphere becomes, where the high the detail the more round it is.
		gluSphere(pSphere, radius, 15, 15);				// Draw the sphere with a radius of 0.2

	glPopMatrix();										// Pop the current matrix

	// Since we are done rendering the Quadric, we can free the object.
	// gluDeleteQuadric() takes the object to be released. If you have a lot 
	// of Quadrics, you might not want to allocate and free them every frame.

	gluDeleteQuadric(pSphere);							// Free the quadric object
}


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////
	
	static float rotateY = 0.0f;						// Create a static variable for rotation

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

	// Below, we will draw a bezier curve with 4 control points 
	// (including the start/end point).  We will use GL_LINES to draw the
	// line segments of the curve.  Then, using Quadrics, we will draw a sphere
	// on the curve.  You can use the LEFT and RIGHT arrow keys to move the
	// sphere along the curve.  If you are not familiar with quadrics, you can
	// view our quadric tutorial at www.GameTutorials.com.  I also include some
	// comments from the tutorial in here in case you just want the basics.
	// Quadrics are used to create cylinders and spheres quickly and easily.

	// We set up our camera back a little bit to get the whole curve in view.

				// Position      View		Up Vector
	gluLookAt(0, 0.5f, 10.0f,   0, 0.5f, 0,   0, 1, 0);	// Set up our camera position and view
		
	// Below we disable the lighting so you can clearly see the bezier curve.

	glDisable(GL_LIGHTING);								// Disable lighting for now

	glColor3ub(0, 255, 0);								// Set the color to Green

	CVector3 vPoint = {0.0f, 0.0f, 0.0f};				// Initialize a CVector3 to hold points.

	// Just so we can see the curve better, I rotate the curve and sphere

	glRotatef(rotateY, 0.0f, 1.0f, 0.0f);				// Rotate the curve around the Y-Axis

	rotateY += 0.1f;									// Increase the current rotation

	// Here we tell OpenGL to render lines with a greater thickness (default is 1.0)

	glLineWidth(1.5);									// Increase the size of a line for visibility

	// We haven't used GL_LINE_STRIP before so let me explain.  Instead of passing
	// in the first point of the line, then the next point of the line, then passing
	// in that same point again for the first point of the next line, etc... we can
	// do a line strip.  That means we just pass in ONE point and it connects them
	// for us.  If we just did GL_LINES it would render the curve is broken segments.
	// Strips are very usefull, as well as fast.  You can do quad and triangle strips too.

	glBegin(GL_LINE_STRIP);								// Start drawing lines

		// Here we actually go through the curve and get the points
		// that make up the curve.  Since our PointOnCurve() function
		// Take 4 points and a time value, we use a for loop starting
		// "t" at 0 (the starting point of the curve) and then increase
		// "t" by constant value of time.  Because time is measured from
		// 0 being the beginning of the curve, and 1 being the end, we divide
		// 1 by the amount of steps we want to draw the curve. Basically the 
		// amount steps defines the detail of the curve.  If we just had 4
		// steps, the curve would be created out of 4 lines.  The lowest the
		// the amount of steps to make the curve is 3.  Otherwise it is just
		// a straight line.  The more steps, the more rounded the curve is.

		// Go through the curve starting at 0, ending at 1 + another step.
		// Since we are using line strips, we need to go past the end point by 1 step.
		for(float t = 0; t <= (1 + (1.0f / MAX_STEPS)); t += 1.0f / MAX_STEPS)
		{
			// Here we pass in our 4 points that make up the curve to PointOnCurve().
			// We also pass in "t", which is the current time from 0 to 1.  If we pass
			// in 0 for t, we should get the starting point of the curve, if we pass in
			// 1 for t, we should get the end point of the curve.  So anything in between
			// 0 and 1 will be another point along the curve (IE, .5 would be a point halfway
			// along the curve).

			// Get the current point on the curve, depending on the time.
			vPoint = PointOnCurve(g_vStartPoint, g_vControlPoint1, g_vControlPoint2, g_vEndPoint, t);

			// Draw the current point at distance "t" of the curve.
			glVertex3f(vPoint.x, vPoint.y, vPoint.z);
		}

	glEnd();
	
	// Now that we drew the curve, we can turn lighting back on so the sphere
	// has some more realistic properties (shading).

	glEnable(GL_LIGHTING);								// Turn lighting back on

	// In order to move the sphere along the curve, we use the same function do draw
	// the curve, except that we pass in the current time from 0 to 1 that the sphere
	// is at.  In the beginning the sphere's time is 0, so it's at the beginning of the
	// curve.  As we hit the RIGHT arrow key, g_CurrentTime will increase and will move
	// the sphere along the curve with a certain fixed speed (BALL_SPEED).  When g_CurrentTime
	// get to 1, we stop.  If the time gets below 0, we stop again.

	// Get the current point on the curve, depending on the time.
	vPoint = PointOnCurve(g_vStartPoint, g_vControlPoint1, 
						  g_vControlPoint2, g_vEndPoint, g_CurrentTime);

	glColor3ub(255, 0, 0);								// Set the color to red
	
	// Draw the sphere at the current point along of the curve.  Give it a radius of 0.2f
	DrawSphere(vPoint.x, vPoint.y, vPoint.z, 0.2f);

	glColor3ub(255, 255, 0);							// Set the color to yellow

	// Now, we should display the control points so you can better visualize what they do.
	// We represent them as small yellow spheres.

	// Draw the first control point as a small yellow sphere
	DrawSphere(g_vControlPoint1.x, g_vControlPoint1.y, g_vControlPoint1.z, 0.1f);

	// Draw the second control point as a small yellow sphere
	DrawSphere(g_vControlPoint2.x, g_vControlPoint2.y, g_vControlPoint2.z, 0.1f);
	
//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

			// Here we check to the input of the keyboard to move the
			// ball along the curve.  We can use the RIGHT or LEFT arrow keys.

			case VK_LEFT:								// If we hit the LEFT arrow

				g_CurrentTime -= BALL_SPEED;			// Increase the position of the ball along the curve

				if(g_CurrentTime < 0)					// If we go below 0
					g_CurrentTime = 0;					// Set it back to 0
				break;

			case VK_RIGHT:								// If we hit the RIGHT arrow key

				g_CurrentTime += BALL_SPEED;			// Decrease the position of the ball along the curve

				if(g_CurrentTime > 1)					// If we go over 1
					g_CurrentTime = 1;					// Set it back to 1
				break;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

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
// This tutorial creates a bezier curve and allows a sphere to move along it
// with the LEFT and RIGHT arrow keys.  You will also note that we display
// 2 of the curve control points to get a better visualization of what is going on.
// 
// Here is the equation we are using:
//
// B(t) = P1 * ( 1 - t )^3 + P2 * 3 * t * ( 1 - t )^2 + P3 * 3 * t^2 * ( 1 - t ) + P4 * t^3 
//
// This is pretty straight forward, even though it looks complicated. If you got 
// up to Trig, you will notice that this is a polynomial.  That is what a curve is.
// "t" is the time from 0 to 1.  You could also think of it as the distance along the
// curve, because that is really what it is.  P1 - P4 are the 4 control points.
// They each have an (x, y, z) associated with them.  P1 is the starting point. P2 is
// the first control point, P3 is the second control point, and P4 is the end point of the
// curve.  You will hear that this is a 4 control point equation. I usually think of it
// as 2 control points because the 2 end points are just the start and end of the curve.
// 
// Bezier curves are awesome.  You can use them to make realistic camera movements.
// Tunnel effects most frequently use bezier curves to shape the tunnel.  Once you
// understand one single bezier curve, you can then branch off and learn how to create
// curved surfaces.  This will also prepare you to learn about NURBS and SPLINES.
// 
// I would like to thank Paul Bourke at http://astronomy.swin.edu.au/pbourke/curves/bezier/
// 
// His web site helped TREMENDOUSLY with understanding Bezier Curves and his source code.  
// I suggest checking it out because it has a HUGE range of other graphics programming topics.
// 
// Once again, go to http://astronomy.swin.edu.au/pbourke/curves/bezier/ to get a more clear
// understanding of the math behind Bezier Curves as well as excellent visual examples.
//
// I would suggest changing the control points around so you could get a better idea of
// how they effect the curve.  The higher the control points are, the higher the arc is.
//
// Good Luck!
//
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
// 


