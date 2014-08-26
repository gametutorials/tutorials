//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		ObjectSelection									 //
//																		 //
//		$Description:	Let's you select objects in 3D			 		 //
//																		 //
//***********************************************************************//

// These 3 calls below are pre processor calls that include a library to your project.
// You can do this manually in the project settings window too.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file

UINT  g_Texture[MAX_TEXTURES];							// This will reference to our texture data stored with OpenGL UINT is an unsigned int (only positive numbers)
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// *OBJECT SELECTING*

// This tutorial was created to show you how to select objects in 3D.
// Basically, what you are doing is taking an X and Y coordinate of the mouse
// and checking to see if any objects are in that region when projected to 2D from 3D.
// This can be very useful for level editors and menu/interface situations.
// The basic concept is, before you draw an object, you just assign an ID to it.


#define SUN				100								// This is the object ID for the SUN	
#define EARTH			101								// This is the object ID for the EARTH
#define PLUTO			102								// This is the object ID for PLUTO

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

	CreateTexture(g_Texture[0], "Sun.bmp");				// Load "Sun.bmp" into openGL as a texture for the Sun
	CreateTexture(g_Texture[1], "Earth.bmp");			// Load "Earth.bmp" into openGL as a texture for the Earth
	CreateTexture(g_Texture[2], "Pluto.bmp");			// Load "Pluto.bmp" into openGL as a texture for Pluto

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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

float SunRotation   = 90;								// This holds our sun's current rotation
float EarthRotation = 90;								// This holds our earth's current rotation
float PlutoRotation = 90;								// This holds pluto's current rotation

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

	// We make our position a bit high and back to view the whole scene

		// 	  Position      View	   Up Vector
	gluLookAt(0, 3, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is

	glInitNames();										// This clears the name stack so we always start with 0 names.

	GLUquadricObj *pObj = gluNewQuadric();				// Get a new Quadric off the stack

	gluQuadricTexture(pObj, true);						// This turns on texture coordinates for our Quadrics

	// Bind the sun texture to the sun quadratic
	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);			// Bind the Sun texture to the sun

	// Below we call glPushName().  We need to pass in an ID that we can check later that will
	// be associated with the polygons drawn next.  Here is how it works.  We call glPushName()
	// and pass an ID.  This pushes this ID onto the name stack.  Then we draw any primitives 
	// or shapes, then we call glPopName() which stops assigning polys to that object name.  
	// We now have a group of polygons that are given an ID.  Our ID SUN now refers to the 
	// sun Quadric we draw below.

	glPushName(SUN);									// Push on our SUN label (IMPORTANT)

	// Here we push on a new matrix so we don't affect any other quadrics.
	// We first translate the quadric to the origin (0, 0, 0), Then we rotate it
	// about the Y axis.  This gives it the spinning effect.  Then we draw the
	// largest of the spheres.  This represents the sun with its texture map.
	
	glPushMatrix();										// Push on a new matrix scope
		glTranslatef(0, 0, 0);							// Translate this sphere to the left
		glRotatef(SunRotation, 0, 1.0, 0);				// Rotate the sphere around the Y axis to make it spin
		gluSphere(pObj, 0.5f, 20, 20);					// Draw the sunwith a radius of 0.5
	glPopMatrix();										// End the current scope of this matrix

	// Now that we drew the sun, we want to end our Object name.  We call glPopName()
	// to do that.  Now nothing else will be associated with the SUN ID.

	glPopName();										// Stop assigning polygons to the SUN label (IMPORTANT)

	// Next, we want to bind the Earth texture to our Earth sphere
	glBindTexture(GL_TEXTURE_2D, g_Texture[1]);

	// Once again, we want to create a object ID for our earth, so we push on the EARTH ID.
	// Now, when we draw the next sphere, it will be associated with the EARTH ID.

	glPushName(EARTH);									// Push on our EARTH label (IMPORTANT)

	// Once again, we want to pop on a new matrix as not to affect any other spheres.
	// We rotate the sphere by its current rotation value FIRST before we translate it.
	// This makes it rotate around the origin, which is where the sun is.
	// Then we rotate it again about the Y-axis to make it spin around itself.

	glPushMatrix();										// Push on a new matrix scope		
		glRotatef(EarthRotation / 3, 0, 1.0, 0);		// Rotate the sphere around the origin (the sun)
		glTranslatef(-2, 0, 0);							// Translate this sphere to the left
		glRotatef(EarthRotation, 0, 1.0, 0);			// Rotate the sphere to make it spin
		gluSphere(pObj, 0.2f, 20, 20);					// Draw the sphere with a radius of 0.2 so it's smaller than the sun
	glPopMatrix();										// End the current scope of this matrix

	// We are done assigning the EARTH object, so we need 
	// to stop assigning polygons to the current ID.

	glPopName();										// Stop assigning polygons to the EARTH label (IMPORTANT)

	// Bind the pluto texture to the last sphere
	glBindTexture(GL_TEXTURE_2D, g_Texture[2]);

	// Finally, we want to be able to click on Pluto, so we need a pluto ID.

	glPushName(PLUTO);									// Push on our PLUTO label (IMPORTANT)

	// Like we did with the earth, we rotate Pluto around the sun first,
	// then we translate it farther away from the sun.  Next, we rotate Pluto
	// around the Y axis to give it some spin.

	// Pass in our empty glBegin()/glEnd() statement because we are using Quadrics.
	// If we don't do this when using glLoadName(), it will grind to a hault on some cards.
	//glBegin(GL_LINES);
	//glEnd();

	glPushMatrix();										// Push on a new matrix scope
		glRotatef(PlutoRotation / 2, 0, 1.0, 0);		// Rotate the sphere around the sun
		glTranslatef(3, 0, 0);							// Translate this sphere farther away from the sun than the earth
		glRotatef(PlutoRotation, 0, 1.0, 0);			// Rotate the sphere around itself to produce the spin
		gluSphere(pObj, 0.1f, 20, 20);					// Draw the sphere with a radius of 0.1 (smallest planet)
	glPopMatrix();										// End the current scope of this matrix

	// We are finished with the PLUTO object ID, so we need to pop it off the name stack.

	glPopName();										// Stop assigning polygons to our PLUTO label (IMPORTANT)

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground

	gluDeleteQuadric(pObj);								// Free the Quadric

	// Below we increase the rotations for each sphere.

	SunRotation   += 0.2f;								// Rotate the sun slowly
	EarthRotation += 0.5f;								// Increase the rotation for the each
	PlutoRotation += 0.6f;								// Make pluto go the fastest
}


///////////////////////////////// PROCESS SELECTION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This takes the cursor's x and y position and returns the closet object
/////
///////////////////////////////// PROCESS SELECTION \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int RetrieveObjectID(int x, int y)
{
	int objectsFound = 0;								// This will hold the amount of objects clicked
	int	viewportCoords[4] = {0};						// We need an array to hold our view port coordinates

	// This will hold the ID's of the objects we click on.
	// We make it an arbitrary number of 32 because openGL also stores other information
	// that we don't care about.  There is about 4 slots of info for every object ID taken up.
	unsigned int selectBuffer[32] = {0};				
														
	// glSelectBuffer is what we register our selection buffer with.  The first parameter
	// is the size of our array.  The next parameter is the buffer to store the information found.
	// More information on the information that will be stored in selectBuffer is further below.

	glSelectBuffer(32, selectBuffer);					// Setup our selection buffer to accept object ID's

	// This function returns information about many things in OpenGL.  We pass in GL_VIEWPORT
	// to get the view port coordinates.  It saves it like a RECT with {top, left, bottom, right}

	glGetIntegerv(GL_VIEWPORT, viewportCoords);			// Get the current view port coordinates

	// Now we want to get out of our GL_MODELVIEW matrix and start effecting our
	// GL_PROJECTION matrix.  This allows us to check our X and Y coords against 3D space.

	glMatrixMode(GL_PROJECTION);						// We want to now effect our projection matrix
	
	glPushMatrix();										// We push on a new matrix so we don't effect our 3D projection

		// This makes it so it doesn't change the frame buffer if we render into it, instead, 
		// a record of the names of primitives that would have been drawn if the render mode was
		// GL_RENDER are now stored in the selection array (selectBuffer).

		glRenderMode(GL_SELECT);						// Allows us to render the objects, but not change the frame buffer

		glLoadIdentity();								// Reset our projection matrix

		// gluPickMatrix allows us to create a projection matrix that is around our
		// cursor.  This basically only allows rendering in the region that we specify.
		// If an object is rendered into that region, then it saves that objects ID for us (The magic).
		// The first 2 parameters are the X and Y position to start from, then the next 2
		// are the width and height of the region from the starting point.  The last parameter is
		// of course our view port coordinates.  You will notice we subtract "y" from the
		// BOTTOM view port coordinate.  We do this to flip the Y coordinates around.  The 0 y
		// coordinate starts from the bottom, which is opposite to window's coordinates.
		// We also give a 2 by 2 region to look for an object in.  This can be changed to preference.

		gluPickMatrix(x, viewportCoords[3] - y, 2, 2, viewportCoords);

		// Next, we just call our normal gluPerspective() function, exactly as we did on startup.
		// This is to multiply the perspective matrix by the pick matrix we created up above. 

		gluPerspective(45.0f,(float)g_rRect.right/(float)g_rRect.bottom,0.1f,150.0f);
		
		glMatrixMode(GL_MODELVIEW);						// Go back into our model view matrix
	
		RenderScene();									// Now we render into our selective mode to pinpoint clicked objects

		// If we return to our normal render mode from select mode, glRenderMode returns
		// the number of objects that were found in our specified region (specified in gluPickMatrix())

		objectsFound = glRenderMode(GL_RENDER);			// Return to render mode and get the number of objects found

		glMatrixMode(GL_PROJECTION);					// Put our projection matrix back to normal.
	glPopMatrix();										// Stop effecting our projection matrix

	glMatrixMode(GL_MODELVIEW);							// Go back to our normal model view matrix

	// PHEW!  That was some stuff confusing stuff.  Now we are out of the clear and should have
	// an ID of the object we clicked on.  objectsFound should be at least 1 if we found an object.

	if (objectsFound > 0)
	{		
		// If we found more than one object, we need to check the depth values
		// of all the objects found.  The object with the LEAST depth value is
		// the closest object that we clicked on.  Depending on what you are doing,
		// you might want ALL the objects that you clicked on (if some objects were
		// behind the closest one), but for this tutorial we just care about the one
		// in front.  So, how do we get the depth value?  Well, The selectionBuffer
		// holds it.  For every object there is 4 values.  The first value is
		// "the number of names in the name stack at the time of the event, followed 
		// by the minimum and maximum depth values of all vertices that hit since the 
		// previous event, then followed by the name stack contents, bottom name first." - MSDN
		// The only ones we care about are the minimum depth value (the second value) and
		// the object ID that was passed into glLoadName() (This is the fourth value).
		// So, [0 - 3] is the first object's data, [4 - 7] is the second object's data, etc...
		// Be carefull though, because if you are displaying 2D text in front, it will
		// always find that as the lowest object.  So make sure you disable text when
		// rendering the screen for the object test.  I use a flag for RenderScene().
		// So, lets get the object with the lowest depth!		

		// Set the lowest depth to the first object to start it off.
		// 1 is the first object's minimum Z value.
		// We use an unsigned int so we don't get a warning with selectBuffer below.
		unsigned int lowestDepth = selectBuffer[1];

		// Set the selected object to the first object to start it off.
		// 3 is the first object's object ID we passed into glLoadName().
		int selectedObject = selectBuffer[3];

		// Go through all of the objects found, but start at the second one
		for(int i = 1; i < objectsFound; i++)
		{
			// Check if the current objects depth is lower than the current lowest
			// Notice we times i by 4 (4 values for each object) and add 1 for the depth.
			if(selectBuffer[(i * 4) + 1] < lowestDepth)
			{
				// Set the current lowest depth
				lowestDepth = selectBuffer[(i * 4) + 1];

				// Set the current object ID
				selectedObject = selectBuffer[(i * 4) + 3];
			}
		}
		
		// Return the selected object
		return selectedObject;
	}

	// We didn't click on any objects so return 0
	return 0;											
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
	int objectID = 0;

    switch (uMsg)
	{ 
    case WM_SIZE:										// If the window is resized

		SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
		GetClientRect(hWnd, &g_rRect);					// Get the window rectangle
        break; 
 
	case WM_PAINT:										// If we need to repaint the scene
		BeginPaint(hWnd, &ps);							// Init the paint struct		
		EndPaint(hWnd, &ps);							// EndPaint, Clean up
		break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// If we click the left mouse button, we need to process our X and Y coordinates and
	// see if we hit any objects.  Remember, the LOWORD and HIWORD of the lParam are the (x,y)
	// coordinates of the mouse.  We call RetrieveObjectID() to get the object ID we clicked on.

	case WM_LBUTTONDOWN:								// If the left mouse button was clicked

		// Here we pass in the cursors X and Y coordinates to test for an object under the mouse.
		objectID = RetrieveObjectID(LOWORD(lParam), HIWORD(lParam));

		// Now we just do a switch on our object ID's to see if we hit one.

		switch(objectID)								// Check the objectID passed back
		{
		case SUN:										// We hit the Sun!
			MessageBox(NULL, "The Sun!", "Click", MB_OK);
			break;
		case EARTH:										// We hit the Earth!
			MessageBox(NULL, "The Earth!", "Click", MB_OK);
			break;
		case PLUTO:										// We hit Pluto!
			MessageBox(NULL, "Pluto!", "Click", MB_OK);
			break;
		}
		break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	case WM_KEYDOWN:
		switch(wParam) 
		{
			case VK_ESCAPE:								// Check if we hit the ESCAPE key.
				PostQuitMessage(0);						// Tell windows we want to quit
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
// This is probably our most complicated tutorial in OpenGL so far.  It gets into
// a lot of the tricky parts of matrices.  If you don't understand how a projection
// or model view matrix works, I would suggest looking up the nitty gritty details.
// It helps you really understand what OpenGL is doing.  Of course, you can always
// just shrug it off and say, "Well, I know it works and I have code to prove it!",
// but this will come to haunt you later when you want to do your own stuff.  You won't
// know what to do and how to effectively use OpenGL.  Eventually we will write some papers
// on this subject, but for now we want to focus mostly on code tutorials.
//
// So, how do you feel?  Do you have tons of ideas yet on what this could be used for?
// The first thing that comes to mind for me is interface stuff.  If you are going to
// have cool 3D menus, or a level editor you need to be able to identify what the heck
// the user is trying to click on.
// 
// You will notice that we took out all the full screen code.  That is because it's
// better to see in a window for the first time, also some people's machines handle the cursor
// differently (it won't show up in full screen).  You can mess around with full screen yourself.
// 
// Let's go over some of the steps in order to allow this magic to happen:
// 
// 1)  Once we find the X and Y position we want to check, we then setup our
//     array to hold the selection data.  To do this we call:
// 
//          glSelectBuffer(int SIZE, unsigned int ARRAY);		
// 
// 2) We will need our view port coordinates, so we call:
// 
//	       glGetIntegerv(GL_VIEWPORT, int viewportArray[4]);	
//
// 3) Next, we need to be in projection mode so we can work with 2D in 3D space.
//    After we make this switch, we need to get out of RENDER mode and into SELECT mode:
//
//	       glRenderMode(GL_SELECT);
// 
// 4) Now that we won't be rendering, but getting information, we need to tell OpenGL where
//    to look for the objects.  We do this by creating a region with:
//
//         gluPickMatrix(xPos, yPos, width, height, int viewport[4])
//
// 5) Next, we call gluPerspective() to create the correct projection matrix for that region.
//    We then need to select our mode view matrix in so we can draw the objects into the
//    selection buffer, NOT the frame buffer (because we are in GL_SELECT mode).
//    We call glInitNames() to clear the name stack (It does this anyway when going in GL_SELECT mode)
//
// 6) Before we render an object in, we need to tag an ID with that object so we know how to 
//    reference it later.  To do this, we call:
//
//	       glPushName(ID);  // The ID is our ID for the object to reference later
//    
//    Then we call glPopName(); to stop assigning polygons to that object's ID.
//
// 7) Next, we want to go back to normal render mode.  Since we were just in GL_SELECT
//    mode, we get back the number of objects hit when we call:
//
//	       numberOfObjectsHit = glRenderMode(GL_RENDER);
//
// 8) We close the current projection matrix by switching back to GL_PROJECTION,
//	  then we go back into our model view matrix.
//
// 9) Finally, we should have the object ID's that we clicked stored in selectBuffer.
//    Each object has 4 slots of information [0 - 3] , [4 - 7], etc...
//    We only want the second and fourth because it holds the minimum depth and objectID.
//	  Depending on how many objects we want to check simultaneously, we need to make our 
//    selection buffer bigger (the size should be (4 * objects) since the first 2/3 of the slots
//    holds depth information for each object).  Once again, be aware that if you render
//    2D texture or a fade/flash quad in front of you it will count that as an object clicked.
//    This is bad of course, so find a way (I used a flag) to NOT render those when you check.
//    If you don't call glLoadName() before rendering something, it's objectID will be 0.
//
// !WARNING!  Some cards (like Voodoo) will staul when using the glPushName()/glPopName()
// functions.  So if this tutorial is grinding along on your box, chances are your card does
// not support these calls.
//
// This should be enough information for you to get started.  This is probably going to be one
// of those tutorials that you need to look over again and against until you understand what's 
// going on.  I love OpenGL though, because this seems so magical.  It does so much for you.
// I can just imagine doing this from scratch.  It's nice to have stuff like this hardware
// accelerated.
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
