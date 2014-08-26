//********************************************************************************//
//																				  //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -				  //
//																				  //
//		$Author:		DigiBen		digiben@gametutorials.com					  //
//																				  //
//		$Program:		GT Loader Part 3										  //
//																				  //
//		$Description:	Loads the GT Skeletal Animation format .GTF				  //
//																				  //
//********************************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

#include "main.h"										// This includes our header file
#include "gtf.h"										// Include the GTF header file.


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This is the long-awaited tutorial on bone animation.  We will be loading
// our own .GTF format for the animation data.  This file format was exported
// from 3D Studio Max.  You can find the format for this file below.  It is 
// assumed that you have gone through all of the previous tutorials in this 
// loader/exporter series and understand the concepts.  Below we explain
// again what bone animation is and the difference with weighted animation.
// 
// * What is bone animation? *
//
// If you have never worked with bone animation, let's go over the basics
// of how it works.  When creating a character, artists will usually make
// a character with their arms pointing outward.  This is because it's
// easier to apply bones to the character that way.  Usually the character
// is one or so objects, so in order to animate the object(s) you need bones
// that are attached to the model to influence the mesh.  Once you attach
// the bones like a skeleton you can move the bones and they will move the
// vertices that are closest to the bone.  It's like a puppet.
//
// When exporting the bone animation data, you export a matrix for every
// bone and for every frame of the animation.  We want to use quaternions
// so we export a separate translation position and the quaternion rotation.
//
// * What is weighted bone animation? *
//
// Weighted bone animation means that if you have a huge fat character that 
// you want his chubby stomach to bounce around to simulate realism, Max 
// allows us to apply special weighting to different parts of the body.
// For every vertice there is a weight attached that influences it.  If
// there are many different weights applied to the model then all those
// weights are factored in as well.
//
// * How does weighted bone animation work? *
//
// Let's go through briefly the process of how weighted bone animation works.
// First, we need have a current frame and the next frame of animation.  We
// use those numbers to index into our bone animation lists.  We basically
// take the current frame and the next frame's rotation and do spherical
// linear interpolation (SLERP) between the two.  Then we multiply that
// by the current vertex position.  The next thing is to add the translation
// value to the rotated position, but we don't just add the translation,
// we do linear interpolation between the current translation and the next
// frame of animation's translation.  That result is then added to our
// rotated position.  We have a running "t" for the time in the interpolation
// that is calculated every frame, depending on the frame rate of the animation.
//
// Finally, we need to add the weights to our final vertex.  This whole process 
// is done as many times as there is weights for the model (numBlendLinks).  If 
// there is weighted animation then the numBlendLinks is usually the amount of
// bones in the model.  For every bone's weight we apply it to the new position
// and keep adding up the result for every weight.  If there is no weighted 
// animation then there is only one weight link and it has a weight value of 1.0,
// which means you apply the new position by 100%.
//
// This process will be explained in detail in the function RenderModel(),
// located in GTF.cpp.
//
// * Additions *
//
// We added a great deal of math to our project in order to make the animation
// work.  This means there is a bit of matrix, quaternion and linear algebra
// functions added.  You will notice that we grew our CVector3 class to have
// some basic functionality with operator overloading.  Most of the math code 
// is taken from the MD3 animation tutorial, especially the quaternions code.
// I will leave a lot of the comments that describe much of the functions in
// case you haven't viewed the MD3 tutorial and/or you don't understand the
// concepts.
//
// Among other things, we added the code to load the animation data from our
// .GTF file.  Since animation is dependant on timing, we also added a couple
// functions to update the timing for the desired frame rate of the animation.
// Lists and other variables were added to some of our model structures to
// handle the bone animation data.  Finally, we added the code to actually
// perform the bone animation in RenderModel().
//
// * We won't handle separate animations *
//
// In this tutorial we won't be handling animations like in our previous 
// animation tutorials.  We will just loop through a walk cycle animation 
// and not select specific animations.  I figure that you can do that yourself 
// and have your own desired format for controlling which animations are playing, 
// also it would just bloat our code and make it more confusing.  Check out the 
// MD2 and MD3 animation tutorials for examples.
//
// Also, in the original Vick.max file we have all the animations for an MD3 model.
// We just exported the Vick_Walk.max file to show a single animation looping over
// again.  If you export the Vick.max file and watch the animations being played you
// might see some stretching between some of the transitions to different animations.
// This is because you will not want to try and interpolate the last frame of an
// animation cycle with the first frame of another animation cycle.  Depending on the
// positions of the 2 frames it will sometimes create a funny stretching effect.  To
// get around this you can do 2 things:
//
// 1) When the last frame is played of an animation cycle just set the vertices to
//    the first frame of the next animation cycle.  This is of course a quick jump
//    that isn't the best option, but get's rid of ugly stretching that appears
//    sometimes.  Most older games use this method because it's the easiest.
//
// 2) The best way to switch between animations is to use a method of called 
//    "animation blending".  This doesn't instantly switch animations in a frame, but
//    takes a couple to a bunch of frames (however fast you need to transition) and
//    blends the bone animation through interpolation.  Imagine a character walking
//    and then slowly the animation turns into a run.  That is the effect that you
//    want.
//
//
// * GTF file format *
//  
// Below is the new format with the added material data:
//
// * "GT3D" (4 characters)
// * Exporter Version (1 float)
// * Number of Objects/Meshes (1 int)
// *	Number of Materials (1 int)
// *	Material Name ( Number of Materials * char[MAX_FILE_NAME])
// *	Number of Vertices (1 int)
// *	Vertices ( Number of Vertices * CVector3)
// *	Number of Indices (1 int)
// *	Indices ( Number of Indices * int)
// *	Number of UV Coordinates (1 int)
// *	UV Coords ( Number of UV Coords * CVector3)
// *	Number of Texture Indices (1 int)
// *	Texture Indices ( Number of Texture Indices * int)
// 
// *	Has Skeletal Anim Bool ( 1 bool )
// *	Number of Weights ( 1 int )
// *	Weight Info ( Number of Weights * CVector2)
// *	Frame Rate ( 1 int )
// *	Number of Frames ( 1 int )
// *		Translation (1 CVector3 )
// *		Quaternion ( 1 CVector4 )
//
// 
//
//
// * Application Controls *
//
// ESC   - Quits the program
// W     - Turns wireframe on and off
// LEFT  - Rotates the model left
// RIGHT - Rotates the model right
//
// Now let's get on with the tutorial and go view the code.  There isn't
// anything in this file that has changed, so you can just right into
// GTF.cpp.  Be sure to also check out the header files, as they have
// changed a bit too.

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


bool  g_bFullScreen = true;				// Set full screen as default
HWND  g_hWnd;							// This is the handle for the window
RECT  g_rRect;							// This holds the window dimensions
HDC   g_hDC;							// General HDC - (handle to device context)
HGLRC g_hRC;							// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;					// This holds the global hInstance for UnregisterClass() in DeInit()
float g_DT;								// This holds the time slice for each frame update

UINT g_Texture[MAX_TEXTURES] = {0};		// This holds the texture info, referenced by an ID

char g_szModel[] = "Vick.gtf";			// This is the model that we will be loading
char g_szModelPath[] = "Model";			// This is the directory that the file is located

CModelGTF g_Model;						// This is our global GTF character model instance

float g_RotateX		  =    0.0f;		// This is the current value at which the model is rotated
float g_RotationSpeed =    0.1f;		// This is the speed that our model rotates. 
float g_TranslationZ  =  -90.0f;		// This stores our distance away from the model
bool  g_RenderMode    =    true;		// This tells us if we are in wire frame mode or not


///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function initializes OpenGL and other stuff
/////
///////////////////////////////// INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void Init(HWND hWnd)
{
	g_hWnd = hWnd;										// Assign the window handle to a global window handle
	GetClientRect(g_hWnd, &g_rRect);					// Assign the windows rectangle to a global RECT
	InitializeOpenGL(g_rRect.right, g_rRect.bottom);	// Init OpenGL with the global rect

	// Load the .GTF file from the model directory
	bool bResult = g_Model.LoadModel(g_szModelPath, g_szModel);

	glEnable(GL_CULL_FACE);								// Turn back face culling on
	glEnable(GL_TEXTURE_2D);							// Enables Texture Mapping
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
}


///////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function keeps our rendering within a specified frame rate
/////
///////////////////////////////// ANIMATE NEXT FRAME \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = GetTickCount() * 0.001f;
	static float elapsedTime = 0.0f;

	float currentTime = GetTickCount() * 0.001f;	// Get the time (milliseconds = seconds * .001)
	float deltaTime = currentTime - lastTime;		// Get the slice of time
	float desiredFPS = 1.0f / desiredFrameRate;		// Store 1 / desiredFrameRate

	elapsedTime += deltaTime;						// Add to the elapsed time
	lastTime = currentTime;							// Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		g_DT = desiredFPS;
		elapsedTime -= desiredFPS;					// Adjust our elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
}


///////////////////////////////// MAIN LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the main program loop
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
		else											// If there wasn't a message
		{ 
			if(AnimateNextFrame(60))					// Make sure we only render 60 frames per second
			{
				RenderScene();							// Render the scene
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

	// Give OpenGL our position,			then view,			then up vector
	gluLookAt(	0, 25.5f, g_TranslationZ,	0, 25.5f, 0,			0, 1, 0);
	
	// We want the model to rotate around the axis so we give it a rotation
	// value, then increase/decrease it. You can rotate right or left with the arrow keys.

	glRotatef(g_RotateX, 0, 1.0f, 0);			// Rotate the object around the Y-Axis
	g_RotateX += g_RotationSpeed;				// Increase the speed of rotation

	g_Model.DrawModel();

	SwapBuffers(g_hDC);							// Swap the backbuffers to the foreground
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
    case WM_SIZE:								// If the window is resized
		if(!g_bFullScreen)						// Do this only if we are NOT in full screen
		{
			SizeOpenGLScreen(LOWORD(lParam),HIWORD(lParam));// LoWord=Width, HiWord=Height
			GetClientRect(hWnd, &g_rRect);		// Get the window rectangle
		}
        break; 
 
	case WM_PAINT:								// If we need to repaint the scene
		BeginPaint(hWnd, &ps);					// Init the paint struct		
		EndPaint(hWnd, &ps);					// EndPaint, Clean up
		break;


	case WM_KEYDOWN:							// If we pressed a key

		switch(wParam)							// Check if we hit a key
		{
			case VK_ESCAPE:						// If we hit the escape key
				PostQuitMessage(0);				// Send a QUIT message to the window
				break;

			case VK_LEFT:						// If the LEFT arrow key was pressed
				g_RotationSpeed -= 0.05f;		// Decrease the rotation speed (eventually rotates left)
				break;

			case VK_RIGHT:						// If the RIGHT arrow key is pressed
				g_RotationSpeed += 0.05f;		// Increase the rotation speed (rotates right)
				break;

			case VK_UP:							// If the UP arrow key was pressed
				g_TranslationZ += 2;			// Move the camera position forward along the Z axis
				break;

			case VK_DOWN:						// If the DOWN arrow key is pressed
				g_TranslationZ -= 2;			// Move the camera position back along the Z axis
				break;

			case 'W':

				g_RenderMode = !g_RenderMode;	// Change the rendering mode

				// Change the rendering mode to and from lines or triangles
				if(g_RenderMode) 				
				{
					// Render the triangles in fill mode		
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
				}
				else 
				{
					// Render the triangles in wire frame mode
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
				}
				break;
		}
		break;

    case WM_CLOSE:										// If the window is being closed
        PostQuitMessage(0);								// Send a QUIT Message to the window
        break; 
     
    default:											// Return by default
        lRet = (LONG)DefWindowProc (hWnd, uMsg, wParam, lParam); 
        break; 
    } 
 
    return lRet;										// Return by default
}


/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
// 
// Nothing new was added to this file.  Check our GTF.cpp for the main
// additions to this tutorial.
//
//
// © 2000-2006 GameTutorials