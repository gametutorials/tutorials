//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		Masking											 //
//																		 //
//		$Description:	Demonstrates transparency using masking			 //
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

UINT g_Texture[MAX_TEXTURES];							// This will reference to our texture data stored with OpenGL UINT is an unsigned int (only positive numbers)

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

// If you have ever wanted to display only part of an image without having it displayed 
// as a rectangle, such as trees, plants, particles and interfaces, this tutorial
// will demonstrate this important technique.  You may have experimented or seen this
// accomplished with blending.  For example:  glBlendFunc(GL_SRC_ALPHA, GL_ONE) with
// blending turned on (glEnable(GL_BLEND)) will take a texture map and blend it with
// the background.  The closer a color is in the texture map to black, the more it
// will disappear.  If the color being blended is black, it will render the color 
// behind it instead.  This means you can have a picture of a tree with a black
// background and it will only draw the tree and not the black background surrounding it.
// The problem with this effect is that it makes the tree look transparent/faded.  This 
// works fine for particle effects, but if you want the image to be clear and not faded, 
// you will want to either use a 32 bit image with an alpha channel or masking.  Masking
// is basically making it so you only draw the part of an image where it's mask image
// has dark colors.  This means you have 2 images, one for the normal picture, and
// another black and white image that holds the mask.  More of this will be explained
// in the code and in the * QUICK NOTES * section at the bottom of this file.  Due
// to the grouping of our tutorials by subject, you might want to check out the
// matrix tutorial that explains glPushMatrix() and glPopMatrix() if you are confused.
// Basically, it works just like a matrix scope, that takes the properties of the
// current matrix, but if you change the matrix with any OpenGL functions, then
// pop it off, it does not effect the previous matrix you started with.
// 
// To demonstrate masking we will render a scope tunnel with a cross hair.  In between
// the end of the scope we will see our room rendered.  This is where the transparency
// will be.  If you look at the mask bitmap for the cross hair, you will see that this
// area is white, which means it will NOT draw that part of the image when combined
// with the original image.  Only the parts with dark colors will be drawn.  Let's
// go over the controls for this app:
//
// LEFT / RIGHT Arrow Keys or 'a' / 'd' - This rotates the camera's view in a circle
// UP / DOWN Arrow Keys or 'w' / 's' - This makes our scope zoom in and out
// SPACE BAR - This turns night vision on and off
// ESCAPE Key - This quits the program
//
// Besides teaching masking, Ortho mode is also discussed.  Ortho mode allows us
// to use 2D coordinates instead of 3D coordinates.  This is perfect for art that
// needs to stay in front of the screen and never move, such as interface art.
// This is what we do for our scope image, this way it will always be centered on screen.
//
// This tutorial was taken from the original texture mapping tutorial.  
//

// This will store if we want night vision on our off (press the space bar)
bool g_bNightVision = false;

// This is our camera rotation degree
float g_rotateY = 0;

// This holds the zoom value of our scope
float g_zoom = 0;


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

	// Here we load all of our textures.  CreateTexture() is defined in Init.cpp.

	CreateTexture(g_Texture[0], "Wall.bmp");			// Load our texture for the wall
	CreateTexture(g_Texture[1], "floor.bmp");			// Load our floor texture
	CreateTexture(g_Texture[2], "CrossHairMask.bmp");	// Load the cross hair mask used for transparency
	CreateTexture(g_Texture[3], "CrossHair.bmp");	    // Load the scope/cross hair image
	
	// Instead of boring black for the clear color, I wanted to make it a nice sky blue.
	// The parameters are (Red Value, Green Value, Blue Value, Alpha Value).  Now our
	// background will have a blue color, instead of clearing away to black.

	glClearColor(.4f, 0, 1, 1);							// Turn our background a bluish purple
}


///////////////////////////////// KEY PRESSED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles the input checking for the scope movement
/////
///////////////////////////////// KEY PRESSED \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void KeyPressed()
{
	// Like in our camera tutorials, we want to use GetKeyState() for movement
	// because it's a lot faster than checking for input through our WinProc().
	// We do however, check if the user typed SPACE for night vision.  This is
	// because we don't want the key sensitivity to be fast when turning something
	// on and off, otherwise it will flash on and off really fast and we will have
	// to time it to get the effect we want.  To fix that, we would need to create
	// an array for each key and handle the WM_KEYDOWN and WM_KEYUP messages.
	// There is no need to in this tutorial.  In here we check for the rotation
	// and zooming in keys.

	// If we hit the W or the UP arrow key we want to zoom the scope in
	if(GetKeyState(VK_UP) & 0x80 || GetKeyState('W') & 0x80) {	
		
		// Make sure we haven't gone past or max scope range, if not, increase our zoom level
		if(g_zoom < 13) g_zoom += 0.5f;
	}

	// If we hit the s or the DOWN arrow key we want to zoom the scope out
	if(GetKeyState(VK_DOWN) & 0x80 || GetKeyState('S') & 0x80) {				

		// Make sure we don't zoom backwards, if we haven't go below 0, decrease our zoom level
		if(g_zoom > 0)  g_zoom -= 0.5f;
	}

	// If we hit the A or LEFT arrow key we want to rotate the camera left
	if(GetKeyState(VK_LEFT) & 0x80 || GetKeyState('A') & 0x80) {				

		// Decrease our current world rotation degree
		g_rotateY -= 2;
	}

	// If we hit the D or RIGHT arrow key we want to rotate the camera right
	if(GetKeyState(VK_RIGHT) & 0x80 || GetKeyState('D') & 0x80) {				

		// Increase our current world rotation degree
		g_rotateY += 2;
	}
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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

				// We want to use this function instead of the WinProc() because of the smoothness.

				KeyPressed();								// Check for input from the keyboard

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


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

// Here comes the complicated stuff that we don't usually deal with, our projection matrix.
// Basically what is going on with the 2 functions OrthoMode() and PerspectiveMode() is
// we are switching from plotting 3D point to plotting 2D points.  This means that
// instead of giving OpenGL 3D coordinates, in Ortho mode we give it screen pixel coords.
// This is a really useful mode to have because it saves us tons of math when it comes
// to displaying interface art over our 3D screen.  You would have to rotate the
// interface art whenever the camera changes, which could be annoying.  On top of that,
// you need to take into account the depth value, which means you have to find the best
// depth to render the art at.  This can stretch and pixelate the art, making it look bad.
// This is also incredibly faster than doing glDrawPixels(), and you don't have to touch
// the rasterizer, plus you can use blending and other OpenGL effects.

///////////////////////////////// ORTHO MODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function changes our projection mode from 3D to 2D
/////
///////////////////////////////// ORTHO MODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void OrthoMode(int left, int top, int right, int bottom)
{
	// This function takes in rectangle coordinates for our 2D view port.
	// What happens below is, we need to change our projection matrix.  To do this,
	// we call glMatrixMode(GL_PROJECTION) to tell OpenGL the current mode we want.
	// Then we push on a new matrix so we can just call glPopMatrix() to return us
	// to our previous projection matrix, but first we will render in ortho mode.
	//  Next, we load a new identity matrix so that everything is initialize before
	// we go into ortho mode.  One we switch to ortho by calling glOrth(), we then
	// want to set our matrix mode to GL_MODELVIEW, which puts us back and ready to
	// render our world using the model matrix.  We also initialize this new model view
	// matrix for our 2D rendering before drawing anything.

	// Switch to our projection matrix so that we can change it to ortho mode, not perspective.
	glMatrixMode(GL_PROJECTION);						

	// Push on a new matrix so that we can just pop it off to go back to perspective mode
	glPushMatrix();									
	
	// Reset the current matrix to our identify matrix
	glLoadIdentity();								

	//Pass in our 2D ortho screen coordinates.like so (left, right, bottom, top).  The last
	// 2 parameters are the near and far planes.
	glOrtho( left, right, bottom, top, 0, 1 );	
	
	// Switch to model view so that we can render the scope image
	glMatrixMode(GL_MODELVIEW);								

	// Initialize the current model view matrix with the identity matrix
	glLoadIdentity();										
}


///////////////////////////////// PERSPECTIVE MODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function changes our returns our projection mode from 2D to 3D
/////
///////////////////////////////// PERSPECTIVE MODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void PerspectiveMode()										// Set Up A Perspective View
{
	// This function doesn't really go to perspective mode, since we aren't
	// using gluPerspective(), but if you look in OrthoMode(), we push on
	// a matrix once we change to our projection matrix.  All we need to do is
	// pop that matrix off of our stack and it will return to our original perspective
	// matrix.  So, before we pop the current matrix, we need to tell OpenGL which
	// matrix we want to use by changing our current matrix mode to perspective.
	// Then we can go back to our model view matrix and render normally.

	// Enter into our projection matrix mode
	glMatrixMode( GL_PROJECTION );							

	// Pop off the last matrix pushed on when in projection mode (Get rid of ortho mode)
	glPopMatrix();											

	// Go back to our model view matrix like normal
	glMatrixMode( GL_MODELVIEW );							

	// We should be in the normal 3D perspective mode now
}


///////////////////////////////// DRAW ROOM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function renders the room walls and floor
/////
///////////////////////////////// DRAW ROOM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DrawRoom()
{
	// To get a better view of our scope transparency, I created some walls and
	// a floor to look at.  We use the wall texture and floor texture for the
	// quads that are rendered.  Notice that our camera is in the center
	// of the world at the origin, and the walls are 15 units away from the camera.
	// Since the quads are so huge, we wanted to wrap the seamless textures multiple
	// times instead of stretching the texture along the huge wall.  That is why the
	// U (x value) for the UV coordinates is greater than 1.  12 repetitions seemed
	// sufficient.  

	// Bind the wall texture to the wall quads
	glBindTexture(GL_TEXTURE_2D,  g_Texture[0]);

	// Display a 4 long quads to represent walls
	glBegin(GL_QUADS);

	////////////////////// BACK WALL ///////////////////////

		glTexCoord2f(0.0f, 2.0f);	glVertex3f(-15, 1, -15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-15, -1, -15);

		glTexCoord2f(12.0f, 0.0f);	glVertex3f(15, -1, -15);

		glTexCoord2f(12.0f, 2.0f);	glVertex3f(15, 1, -15);

	////////////////////// FRONT WALL ///////////////////////

		glTexCoord2f(0.0f, 2.0f);	glVertex3f(-15, 1, 15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-15, -1, 15);

		glTexCoord2f(12.0f, 0.0f);	glVertex3f(15, -1, 15);

		glTexCoord2f(12.0f, 2.0f);	glVertex3f(15, 1, 15);

	////////////////////// LEFT WALL ///////////////////////

		glTexCoord2f(0.0f, 2.0f);	glVertex3f(-15, 1, -15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-15, -1, -15);

		glTexCoord2f(12.0f, 0.0f);	glVertex3f(-15, -1, 15);

		glTexCoord2f(12.0f, 2.0f);	glVertex3f(-15, 1, 15);


	////////////////////// RIGHT WALL ///////////////////////

		glTexCoord2f(0.0f, 2.0f);	glVertex3f(15, 1, -15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(15, -1, -15);

		glTexCoord2f(12.0f, 0.0f);	glVertex3f(15, -1, 15);

		glTexCoord2f(12.0f, 2.0f);	glVertex3f(15, 1, 15);

	// Stop drawing the walls
	glEnd();

	// Now we need to draw the floor.  We wrapped the seamless texture
	// along the floor 16 times each way.  Notice the 16.0f in the texture coords.
		
	// Bind this texture to the floor quad
	glBindTexture(GL_TEXTURE_2D,  g_Texture[1]);

	// Display a huge quad to represent a floor
	glBegin(GL_QUADS);

		glTexCoord2f(0.0f, 16.0f);	glVertex3f(-15, -1, -15);

		glTexCoord2f(0.0f, 0.0f);	glVertex3f(-15, -1, 15);

		glTexCoord2f(16.0f, 0.0f);	glVertex3f(15, -1, 15);

		glTexCoord2f(16.0f, 16.0f);	glVertex3f(15, -1, -15);

	// Stop drawing the floor
	glEnd();
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
	
/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Instead of adding our camera class to this tutorial, I just did a quick hack to
	// demonstrate the zooming in and out of the scope.  Ideally, we would have a position
	// for the camera and a separate camera object for the scope.  We just need to zoom
	// in along the Z axis, because g_rotateY takes care of the rotations.

		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, g_zoom,     0, 0, g_zoom + 1,     0, 1, 0);	

	// Rotate the world around the y axis, according to the degree stored in g_rotateY
	glRotatef(g_rotateY , 0, 1, 0);
				
	// Here we draw the texture walls and floor
	DrawRoom();

	// Before drawing the scope texture, we need to switch to ortho (2D) mode.  We pass in
	// our screen width and height to use as the right and bottom coordinates of the view port.
	OrthoMode(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	// If g_bNightVision is true, we want to turn our world green for night vision,
	// otherwise do the normal color.  glColor4f() takes a RGB an alpha value,
	if(g_bNightVision)
		glColor4f(0, 1, 0, 1);
	else
		glColor4f(1, 1, 1, 1);

	
	///////////////////// MASKING ///////////////////////////////

	// Once the room is drawn, we need to start doing the masking.  In order for
	// this to work, we NEED TO TURN OFF DEPTH TESTING.  Otherwise it won't work.
	// Before we render the real image, we will render the mask image.
	// The blending method we will use is (GL_DST_COLOR, GL_ZERO).


	// Disable depth testing (MUST do this to make it work)
	glDisable(GL_DEPTH_TEST);

	// Select our desired depth testing and turn on blending
	glBlendFunc(GL_DST_COLOR,GL_ZERO);
	glEnable(GL_BLEND);											

	// Bind the mask texture to our new 2D quad
	glBindTexture(GL_TEXTURE_2D,  g_Texture[2]);

	// Display a 2D quad with the scope/cross hair mask
	glBegin(GL_QUADS);
	
		// Notice that when we are in ortho mode, we use glVertex2f() to pass
		// in screen coordinates, not vertices.  This makes it incredibly easy
		// to put up 2D interface art.  It's just like doing 2D graphics.
		// The texture coordinate stay the same regardless.

		// Display the top left point of the 2D image
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(0, 0);

		// Display the bottom left point of the 2D image
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(0, SCREEN_HEIGHT);

		// Display the bottom right point of the 2D image
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);

		// Display the top right point of the 2D image
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(SCREEN_WIDTH, 0);

	// Stop drawing 
	glEnd();	


	// Once the mask is draw to the screen, we want to set a new blend mode.
	// This mode will perform the masking effect that we desire.  Then we just
	// render the desired transparent image over top of the mask and it will
	// only draw the desired effect.

	// Turn our blending mode to one to one
	glBlendFunc(GL_ONE,GL_ONE);
	
	// Bind the scope texture to this next 2D quad
	glBindTexture(GL_TEXTURE_2D,  g_Texture[3]);

	// Display a 2D quad with our scope texture
	glBegin(GL_QUADS);

		// Display the top left point of the 2D image
		glTexCoord2f(0.0f, 1.0f);	glVertex2f(0, 0);

		// Display the bottom left point of the 2D image
		glTexCoord2f(0.0f, 0.0f);	glVertex2f(0, SCREEN_HEIGHT);

		// Display the bottom right point of the 2D image
		glTexCoord2f(1.0f, 0.0f);	glVertex2f(SCREEN_WIDTH, SCREEN_HEIGHT);

		// Display the top right point of the 2D image
		glTexCoord2f(1.0f, 1.0f);	glVertex2f(SCREEN_WIDTH, 0);

	// Stop drawing 
	glEnd();

	// Since we are done, we can now turn OFF blending and turn ON depth testing
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	// Let's set our mode back to perspective 3D mode.  None of this archaic 2D stuff :)
	PerspectiveMode();

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	// Swap the backbuffers to the foreground
	SwapBuffers(g_hDC);									
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

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

			// We can switch between night vision with the space bar.
			// Once again, this is here and not in KeyPressed() because it's key
			// response time is a lot more practical, not liquid fast like GetKeyState().

			case VK_SPACE:								// Check if we hit the space bar
				g_bNightVision = !g_bNightVision;		// Turn night vision on/off
				break;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

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
// Pretty cool effect huh?  Well, you might be puking over the incredible blurred
// scope image.  This is because we are taking a small texture and stretching it
// across the whole screen.  I chose to do a small texture because some video
// cards don't support larger textures than 512.by 512.  The scope looks fine, but
// the cross hair is blurry,  Most likely, the scope image would be blank without a
// cross hair, then we could make a more detailed scope and render it to the center
// of the screen with it's own mask.  If the scope image is currently to big for
// your video card, just go into any paint program and shrink it, then try it.
// 
// Let's review once more on some vocabulary:
// 
// Masking - This technique allows you to render the shape of an image, instead of
// seeing a rectangle tree for instance.  This is accomplished by creating a mask,
// like in 2D, to blit before the image, then render the transparent texture with a
// one to one blend over top of the mask.  The draw back of masking is that you have
// to render the image twice.  Compiled vertex arrays come in handy for speeding this
// up though.  I suggest maybe trying a 32-bit image with an alpha channel instead.
//
// Ortho Mode - Ortho mode is a projections mode that allows us to draw primitives
// in 2D.  We don't use glVertex3f(), but glVertex2f().  This takes screen coordinates
// instead of 3D unit coordinates.  This is very helpful when dealing with interfaces
// or anything else you want to stay on the screen, no matter what the camera is doing.
//
// So, how is it that we make the image stay in front of the screen no matter where the
// camera's view moves too?  This is quite simple really.  Notice in OrthoMode() that
// we push on a matrix right before we call glLoadIdentity().  We do this so that
// the current matrix we are working with is NOT effected by us reseting a new matrix
// to the identity matrix.  By reseting the matrix, there is NO rotation, scale or
// translations applied to anything rendered afterwards, until we call a glPopMatrix()
// to go back to our original matrix.  So basically, it's as if we said, "Time out,
// I need to start with a fresh matrix, but don't delete my last matrix because I want
// to go back to it in a second".  This has nothing to do with ortho mode, and you
// don't even have to push and pop a new matrix on the stack if you render with your
// reset matrix last.  We just push on a new matrix because we switch to ortho mode
// and want to go back to perspective mode without having to reinitialize it.
// 
// This tutorial should give you tons of idea when making your games.  This knowledge
// is vital to making most any 3D game. 
//
// Once again, this tutorial allows you to rotate and zoom in/out with your scope.
// There is also a night vision mode.  Here are the keys:
//
// LEFT / RIGHT Arrow Keys or 'a' / 'd' - This rotates the camera's view in a circle
// UP / DOWN Arrow Keys or 'w' / 's' - This makes our scope zoom in and out
// SPACE BAR - This turns night vision on and off
// ESCAPE Key - This quits the program
//
// I hope this helps someone out!
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
