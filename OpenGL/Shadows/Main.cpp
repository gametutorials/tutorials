//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorilas.com		 //
//																		 //
//		$Program:		Shadows											 //
//																		 //
//		$Description:	Simple planar shadows using the stencil buffer   //
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
float g_DT;												// This stores the elapsed time between the current and last frame



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// In this tutorial we will show one of the most basic ways to draw real-time
// shadows.  This technique is called "planar shadows".  In this lesson we
// have 2 objects (cylinder and sphere) that will shadow the background walls.
//
// * How do planar shadows work? *
// 
// To compute shadows we need to do a bit of math.  Basically, the technique
// is called planar shadows because it is most effective for flat surfaces.
// This is because we need to render an object for EVERY plane
// that will have shadows.  So, in our example we have four wall right?  In the
// code we have to draw the cylinder and sphere for every wall.  We create a special 
// shadow matrix that will project the objects to a plane, depending on our 
// light's position.  Since we need to keep track of each walls normal and
// vertices for rendering planar shadows I created a simple structure tWalls
// that holds that information.
//
// To help us with the shadows effect we use the stencil buffer.  This will
// allow us to only draw the shadows where the walls are.  Otherwise the shadows
// would extend past the walls and go into space which would look horrible.
//
// * What is the stencil buffer? *
//
// OpenGL has some functions that allow you to create a stencil, essentially
// a mask, which we can then render something only to that mask.  We do this
// with our shadows.  We create a black mask where our walls are drawn, then
// we draw our shadows into the spot where the walls are, constrained
// by our mask.  This makes sure we only draw a shadow within the perimeter of
// the walls.  Uncomment the stencil code and run it to see why this is necessary.
//
// If we only were drawing shadows on the floor and not on the walls we wouldn't
// need stencils.  You could just loop through all the faces in your world and
// only render to polygons that had a normal of (0, 1, 0), which is pointing
// straight up to tell us it's the floor.  Of course this would only be acceptable
// if your world was like a first person shooter with just walls and floor, and
// no bumpy terrain/floors.
//
// * What other shadow methods are better than planar shadows? *
//
// In the next tutorial you will find a different shadow technique called
// shadow mapping.  This renders shadows by comparing what the light sees
// and what the camera sees.  Using depth values we can test to see if there
// needs to be a shadow rendered on the current pixel.  This only requires
// 2 passes, instead of the numerous passes that planar shadows require.
//
// Let's go look over the code below, and remember to look at the bottom of
// this file in the QUICK NOTES to get a more in-depth look at the math required
// for creating the shadow matrix.
//
// We added our normal camera code in this tutorial, as well as adding a stencil
// bit to our pixel-format descriptor in Init.cpp.  This is necessary for using
// the stencil buffer.  In 99% of our tutorials we don't use the stencil buffer so
// we need to be sure to add a stencil-buffer bit for our program.
//
// It is assumed that you have read through the basic OpenGL tutorials like
// Quadrics, Matrices and Lighting.  Let's go hide some light!
//


// Here we add our normal camera object so we can move through the world.
CCamera g_Camera;

// This structures holds each wall's vertex, normal and color information.
vector<tWall> g_Walls;

// Below is our starting light position and moving X direction
float g_LightPosition[4] = {0.0f, 0.0f, 13.0f, 1.0f};
float g_Direction = -0.04f;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes the program window.
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// In this lesson our Init() function sets up the light, positions our
	// camera, and creates our walls.

	// First we need to give OpenGL our light position so it can light the world
	glLightfv( GL_LIGHT0, GL_POSITION, g_LightPosition );

	// This turns the background to a dark grey/black.
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	// Like normal, let's turn on depth testing
	glEnable(GL_DEPTH_TEST);

	// Here we enable lighting and the first OpenGL light, then turn on color materials.
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	// Move the camera to its Position,    View,  and  Up Vector
	g_Camera.PositionCamera(0, 0, 18,     0, 0, 0,     0, 1, 0);

	// Now we need to initialize the four walls.  We create the walls
	// with a size of 5 and store the data in a tWall structure so we
	// can keep track of the wall data when creating the wall's shadow 
	// matrix.
	float size = 5.0f;	// The wall size
	tWall wall;			// The wall structure we will fill in for each wall

	// When we create the walls we add the vertices to a STL vector list.
	// We then add the wall structure to a STL vector list of tWalls.

	// Create the LEFT wall
	wall.vVerts.clear();									// Clear the vertex list
	wall.vNormal = CVector3(1, 0, 0);						// Store the RIGHT normal
	wall.vVerts.push_back(CVector3(-size, size, size));		// The top-left vertex
	wall.vVerts.push_back(CVector3(-size, -size, size));	// The bottom-left vertex
	wall.vVerts.push_back(CVector3(-size, -size, 0));		// The bottom-right vertex
	wall.vVerts.push_back(CVector3(-size, size, 0));		// The top-right vertex
	wall.vColor = CVector3(1, 0, 0);						// Make this wall red
	g_Walls.push_back(wall);								// Add this wall to our list

	// Create the RIGHT wall
	wall.vVerts.clear();									// Clear the vertex list
	wall.vNormal = CVector3(-1, 0, 0);						// Store the LEFT normal
	wall.vVerts.push_back(CVector3(size, size, 0));			// The top-left vertex
	wall.vVerts.push_back(CVector3(size, -size, 0));		// The bottom-left vertex
	wall.vVerts.push_back(CVector3(size, -size, size));		// The bottom-right vertex
	wall.vVerts.push_back(CVector3(size, size, size));		// The top-right vertex
	wall.vColor = CVector3(0, 0, 1);						// Make this wall blue
	g_Walls.push_back(wall);								// Add this wall to our list
	
	
	// Create the BOTTOM wall
	wall.vVerts.clear();									// Clear the vertex list
	wall.vNormal = CVector3(0, 1, 0);						// Store the UP normal
	wall.vVerts.push_back(CVector3(size, -size, size));		// The front-right vertex
	wall.vVerts.push_back(CVector3(size, -size, 0));		// The back-right vertex
	wall.vVerts.push_back(CVector3(-size, -size, 0));		// The back-left vertex
	wall.vVerts.push_back(CVector3(-size, -size, size));	// The front-left vertex
	wall.vColor = CVector3(0, 1, 0);						// Make this wall green
	g_Walls.push_back(wall);								// Add this wall to our list
	
	// Create the BACK wall
	wall.vVerts.clear();									// Clear the vertex list
	wall.vNormal = CVector3(0, 0, 1);						// Store the FORWARD normal
	wall.vVerts.push_back(CVector3(-size, size, 0));		// The top-left vertex
	wall.vVerts.push_back(CVector3(-size, -size, 0));		// The bottom-left vertex
	wall.vVerts.push_back(CVector3(size, -size, 0));		// The bottom-right vertex
	wall.vVerts.push_back(CVector3(size, size, 0));			// The top-right vertex
	wall.vColor = CVector3(1, 0, 1);						// Make this wall pink
	g_Walls.push_back(wall);								// Add this wall to our list
}


///////////////////////////////// RENDER OBJECTS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the objects that need to be shadowed
/////
///////////////////////////////// RENDER OBJECTS \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderObjects()
{
	// This function just displays our color cylinder and sphere.  We use
	// the GLU library to draw the quadrics.

	// Color the cylinder green
	glColor4f(0.0f, 1.0f, 0.0f, 1.0);

	// Create a pointer to a quadric object and set its type to FILL
	GLUquadricObj *pObj = gluNewQuadric();			
	gluQuadricDrawStyle(pObj, GLU_FILL);

	// To make sure our positioning and rotation of our objects don't affect
	// other objects we push and pop a couple matrices.  Below we draw our
	// cylinder with a set position and continual rotation.
	glPushMatrix();
		static float rotY = 0;
		glTranslatef(-2, 0, 4);
		glRotatef(rotY++, 0, 1, 0);
		gluCylinder(pObj, 0.5f, 0.5f, 2.0f, 75, 75);
	glPopMatrix();

	// Color the sphere yellow
	glColor4f(1.0f, 1.0f, 0.1f, 1.0);

	// Move and draw our yellow sphere
	glPushMatrix();
		glTranslatef(2, 0, 4);
		gluSphere(pObj, 0.5f, 75, 75);
	glPopMatrix();
	
	// Reset the color back to pure white
	glColor4f(1.0f, 1.0f, 1.0f, 1.0);

	// Free our quadric object
	gluDeleteQuadric(pObj);
}


///////////////////////////////// RENDER WALL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders a wall, given an instance of a wall passed in
/////
///////////////////////////////// RENDER WALL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderWall(const tWall &wall)
{
	// To draw a wall we just take a reference to a wall and then loop
	// through all of its vertices and draw a quad with its normal and
	// color.

	// Push on a new matrix in order to not affect other objects
	glPushMatrix();
		
		// Start rendering QUADS or our wall
		glBegin(GL_QUADS);
			
			// Set the normal of the current wall for lighting
			glNormal3f(wall.vNormal.x, wall.vNormal.y, wall.vNormal.z);

			// Color the wall with it's set color
			glColor4f(wall.vColor.x, wall.vColor.y, wall.vColor.z, 1.0);

			// Go through all of the wall's vertices and input them to OpenGL
			for(int i = 0; i < (int)wall.vVerts.size(); i++)
			{
				glVertex3f(wall.vVerts[i].x, wall.vVerts[i].y, wall.vVerts[i].z);
			}

		// Stop drawing QUADS
		glEnd();

	// Return back to the previous matrix
	glPopMatrix();
}


///////////////////////////// CREATE SHADOW MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This fills in a matrix for a projected shadow to be applied to an object
/////
///////////////////////////// CREATE SHADOW MATRIX \\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateShadowMatrix(float m[16], CVector3 vPoint, CVector3 vN, float l[4])
{
	// Muhahaha!  Are you ready for the meat and potatoes of this tutorial?
	// Below we setup the shadow matrix that will be applied to our objects
	// for each plane (wall).  This matrix will then project a stenciled shadow
	// flat onto a plane (wall).
	//
	// In order to create a shadow matrix we need the following information:
	//
	// - A point on the plane that the shadow will be projected on (a vertex)
	// - The normal of the plane that the shadow will be projected on
	// - The light position or direction (we use a position light)
	//
	// From the above information we can calculate all the needed information
	// for our shadow matrix.  
	//
	// (1) First we need to find the distance (d) that the plane is from the 
	// origin.  Remember that the plane equation is Ax + By + Cz + D = 0.  
	// Where ABC is the normal and XYZ is a point.  D is the distance.  
	// When the plane equation is 0 that means that the point XYZ is on the plane.  
	// We need to find the distance so we just use simple algebra to get the 
	// following: D = -(Ax + By + Cz). 
	//
	// (2) Next, we calculate the dot product between the normal and the light.
	// The result will give us the necessary data for projecting a shadow onto
	// the surface with the given normal.  We then use the distance calculation
	// to input into our matrix so that we know how far from the origin to place
	// the shadow.

	// Here we calculate the distance from the equation D = -(Ax + By + Cz)
	float d = - ((vN.x * vPoint.x) + (vN.y * vPoint.y) + (vN.z * vPoint.z));

	// Calculate the dot product of the normal and the light
	float dot = vN.x*l[0]  + vN.y*l[1] + vN.z*l[2] + d*l[3];

	// Now comes the juicy part of converting all of our projection information
	// a matrix.  The equation for the creating the matrix is below:

	/*	
		| dot-light.x*Normal.x	   -light.y*Normal.x	 -light.z*Normal.x	   -light.w*Normal.x |
	M = |    -light.x*Normal.y	dot-light.y*Normal.y	 -light.z*Normal.y	   -light.w*Normal.y |
		|	 -light.x*Normal.z	   -light.y*Normal.z  dot-light.z*Normal.z	   -light.w*Normal.z |
		|    -light.x*Distance	   -light.y*Distance	 -light.z*Distance	dot-light.w*Distance |
	*/

	// Follow the matrix equation below to fill in our matrix for the current surface
	m[0]  = dot - l[0]*vN.x;  m[1]  =	   -l[1]*vN.x;	m[2]  =		 -l[2]*vN.x;  m[3]  =	  -l[3]*vN.x;
	m[4]  =		 -l[0]*vN.y;  m[5]  = dot - l[1]*vN.y;	m[6]  =		 -l[2]*vN.y;  m[7]  =	  -l[3]*vN.y;
	m[8]  =		 -l[0]*vN.z;  m[9]  =	   -l[1]*vN.z;	m[10] = dot - l[2]*vN.z;  m[11] =	  -l[3]*vN.z;
	m[12] =		 -l[0]*d;	  m[13] =	   -l[1]*d;		m[14] =		 -l[2]*d;	  m[15] = dot -l[3]*d;
}


//////////////////////////// RENDER SHADOWED SURFACES \\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the walls and the shadows on those walls
/////
//////////////////////////// RENDER SHADOWED SURFACES \\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderShadowedSurfaces()
{	// Now that we have all the needed functions to start rendering shadows,
	// we put everything together with the stencil buffer to complete the effect.
	// The logic of using the stencil buffer to render the shadows is this:
	//
	// (1)  Clear the stencil buffer and start fresh
	// (2)  Enable stencil buffer testing
	// (3)  Set our stencil to first accept all the next pixels that will be rendered
	// (4)  Set the stencil operation to replace the stencil value with a 1 where the
	//      surface is drawn (since we use ALWAYS for our stencil function).
	// (5)  Draw the walls with the lights off (important) to create a stencil
	// (6)  Disable depth testing while using stencils after we create the stencil mask
	// (7)  Set the color mask to FALSE for RGBA so it just draws black shadows
	// (8)  Change the stencil function to draw only where the stencil value EQUALS 1
	// (9)  Set the stencil operation to keep the current pixels if the test fails,
	//      then increment the stencil buffer when we find a 1 in the stencil buffer.
	//      That means that where ever the shadows are drawn the stencil buffer will
	//      have a 2 instead of a 1 value (it was incremented when found).  This is
	//      important because when we draw the walls we only draw when we find a 1,
	//      which means the walls won't draw over the area where the shadows were
	//      drawn because there is a 2 in the stencil buffer.
	// (10) Create/apply the shadow matrix for the current surface, then draw the
	//      objects.  The object will then be projected as black to the surface.
	// (11) Restore the color mask to TRUE since we don't need black shadows anymore
	// (12) Render the surfaces except where the shadows were drawn (value 2).
	// (13) Turn depth testing back on after drawing the shadow and wall.
	// (14) Finally, disable the stencil buffer testing
	//
	// We don't perform step 14 until we have looped through 3-13 as many times as there
	// is walls to render.

	glClear(GL_STENCIL_BUFFER_BIT);							// (1) Clear the stencil buffer
	glEnable(GL_STENCIL_TEST);								// (2) Turn on stencil testing

	// Now we need to go through all of the walls and render the shadows to them
	for(int i = 0; i < (int)g_Walls.size(); i++)
	{
		glStencilFunc(GL_ALWAYS, 1, 1);						// (3) Store a 1 for all pixels
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);			// (4) Replace the stencil value with 1

		// To create the mask we just drawn the current wall now to the stencil buffer
		glDisable(GL_LIGHT0);							
		RenderWall(g_Walls[i]);								// (5) Create our stencil mask
		glEnable(GL_LIGHT0);                          

		glDisable(GL_DEPTH_TEST);							// (6) Make sure depth testing is off
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);// (7) Turn the shadows black
		glStencilFunc(GL_EQUAL, 1, 1);                      // (8) Draw where stencil value is 1
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);             // (9) Increase value after pixel drawn

		// (10) Here is the place where we actually draw and project the shadow
		glPushMatrix();

			// To create a shadow matrix we pass in the matrix array "m", then
			// pass in the first vertex of the wall, its normal, and the light position.
			// To apply the matrix we use glMultMatrixf() to multiply the matrix by
			// the current model-view matrix.  This projects the shadow appropriately.
			float m[16] = {0};								
			CreateShadowMatrix(m, g_Walls[i].vVerts[0], g_Walls[i].vNormal, g_LightPosition);
			glMultMatrixf(m);

			// From here on all things rendered will be projected onto the current wall.
			// So this is the point where we draw the objects in our scene that need shadows.
			glPushMatrix();                                     
				RenderObjects();
			glPopMatrix();

		// Pop the matrix to go back to normal and so nothing else will be projected as a shadow
		glPopMatrix();

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);	// (11) Restore the color mask

		RenderWall(g_Walls[i]);								// (12) Draw walls around the shadow
		glEnable(GL_DEPTH_TEST);							// (13) Turn on normal depth testing again
	}

	glDisable(GL_STENCIL_TEST);                             // (14) We are done using the stencils
}


///////////////////////////////// RENDER LIGHT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders and moves the light at its current position
/////
///////////////////////////////// RENDER LIGHT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderLight()
{
	// This function isn't necessary, but it seems appropriate to show where
	// the light is in the world.  Basically, all we do is just draw a small
	// sphere that represents the light by using the GLU quadrics.

	// Disable lighting so the light isn't lit but bright yellow
	glDisable(GL_LIGHTING);

	// Push on a new matrix to not affect other objects in the world
	glPushMatrix();
		GLUquadricObj *pObj = gluNewQuadric();				// Get a Quadric off the stack
		gluQuadricDrawStyle(pObj, GLU_FILL);				// Draw the sphere normally

		// Draw the light as a sphere at the light's position with a yellow color
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		glTranslatef(g_LightPosition[0], g_LightPosition[1], g_LightPosition[2]);
		gluSphere(pObj, 0.05f, 25, 25);
		
		gluDeleteQuadric(pObj);								// Free the Quadric
	glPopMatrix();

	// Turn lighting back on for the rest of the world
	glEnable(GL_LIGHTING);

	// If the light position goes out of the screen to the left, reset the direction and position.
	if(g_LightPosition[0] < -8.0f)
	{
		g_Direction = g_Direction * -1;
		g_LightPosition[0] = -8.0f; 
	}
	// If the light position goes out of the screen to the right, reset the direction and position.
	if(g_LightPosition[0] > 8.0f)
	{
		g_Direction = g_Direction * -1;
		g_LightPosition[0] = 8.0f; 
	}	

	// Each frame we move the light by the current direction, then update the light with GL.
	g_LightPosition[0] += g_Direction;
	glLightfv( GL_LIGHT0, GL_POSITION, g_LightPosition );
}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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
				g_Camera.Update();						// Enable camera movement
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();									


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// This is how our rendering works for shadows.  (1) Position our Camera,
	// (2) render our objects, (3) render walls and shadows, (4) then draw our light.
	
	// (1) Position our camera
	g_Camera.Look();	
	
	// Here we make sure the color is put back to default white
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	//(2) Render our objects that will receive shadows
	RenderObjects();
	
	// (3) Render the walls and the object's shadows on the walls
	RenderShadowedSurfaces();

	// (4) Render the moving light so we can see where the shadows are coming from
	RenderLight();

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

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized
		if(!g_bFullScreen)								// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);				// Get the window rectangle
		}
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
        lRet = (long)DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}



/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// A bit frustrated with the matrix math and stencil code?  It can be pretty
// frustrating at first because there is so much to keep track of and wrap
// your head around.
//
// Let's go over the matrix equation again to summarize this tutorial:
//	
//		| dot-light.x*Normal.x	   -light.y*Normal.x	 -light.z*Normal.x	   -light.w*Normal.x |
//	M = |    -light.x*Normal.y	dot-light.y*Normal.y	 -light.z*Normal.y	   -light.w*Normal.y |
//		|	 -light.x*Normal.z	   -light.y*Normal.z  dot-light.z*Normal.z	   -light.w*Normal.z |
//		|    -light.x*Distance	   -light.y*Distance	 -light.z*Distance	dot-light.w*Distance |
//
// We need the light's position, the normal of the plane to project the shadow onto, a point
// on that plane (a vertex), the distance the plane is from the origin, and the dot product
// between the normal of the plane and the light's position.
// 
// Notice that if you look from the top left down to the bottom right there is a pattern.
// This is called the matrix's diagonal.  The matrix diagonal adds the dot product to itself.
// Once we create the matrix we can multiply it by any object we want to shadow and it will
// project that shadow onto a surface's plane.  We use the stencil buffer to make sure that
// the shadow doesn't go outside the surface's boundaries (the wall polygon).  Below is the
// step for rendering shadows with the stencil buffer:
//
// (1)  Clear the stencil buffer and start fresh
// (2)  Enable stencil buffer testing
// (3)  Set our stencil to first accept all the next pixels that will be rendered
// (4)  Set the stencil operation to replace the stencil value with a 1 where the
//      surface is drawn (since we use ALWAYS for our stencil function).
// (5)  Draw the walls with the lights off (important) to create a stencil
// (6)  Disable depth testing while using stencils after we create the stencil mask
// (7)  Set the color mask to FALSE for RGBA so it just draws black shadows
// (8)  Change the stencil function to draw only where the stencil value EQUALS 1
// (9)  Set the stencil operation to keep the current pixels if the test fails,
//      then increment the stencil buffer when we find a 1 in the stencil buffer.
//      That means that where ever the shadows are drawn the stencil buffer will
//      have a 2 instead of a 1 value (it was incremented when found).  This is
//      important because when we draw the walls we only draw when we find a 1,
//      which means the walls won't draw over the area where the shadows were
//      drawn because there is a 2 in the stencil buffer.
// (10) Create/apply the shadow matrix for the current surface, then draw the
//      objects.  The object will then be projected as black to the surface.
// (11) Restore the color mask to TRUE since we don't need black shadows anymore
// (12) Render the surfaces except where the shadows were drawn (value 2).
// (13) Turn depth testing back on after drawing the shadow and wall.
// (14) Finally, disable the stencil buffer testing
//
// That is pretty much it.  There are tons of optimizations to this technique, but
// this tutorial was to teach the very basic concept without any confusion.  What
// I would recommend is to put objects into display lists so you don't have to
// keep sending the graphics card tons of data for each pass.  Another technique
// would be to render each object to a texture quad and then keep projecting that
// quad onto the walls.  There is no need to render the object each time if it
// isn't moving during all of those passes.  This would be a great speed increase.
// You would of course need to use a form of masking to so that you don't render
// the entire quad texture to the surfaces, just the rendered object.  You could
// render the object to a texture with a pink background, then render it to an
// alpha texture to use as a mask for the transparency.  I would of course rather
// skip that step and just do it in shaders where you can do your own color keying.
//
// Though this is a cool effect for shadows, the technique is limiting because of
// the number of passes that need to happen to perform it.  The next tutorial
// shows how to do shadows with a method know as shadow mapping.  This only
// requires 2 passes for the entire world, but is somewhat limited to only 
// directional lights.  There are ways to use omni-directional lights, but it's a
// lot more complicated.
//
// Be sure to remember that when using stencils you have to add at least one
// stencil bit to your pixel-format description structure in Init.cpp.  We
// added this in Init.cpp.
//
//
// © 2000-2005 GameTutorials