//********************************************************************************//
//																				  //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -				  //
//																				  //
//		$Author:		DigiBen		digiben@gametutorials.com					  //
//																				  //
//		$Program:		MD3 Animation											  //
//																				  //
//		$Description:	Demonstrates animating Quake3 characters with quaternions //
//																				  //
//********************************************************************************//

// This is a compiler directive that includes libraries (For Visual Studio).
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

#include "main.h"										// This includes our header file
#include "Md3.h"										// Include the MD3 header file.


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

// This tutorial is the second MD3 tutorial.  In this version we will be able to load and
// animate the Quake3 character using quaternions.    Be sure you have read the MD3 Loader
// and Quaternions tutorial on our site at www.GameTutorials.com in the OpenGL section.
// This tutorial was written assuming you did, which enables us to cut out a ton of comments
// and explanations that can be found in the previous tutorials.  I will leave some comments
// to refresh your memory, but most of the large blocks will be taken out.
// 
// Okay, so what's different from this tutorial and the last one?  Well, if you run the
// program you will see that our character is animating.  If you click the right and let
// mouse buttons you can cycle through the torso and leg animations.  The name of the
// animations are also displayed in the title bar.  So the question is, how much of the
// code changed to make this happen?  Unfortunately, it's just a couple simple additions,
// but it isn't a bunch of complicated ones either.  The only complicated code is the
// quaternion code.  As I explained in the first quaternions tutorial, the math is quite
// complex to wrap your head around.  Don't stress out about trying to understand why it works,
// it's sufficient enough to understand the equations needed to perform the desired task.
// It goes without saying that if you do have a desire to delve into the math and proofs,
// you will be a better person for it, for it can only help you.  I will NOT be going over
// the theory or proofs beneath the quaternion equations.  Just get a math book.  I will
// talk more about the quaternions in Md3.cpp.  Here is a address to a really detailed PDF 
// file written by Ken Shoemake on quaternions:
//
// http://www.daimi.au.dk/~mbl/cgcourse/wiki/_files/shoemake94.pdf
//
// As well as another great PDF from Magic Software here:
//
// http://www.magic-software.com/Documentation/quat.pdf
//
// One of the most notable changes added to this tutorial is that we now read in the
// animation config file, with the .cfg extension.  Remember from the last tutorial that
// only the torso (upper) and the legs (lower) have animation associated with them.  If you
// want the head to animate, the artist will need to make the head part of the upper.md3 file.
// This is why on some models you will see a small head when you turn on wire frame mode.
// The config file stores all of the animations and their respective frame data.  For example, 
// here is a few lines from a config file:
//
//		0	31	0	25		// BOTH_DEATH1
//		31	1	1	25		// BOTH_DEAD1
//		32	35	0	20		// BOTH_DEATH2
//
// These 3 lines indicate 3 animations that both the torso and legs must perform.
// The four numbers in front of the animation name store the starting frame, the ending
// frame, the number of looping frames (ignored) and the frames per second the animation runs.
// In the upper.md3 and the lower.md3, vertex key frames are saved, almost exactly like the
// .MD2 file format is saved.  The first 2 numbers are indices into those key frames, which
// are loading with the vertex array.  They aren't actually indexed directly, but depending
// on the current key frame, you would say:
//
// currentIndex = pModel->currentFrame * pObject->numOfVerts;
//
// Since all the key frames are stored in a 1 dimensional array, we treat it like a 2D
// array by multiplying the current frame by the number of vertices of the model (which
// does not count the key frames, just vertex count for one frame).  That new index is
// used to index into the vertex array of the current model.
//
// There are 3 blocks of animations in the config file.  They are BOTH_*, TORSO_* and LEGS_*.
// This is because the torso model can be doing a different animation that has nothing to do
// with the legs animation.  For instance, the character can be doing a shooting animation,
// while they are jumping, landing, turning, swimming, etc...
//
// It is assumed that by reading this tutorial you understand a decent amount of OpenGL and
// are comfortable with advanced topics.  This is a extremely advanced topic and should not
// be tackled until you get to a certain level of understanding in 3D.  I would recommend
// people who might not be up to that level to try looking at the MD2 Loader and animation.
// This is still an advanced tutorial, but not as complicated.  It only gets harder from here.
// And remember, copy and pasting of code hurts small children in Africa :)
//
// The last notable change from the last tutorial is that we now use a matrix to perform the
// translations and rotations of our model's body parts for animation.  Since we are using 
// quaternions, and the fact that the key frame rotations are stored in a 3x3 matrix. this 
// is necessary.  The rest of the functions and things added are just smaller helper functions.
//
// Before we get to the code, I would like to remind those who might be lost, to go back to
// the original MD3 Loader tutorial and see if there is an explanation that you missed and was
// taken out of this version.
// 
// The controls for this application are:
//
// Right Mouse Click - Cycles through the upper (torso) animations
// Left Mouse Click  - Cycles through the lower (legs) animations		
// Left Arrow Key - Spins the model to the left
// Right Arrow Key - Spins the model to the right
// Up Arrow Key - Moves the camera closer to the model
// Right Arrow Key - Moves the camera farther away from the model
// W - Changes the Render mode from normal to wire frame.
// Escape - Quits
//
// * Note * Keep in mind that when you change animations it is not immediately.  It waits to
// finish the current animation that it's on.  This can be changed easily if you read 
// the note in IncreaseCharacterAnimation() below.
//
//

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

#define MODEL_PATH  "elf"				// This stores the folder name the character is in
#define MODEL_NAME	"elf"				// This stores the prefix for all the character files
#define GUN_NAME    "Railgun"			// This stores the name of our gun to load

bool  g_bFullScreen = true;				// Set full screen as default
HWND  g_hWnd;							// This is the handle for the window
RECT  g_rRect;							// This holds the window dimensions
HDC   g_hDC;							// General HDC - (handle to device context)
HGLRC g_hRC;							// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;					// This holds the global hInstance for UnregisterClass() in DeInit()

UINT g_Texture[MAX_TEXTURES] = {0};		// This holds the texture info, referenced by an ID

CModelMD3 g_Model;						// This is our global md3 character model instance

float g_RotateX		  =    0.0f;		// This is the current value at which the model is rotated
float g_RotationSpeed =    0.1f;		// This is the speed that our model rotates. 
float g_TranslationZ  = -120.0f;		// This stores our distance away from the model
bool  g_RenderMode    =    true;		// This tells us if we are in wire frame mode or not


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

	// Load the 3 body part meshes and their skin, textures and animation config files
	bool bResult = g_Model.LoadModel(MODEL_PATH, MODEL_NAME);

	// Load the gun and attach it to our character
	bResult = g_Model.LoadWeapon(MODEL_PATH, GUN_NAME);


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// When we get here, the character should have everything loaded.  Before going on,
	// we want to set the current animation for the torso and the legs.

	// Set the standing animation for the torso
	g_Model.SetTorsoAnimation("TORSO_STAND");

	// Set the walking animation for the legs
	g_Model.SetLegsAnimation("LEGS_WALK");

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	glEnable(GL_CULL_FACE);								// Turn back face culling on
	glCullFace(GL_FRONT);								// Quake3 uses front face culling apparently

	glEnable(GL_TEXTURE_2D);							// Enables Texture Mapping
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
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

	// Notice that nothing has changed in the rendering, it's still the same (* NEW *)

	// Give OpenGL our position,			then view,			then up vector
	gluLookAt(	0, 5.5f, g_TranslationZ,	0, 5.5f, 0,			0, 1, 0);
	
	// We want the model to rotate around the axis so we give it a rotation
	// value, then increase/decrease it. You can rotate right or left with the arrow keys.

	glRotatef(g_RotateX, 0, 1.0f, 0);			// Rotate the object around the Y-Axis
	g_RotateX += g_RotationSpeed;				// Increase the speed of rotation

	g_Model.DrawModel();						//  Render our character to the screen

	SwapBuffers(g_hDC);							// Swap the backbuffers to the foreground
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

////////////////////////// INCREASE CHARACTER ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function increases the model's torso or legs animation
/////
////////////////////////// INCREASE CHARACTER ANIMATION \\\\\\\\\\\\\\\\\\\\\\\\\\*

void IncreaseCharacterAnimation(CModelMD3 *pCharacter, int whichPart)
{
	char strWindowTitle[255] = {0};
	t3DModel *pModel, *pUpper, *pLower;

	// This function doesn't have much to do with the character animation, but I
	// created it so that we can cycle through each of the animations to see how
	// they all look.  You can press the right and left mouse buttons to cycle through
	// the torso and leg animations.  If the current animation is the end animation,
	// it cycles back to the first animation.  This function takes the character you
	// want, then the define (kLower, kUpper) that tells which part to change.

	// Here we store pointers to the legs and torso, so we can display their current anim name
	pLower = pCharacter->GetModel(kLower);
	pUpper = pCharacter->GetModel(kUpper);

	// This line gives us a pointer to the model that we want to change
	pModel = pCharacter->GetModel(whichPart);

	// To cycle through the animations, we just increase the model's current animation
	// by 1.  You'll notice that we also mod this result by the total number of
	// animations in our model, to make sure we go back to the beginning once we reach
	// the end of our animation list.  

	// Increase the current animation and mod it by the max animations
	pModel->currentAnim = (pModel->currentAnim + 1) % (pModel->numOfAnimations);

	// Set the current frame to be the starting frame of the new animation
	pModel->nextFrame = pModel->pAnimations[pModel->currentAnim].startFrame;

	// (* NOTE *) Currently when changing animations, the character doesn't immediately
	// change to the next animation, but waits till it finishes the current animation
	// and slowly blends into the next one.  If you want an immediate switch, change
	// the pModel-nextFrame to pModel->currentFrame.

	// Display the current animations in our window's title bar
	sprintf(strWindowTitle, "www.GameTutorials.com - Md3 Animation:   Lower: %s  Upper: %s", 
			pLower->pAnimations[pLower->currentAnim].strName, 
			pUpper->pAnimations[pUpper->currentAnim].strName);

	// Set the window's title bar to our new string of animation names
	SetWindowText(g_hWnd, strWindowTitle);
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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

	// Below we define our controls for this tutorial. The controls are: 

	// Right Mouse Click - Cycles through the upper (torso) animations
	// Left Mouse Click  - Cycles through the lower (legs) animations	
	// Left Arrow Key - Spins the model to the left
	// Right Arrow Key - Spins the model to the right
	// Up Arrow Key - Moves the camera closer to the model
	// Right Arrow Key - Moves the camera farther away from the model
	// W - Changes the Render mode from normal to wire frame.
	// Escape - Quits


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	case WM_LBUTTONDOWN:								// If the left mouse button was clicked
		
		// Increase the leg's current animation to the next animation
		IncreaseCharacterAnimation( &g_Model, kLower);
		break;

	case WM_RBUTTONDOWN:								// If the right mouse button was clicked

		// Increase the torso's current animation to the next animation
		IncreaseCharacterAnimation( &g_Model, kUpper );
		break;

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


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
// You probably noticed that there wasn't much changed in this file.  Most of the
// changes happened in Md3.cpp.  Since I wrote this tutorial before writing the
// previous tutorial, I was able to eliminate tons of little changes, due to the
// differences that would have been added with the animation.  That way you don't
// need to look at a bunch of changes as much as the additions of code.
// 
// Below is a excerpt from Md3.cpp explaining more additions to the previous tutorial:
// 
// -----
//
// First of all, we added a basic quaternion class to our tutorial.  This is used
// to take a matrix, convert it to a quaternion, interpolate between another 
// quaternion that was converted to a matrix, then turned back into a matrix.
// This is because quaternions are a great way to interpolate between rotations.
// If you wanted to use the glRotatef() and glTranslatef() functions, you would
// want to convert the interpolated quaternion to an axis angle representation,
// which might be less code, but not as useful in the long run.
// 
// The next important thing that was added was the interpolation between vertex
// key frames.  We learned earlier (top of Md3.cpp) that not most of the animation
// is done through key frame vertices, not bones.  The only bone animation that is
// done is with the joints that connect the .md3 models together.  The game Half-Life, 
// for example, uses a full on skeletal animation.
// 
// Also, in this tutorial we added the code to parse the animation config file (.cfg).
// This stores the animation information for each animation.  The order of them are
// important.  For the most part, the config files are the same format.  As discussed
// in the previous MD3 tutorial, there is a few extra things in the config file that
// we don't read in here, such as footstep sounds and initial positions.  The tutorial
// was not designed to be a robust reusable Quake3 character loader, but to teach you
// how it works for the most part.  Other things are extra credit :)
//
// Another important thing was our timing system.  Since we were dealing with multiple
// models that had different frames per second, we needed to add some variables to our
// t3DModel class to hold some things like elapsedTime and the current t value.  This
// way, no static variables had to be created, like in the .MD2 tutorial.
//
// ----- 
//
// Be sure to check out the top of Md3.cpp for a more detailed explanation of animation
// and interpolation.
//
// Let me know if this helps you out!
//
// The Quake3 .Md3 file format is owned by ID Software.  This tutorial is being used 
// as a teaching tool to help understand model loading and animation.  This should
// not be sold or used under any way for commercial use with out written consent
// from ID Software.
//
// Quake, Quake2 and Quake3 are trademarks of ID Software.
// All trademarks used are properties of their respective owners. 
//
//
// © 2000-2005 GameTutorials
