//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		3DS Animation									 //
//																		 //
//		$Description:	Shows you how to animate .3DS key frame models   //
//																		 //
//***********************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file
#include "3ds.h"										// Include the 3DS header file.

bool  g_bFullScreen = true;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial explains how to read in simple key frame animation from .3DS files.
// A key frame is a special frame in the animation where you apply a rotation, scale
// or translation on an object, and it interpolates between the previous key frame
// so you don't have to do it for every frame.  If you want to move some object
// from one place to another smoothly, just set a key frame to the new position
// on a new frame, and depending on how many frames are between the beginning position,
// it will quickly or slowly move to the new position that was set.  You can have as
// many key frames are there are frames in the animation.  This is not a skeletal (bone)
// animation tutorial, that will come later.  This is just an introduction to animation
// in 3D.  Of course, you eventually want to move up to bone animation.  If you decide
// to use this type of animation, it should go in a robust class, not like I have done here.
// The controls for this tutorial are the same as the last 3DS loading tutorial:

// Left Mouse Button - Changes the Render mode from normal to wireframe.
// Right Mouse Button - Turns lighting On/Off
// Left Arrow Key - Spins the model to the left
// Right Arrow Key - Spins the model to the right
// Escape - Quits

#define FILE_NAME  "Character.3ds"						// This is the 3D file we will load.

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

UINT g_Texture[MAX_TEXTURES] = {0};						// This holds the texture info, referenced by an ID

CLoad3DS g_Load3ds;										// This is 3DS class.  This should go in a good model class.
t3DModel g_3DModel;										// This holds the 3D Model info that we load in

int   g_ViewMode	  = GL_TRIANGLES;					// We want the default drawing mode to be normal
bool  g_bLighting     = true;							// Turn lighting on initially
float g_RotateX		  = 0.0f;							// This is the current value at which the model is rotated
float g_RotationSpeed = 0.2f;							// This is the speed that our model rotates.  (-speed rotates left)


///////////////////////////////// DRAW 3D GRID \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function create a simple grid to get a better view of the animation
/////
///////////////////////////////// DRAW 3D GRID \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Draw3DSGrid()
{
	// Turn of lighting if it's already on
	if(g_bLighting) glDisable(GL_LIGHTING);

	// Turn the lines GREEN
	glColor3ub(0, 255, 0);

	// Draw a 10x10 grid along the X and Z axis'
	for(int x = -1000; x <= 1000; x += 10)
	{
		// Start drawing some lines
		glBegin(GL_LINES);

			// Do the horizontal lines (along the X)
			glVertex3f(-1000, 0, (GLfloat)x);
			glVertex3f(1000, 0, (GLfloat)x);

			// Do the vertical lines (along the Z)
			glVertex3f((GLfloat)x, 0, -1000);
			glVertex3f((GLfloat)x, 0, 1000);

		// Stop drawing lines
		glEnd();
	}

	// Turn the lines YELLOW
	glColor3ub(255, 255, 0);

	// Increase the width of the lines to better show the (X, Y, Z) axis
	glLineWidth(2.5);

	// Start drawing lines
	glBegin(GL_LINES);

			// Do the X Axis
			glVertex3f(-1000, 0.01f, 0);
			glVertex3f(1000, 0.01f, 0);

			// Do the Y Axis
			glVertex3f(0, 0.01f, 0);
			glVertex3f(0, 1000, 0);

			// Do the Z Axis
			glVertex3f(0, 0.01f, -1000);
			glVertex3f(0, 0.01f, 1000);

	// Stop drawing lines
	glEnd();

	// Turn the line color back to normal
	glColor3ub(255, 255, 255);

	// Reset the line width to it's default size
	glLineWidth(1);

	// Turn lighting back on it we had it on previously
	if(g_bLighting) glEnable(GL_LIGHTING);
}



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

	g_Load3ds.Import3DS(&g_3DModel, FILE_NAME);			// Load our .3DS file into our model structure

	// Go through all the materials
	for(int i = 0; i < g_3DModel.numOfMaterials; i++)
	{
		// Check to see if there is a file name to load in this material
		if(strlen(g_3DModel.pMaterials[i].strFile) > 0)
		{
			// Use the name of the texture file to load the bitmap, with a texture ID (i).
			// We pass in our global texture array, the name of the texture, and an ID to reference it.	
			CreateTexture(g_3DModel.pMaterials[i].strFile, g_Texture[i]);			
		}

		// Set the texture ID for this material
		g_3DModel.pMaterials[i].texureId = i;
	}

	glEnable(GL_LIGHT0);								// Turn on a light with defaults set
	glEnable(GL_LIGHTING);								// Turn on lighting
	glEnable(GL_COLOR_MATERIAL);						// Allow color
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

	// Go through all the objects in the scene
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// Free the faces, normals, vertices, and texture coordinates.
		delete [] g_3DModel.pObject[i].pFaces;
		delete [] g_3DModel.pObject[i].pNormals;
		delete [] g_3DModel.pObject[i].pVerts;
		delete [] g_3DModel.pObject[i].pTexVerts;
	}

	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// DELAY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function returns true when we pass the time desired
/////
///////////////////////////////// DELAY \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool Delay(int milliseconds)
{
	// Get the starting time to go off of, we store this in a static variable only once
    static float startTime = (float)GetTickCount();

    // Keep track of the time lapse
    float currentTime = (float)GetTickCount();

    // Check if the time elapsed is great than the time we passed in
    if( currentTime - startTime > milliseconds )
    {
		// Set the new starting time to the current time
        startTime = currentTime;
	
		// Return true to say we elapsed that time
		return true;
    }

	// We haven't elapsed the desired time yet so return false
	return false;
}


///////////////////////////////// ANIMATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function translates, scales and rotates the object depending on the frame
/////
///////////////////////////////// ANIMATE MODEL \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void AnimateModel(t3DModel *pModel, t3DObject *pObject)
{	
	// This is really the main change to this file.  This is what controls
	// the animation for each frame.  Depending on the frame, we translate,
	// then scale, then rotate the object to it's current frames values.
	// For each object they have a list of data for every frame.

	// Get the current position for the current frame
	CVector3 vPosition = pObject->vPosition[pModel->currentFrame];

	// Translate the object to it's current position
	glTranslatef(vPosition.x, vPosition.y, vPosition.z);

	// Get the current scale ratio for the current frame
	CVector3 vScale = pObject->vScale[pModel->currentFrame];

	// Scale the object to the current frame's scale value
	glScalef(vScale.x, vScale.y, vScale.z);

	// Here is when it gets a little strange.  Because we aren't
	// using matrices, we loop through all the previous frames of
	// animation until we get back to the first frame, and call
	// glRotatef() for every frame.  This rotates the object by
	// little increments until it gets to the current frame.  If we
	// used matrices we would just have a rotation matrix that we multiply
	// by the current matrix.  I decided to simplify the tutorial by doing
	// it this way.  Besides, we haven't done a matrix class tutorial yet.
	// When we get into bone animation we will use matrices.  It is a must.
	// We set "i" to 1 because we ignore the first frame of animation (0).
	for(int i = 1; i <= pModel->currentFrame; i++)
	{
		// Get the current rotation degree and axis for the current frame
		CVector3 vRotation = pObject->vRotation[i];
		float rotDegree = pObject->vRotDegree[i];

		// Check if the rotation degree is anything BUT 0. (if we need to rotate)
		if(rotDegree)
		{
			// Rotate the object to the current frame's rotation value
			glRotatef(rotDegree, vRotation.x, vRotation.y, vRotation.z);
		}
	}
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the entire scene.
/////
///////////////////////////////// RENDER SCENE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void RenderScene() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The matrix

	gluLookAt(0, 35, 75,		0, 25, 0,			0, 1, 0);
	
	// We want the model to rotate around the axis so we give it a rotation
	// value, then increase/decrease it. You can rotate right of left with the arrow keys.

	glRotatef(g_RotateX, 0, 1.0f, 0);						// Rotate the object around the Y-Axis
	g_RotateX += g_RotationSpeed;							// Increase the speed of rotation


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Here we draw a grid to give us a better representation of the model and it's size
	Draw3DSGrid();								

	// Below, we need to increase the frame, but first we put a delay between
	// each increase so that it doesn't go super fast.  I believe the average frame speed 
	// is ~~ 16 milliseconds in 3D Studio Max.  You can change this to what ever you want.
	// When ever Delay() returns true it is time to increase to the next frame of animation.

	// Check if we can increase to the next frame of animation yet
	if(Delay(16))
	{
		// Increase the frame count but mod it by the total amount of frames
		g_3DModel.currentFrame = (g_3DModel.currentFrame) % (g_3DModel.numberOfFrames) + 1;
	}

	// * IMPORTANT *
	//
	// We do NOT want to animation frame 0 of the animation because it screws
	// up the animation sometimes.  If you have rotations in your model in frame 0
	// it will store that rotation in the animation, which messes it up because
	// when you save the model, the vertices already reflect that rotation.  SO, we just
	// ignore frame 0.  If you need frame 0 for some reason, refer to the Quick Notes
	// for more information on how to make it work (Hint, Reset The Transforms for the object).

	// Make sure we skip frame 0 of the animation
	if(g_3DModel.currentFrame == 0) g_3DModel.currentFrame = 1;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	
	// Since we know how many objects our model has, go through each of them.
	for(int i = 0; i < g_3DModel.numOfObjects; i++)
	{
		// Make sure we have valid objects just in case. (size() is in the vector class)
		if(g_3DModel.pObject.size() <= 0) break;

		// Get the current object that we are displaying
		t3DObject *pObject = &g_3DModel.pObject[i];


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Push on a new matrix for every object so we don't translate/rotate/scale for another
		glPushMatrix();

		// Animated the model with the scale, rotation and translation
		AnimateModel(&g_3DModel, pObject);

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


		// Check to see if this object has a texture map, if so bind the texture to it.
		if(pObject->bHasTexture) {

			// Turn on texture mapping and turn off color
			glEnable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);

			// Bind the texture map to the object by it's materialID
			glBindTexture(GL_TEXTURE_2D, g_Texture[pObject->materialID]);
		} else {

			// Turn off texture mapping and turn on color
			glDisable(GL_TEXTURE_2D);

			// Reset the color to normal again
			glColor3ub(255, 255, 255);
		}

		// This determines if we are in wireframe or normal mode
		glBegin(g_ViewMode);					// Begin drawing with our selected mode (triangles or lines)

			// Go through all of the faces (polygons) of the object and draw them
			for(int j = 0; j < pObject->numOfFaces; j++)
			{
				// Go through each corner of the triangle and draw it.
				for(int whichVertex = 0; whichVertex < 3; whichVertex++)
				{
					// Get the index for each point of the face
					int index = pObject->pFaces[j].vertIndex[whichVertex];
			
					// Make sure the normals were calculated
					if(pObject->pNormals) 
					{
						// Give OpenGL the normal for this vertex.
						glNormal3f(pObject->pNormals[ index ].x, pObject->pNormals[ index ].y, pObject->pNormals[ index ].z);
					}
				
					// If the object has a texture associated with it, give it a texture coordinate.
					if(pObject->bHasTexture) {

						// Make sure there was a UVW map applied to the object or else it won't have tex coords.
						if(pObject->pTexVerts) {
							glTexCoord2f(pObject->pTexVerts[ index ].x, pObject->pTexVerts[ index ].y);
						}
					} else {

						// Make sure there is a valid material/color assigned to this object.
						// You should always at least assign a material color to an object, 
						// but just in case we want to check the size of the material list.
						// if the size is at least one, and the material ID != -1,
						// then we have a valid material.
						if(g_3DModel.pMaterials.size() && pObject->materialID >= 0) 
						{
							// Get and set the color that the object is, since it must not have a texture
							BYTE *pColor = g_3DModel.pMaterials[pObject->materialID].color;

							// Assign the current color to this model
							glColor3ub(pColor[0], pColor[1], pColor[2]);
						}
					}

					// Make sure we have valid vertices
					if(pObject->pVerts) 
					{
						// Pass in the current vertex of the object (Corner of current face)
						glVertex3f(pObject->pVerts[ index ].x, pObject->pVerts[ index ].y, pObject->pVerts[ index ].z);
					}
				}
			}

		glEnd();								// End the drawing


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

		// Pop the current matrix and start fresh for another object
		glPopMatrix();

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	}

	SwapBuffers(g_hDC);									// Swap the backbuffers to the foreground
}

///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the window messages.
/////
///////////////////////////////// WIN PROC \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

LRESULT CALLBACK WinProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Below we define our controls for this simple tutorial.
	// The controls are: 

	// Left Mouse Button - Changes the Render mode from normal to wireframe.
	// Right Mouse Button - Turns lighting On/Off
	// Left Arrow Key - Spins the model to the left
	// Right Arrow Key - Spins the model to the right
	// Escape - Quits

	case WM_LBUTTONDOWN:								// If the left mouse button was clicked
		
		if(g_ViewMode == GL_TRIANGLES) {				// We our drawing mode is at triangles
			g_ViewMode = GL_LINE_STRIP;					// Go to line stips
		} else {
			g_ViewMode = GL_TRIANGLES;					// Go to triangles
		}
		break;

	case WM_RBUTTONDOWN:								// If the right mouse button was clicked.
		
		g_bLighting = !g_bLighting;						// Turn lighting ON/OFF

		if(g_bLighting) {								// If lighting is ON
			glEnable(GL_LIGHTING);						// Enable OpenGL lighting
		} else {
			glDisable(GL_LIGHTING);						// Disable OpenGL lighting
		}
		break;

	case WM_KEYDOWN:									// If we pressed a key

		switch(wParam) {								// Check if we hit a key
			case VK_ESCAPE:								// If we hit the escape key
				PostQuitMessage(0);						// Send a QUIT message to the window
				break;

			case VK_LEFT:								// If the LEFT arrow key was pressed
				g_RotationSpeed += 0.01f;				// Decrease the rotation speed (eventually rotates left)
				break;

			case VK_RIGHT:								// If the RIGHT arrow key is pressed
				g_RotationSpeed -= 0.01f;				// Increase the rotation speed (rotates right)
				break;
		}
		break;

    case WM_CLOSE:										// If the window is being closed
        PostQuitMessage(0);								// Send a QUIT Message to the window
        break; 
     
    default:											// Return by default
        return DefWindowProc (hWnd, uMsg, wParam, lParam); 
    } 
 
    return 0;											// Return 0 if we handled the message
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// This file has support for key frame animation, but is still missing bones.
// Skeletal animation will be in the next tutorial on animation.  This is a HUGE
// concept so this will most likely be done in about 3 seperate tutorials.
//
// * Note *
// 
// 3DS max isn't the best program for animation, but It seems to be the most popular
// in the game industry.  This animation tutorial is not the most robust it could be,
// but should be taken as a learning tool to creating a better animation system.  This
// should be a step up to bone animation, which is the BEST way to make cool characters.
// This animation is just objects rotating, scaling and translating.  It is how they
// used to do it back in the 80's early 90's.  So don't try and take a cool model
// and put bones and IK on it, animate it and expect anything to happen :).  
// One thing to keep in mind, remember that we IGNORE the frame 0 of the animation.
// If you want to use frame zero for some reason, here is a way that it won't screw up
// the rotation key frames (it saves the rotation of the object for the first frame (0)).
// After you model your scene, before you start animating it, select all the objects,
// go to the heirarchy tab (On the right of the modify tab) and select the button
// called "Transform" under the "Reset" section.  If you don't and you have rotations in
// int the first frame (0), it will screw up the animation.  Please refer to the HTML tutorial
// on how to create this type of animation and the steps to take.  This should always 
// accompany that HTML tutorial.  So remember, we want to ignore frame 0 of the animation,
// but if you need to use it, be sure to reset the transforms for every object in your model.
//
// Okay, now that I covered my back :)  Let's start talking about how this works!
// Once again, refer to the .3DS loader tutorial if you are confused on how to do 
// the basics of loading data from a .3DS file.  It is at www.GameTutorials.com in
// the OpenGL section.
//
// The basic concepts of key frame animation is this:  You store a position, rotation
// and scale value on certain frames, and it interpolates between that data.  Let's say,
// if you start at frame 0, with a box centered around (0, 0, 0), then go to frame 60
// and center the box around (10, 10, 10), THEN play the animation it should move the
// box from (0, 0, 0) to (10, 10, 10) smoothly.  The same thing works for rotations
// and scales.  You just set "KEY" frames and it does the rest for you.  The weird thing
// with 3DS files is that it saves the position and scale EVERY frame between each key frame,
// buton only saves the rotation values for each key frame.  That means that we still have
// to do the interpolation for the rotation, where we get the position and scale value for
// every frame, so they did the interpolation for us.  
//
// So that is all we are doing, just reading in the positions/Scale value for ever frame,
// and if there isn't a new position/scale value for the next frame we just set the rest
// of the position/scale values to the last one.  The same thing with rotations, but we
// first get the key frames rotation degree and axis it's rotation along, inteprolate
// between key frames, then if there is no more rotating we just set the rest of the
// frames rotation degrees to 0.  This should ideally be done with a matrix, where it
// matrix multiplies the rotation degree and axis by the current rotation matrix but
// I didn't want to complicate the tutorial any more.  Any other animation tutorials will
// use matrices.  Instead, I just loop through all the degrees and call glRotatef() for
// every frame that has happened.  That way it's doing the same thing, but less efficient.
// 
// There is some data that we ignore in the KEYFRAME chunk, but it will be explained later
// in another tutorial.  This is the jist of it.  Go through the code and read the comments
// around each function to get a more in depth idea of what is going on.  It's pretty basic
// stuff, just a pain to get the data in the format you want it.
//
// I would like to thank www.wosit.org and Terry Caton (tcaton@umr.edu) for his help on this.
//
// Let me know if this helps you out!
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
//
//

