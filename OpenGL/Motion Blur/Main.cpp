//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		MotionBlur										 //
//																		 //
//		$Description:	Demonstrates fast motion blurring in OpenGL		 //
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

// This tutorial is taken from the Masking and Ortho tutorial, with a bunch also
// taken out from the Rendering To A Texture tutorial.  In the masking tutorial
// we learned how to mask images for distinct transparencies, along with the
// glOrtho() mode.  The glOrtho() mode allows us to pass vertices in as 2D
// coordinates.  This is great for things that we want to have in front of the
// camera at all times, such as a scope or interfaces.  In this tutorial we gutted
// out the masking stuff, but left the ortho code and added some functions for rendering
// to a texture which were taken from the RenderToTexture tutorial.  Before looking at
// this tutorial, it is assumed you have gone through the referenced tutorials, or are
// prepared to suffer an overload of new information :)
//
// The technique shown in this application is how to render motion blurring in an 
// incredibly fast way.  Usually you hear that you want to use the accumulation buffer 
// for motion blurring, but this is ridiculously slow for real time rendering.  A more 
// practical way to do it is to render the current screen to a texture, then go into
// ortho mode and blend that texture over the entire screen.  This could be done every
// frame or intermittently.  I used our AnimateNextFrame() function from our 2D sprite
// tutorial to restrict the blending to a certain frame rate.  This is stored in g_BlurRate.
// We can change the current rate by using the LEFT and RIGHT arrow keys.  The faster the
// rate the more realistic it looks, where as the slower it is, it appears more like a
// slow motion video game blur.  
// 
// To create the blur effect, we actually render the texture when rendering to the texture.
// This is a bit confusing.  Basically, when we go to render the screen to the texture,
// we go into ortho mode and blend the current texture of the screen over the object that
// is being blurred.  This acts as a recursive blending.  This makes it so we don't do
// tons of passes to create the blur trail.  
//
// The fading happens when we blend the current rendered texture with the blackness of 
// the background.  When blending, we need to blend the texture with a small decrease
// in alpha.  Basically, we don't render the texture over the screen at full alpha, 
// otherwise it would never blend away, or at least it would take forever to fade.
//
// Let's go over the controls for this app:
//
// LEFT / RIGHT Arrow Keys - Increase/Decrease the amount of passes per second
// '1' - This changes the texture size to 128 (LOW QUALITY)
// '2' - This changes the texture size to 256 (ACCEPTABLE QUALITY)
// '3' - This changes the texture size to 512 (HIGH QUALITY)
// SPACE BAR - Stops and starts the rotating box
// ESCAPE Key - This quits the program
// 
//

// This holds how many times per second we render to the texture
int g_BlurRate = 50;

// This stores if rotating is on or off
bool g_bRotate = true;

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *


// This stores our current rotation
float g_RotateX = 0, g_RotateY = 0, g_RotateZ = 0;

// This will store the size of the viewport that we render the texture from.
int g_Viewport = 512;

// This is our prototype for the function that creates the empty texture to render to
void CreateRenderTexture(UINT textureArray[], int size, int channels, int type, int textureID);


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

	glEnable(GL_DEPTH_TEST);							// Turn depth testing on
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping

	// This is where we initialize our empty texture to be rendered too.
	// We pass in the texture array to store it, the texture size,
	// the channels (3 for R G B), the type of texture (RGB) and texture ID.
	CreateRenderTexture(g_Texture, 512, 3, GL_RGB, 0);

	// Create the texture map for the spinning cube and assign it to texture ID 1
	CreateTexture(g_Texture[1], "Brick.bmp");
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
			RenderScene();								// Render the scene every frame
		} 
	}

	DeInit();											// Clean up and free all allocated memory

	return(msg.wParam);									// Return from the program
}


///////////////////////////////// ORTHO MODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function changes our projection mode from 3D to 2D
/////
///////////////////////////////// ORTHO MODE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void OrthoMode(int left, int top, int right, int bottom)
{
	// Switch to our projection matrix so that we can change it to ortho mode, not perspective.
	glMatrixMode(GL_PROJECTION);						

	// Push on a new matrix so that we can just pop it off to go back to perspective mode
	glPushMatrix();									
	
	// Reset the current matrix to our identify matrix
	glLoadIdentity();								

	//Pass in our 2D ortho screen coordinates like so (left, right, bottom, top).  The last
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
	// Enter into our projection matrix mode
	glMatrixMode( GL_PROJECTION );							

	// Pop off the last matrix pushed on when in projection mode (Get rid of ortho mode)
	glPopMatrix();											

	// Go back to our model view matrix like normal
	glMatrixMode( GL_MODELVIEW );							

	// We should be in the normal 3D perspective mode now
}


///////////////////////////////////// CREATE BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a textured box (if textureID >= 0) with a center and dimension
/////
///////////////////////////////////// CREATE BOX \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void CreateBox(int textureID, float x, float y, float z, float width, 
			   float height,  float length, float uscale, float vscale)
{
	// Check if we have a valid texture ID, then bind that texture to our box
	if(textureID >= 0) glBindTexture(GL_TEXTURE_2D, g_Texture[textureID]);

	// This centers the box around (x, y, z)
	x = x - width  / 2;
	y = y - height / 2;
	z = z - length / 2;

	// Start drawing the side as a QUAD
	glBegin(GL_QUADS);		
		
		// Assign the texture coordinates and vertices for the BACK Side
		glTexCoord2f(0.0f,     0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(0.0f,   vscale); glVertex3f(x,			y + height, z);
		glTexCoord2f(uscale, vscale); glVertex3f(x + width, y + height, z); 
		glTexCoord2f(uscale,   0.0f); glVertex3f(x + width, y,			z);
	
		// Assign the texture coordinates and vertices for the FRONT Side
		glTexCoord2f(uscale,   0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(uscale, vscale); glVertex3f(x,			y + height, z + length);
		glTexCoord2f(0.0f,   vscale); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f,     0.0f); glVertex3f(x + width, y,			z + length);
	
		// Assign the texture coordinates and vertices for the BOTTOM Side
		glTexCoord2f(uscale,   0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(uscale, vscale); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f,   vscale); glVertex3f(x + width, y,			z + length); 
		glTexCoord2f(0.0f,     0.0f); glVertex3f(x + width, y,			z);

		// Assign the texture coordinates and vertices for the TOP Side
		glTexCoord2f(uscale, vscale); glVertex3f(x,			y + height,	z);
		glTexCoord2f(uscale,   0.0f); glVertex3f(x,			y + height,	z + length);
		glTexCoord2f(0.0f,     0.0f); glVertex3f(x + width, y + height, z + length); 
		glTexCoord2f(0.0f,   vscale); glVertex3f(x + width, y + height, z);
	
		// Assign the texture coordinates and vertices for the LEFT Side
		glTexCoord2f(uscale,   0.0f); glVertex3f(x,			y,			z);
		glTexCoord2f(0.0f,     0.0f); glVertex3f(x,			y,			z + length);
		glTexCoord2f(0.0f,   vscale); glVertex3f(x,			y + height,	z + length); 
		glTexCoord2f(uscale, vscale); glVertex3f(x,			y + height,	z);

		// Assign the texture coordinates and vertices for the RIGHT Side
		glTexCoord2f(0.0f,     0.0f); glVertex3f(x + width, y,			z);
		glTexCoord2f(uscale,   0.0f); glVertex3f(x + width, y,			z + length);
		glTexCoord2f(uscale, vscale); glVertex3f(x + width, y + height,	z + length); 
		glTexCoord2f(0.0f,   vscale); glVertex3f(x + width, y + height,	z);

	// Stop drawing quads
	glEnd();
}


///////////////////////////////// CREATE RENDER TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a blank texture to render too
/////
///////////////////////////////// CREATE RENDER TEXTURE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	
void CreateRenderTexture(UINT textureArray[], int size, int channels, int type, int textureID)										
{
	// Create a pointer to store the blank image data
	unsigned int *pTexture = NULL;											

	// Allocate and init memory for the image array and point to it from pTexture
	pTexture = new unsigned int [size * size * channels];
	memset(pTexture, 0, size * size * channels * sizeof(unsigned int));	

	// Register the texture with OpenGL and bind it to the texture ID
	glGenTextures(1, &textureArray[textureID]);								
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);					
	
	// Create the texture and store it on the video card
	glTexImage2D(GL_TEXTURE_2D, 0, channels, size, size, 0, type, GL_UNSIGNED_INT, pTexture);						
	
	// Set the texture quality
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// Since we stored the texture space with OpenGL, we can delete the image data
	delete [] pTexture;																					
}



/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

///////////////////////////////// RENDER MOTION BLUR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function takes a texture ID to blend over the screen for motion blur
/////
///////////////////////////////// RENDER MOTION BLUR \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
	
void RenderMotionBlur(int textureID)
{
	// This function was created to blend the rendered texture over top of the screen
	// to create the recursive blur effect.  What happens is we start out by turning
	// off depth testing, setting our blending mode, then binding the texture of our 
	// rendered textured over the QUAD that is about to be created.  Next, we set our 
	// alpha level to %90 of full strength.  This makes it so it will slowly disappear.
	// Before rendering the QUAD, we want to go into ortho mode.  This makes it easier
	// to render a QUAD over the full screen without having to deal with vertices, but
	// 2D coordinates.  Once we render the QUAD, we want to go back to perspective mode.
	// We can then turn depth testing back on and turn off texturing.

	// Push on a new stack so that we do not interfere with the current matrix
	glPushMatrix();

		// Turn off depth testing so that we can blend over the screen
		glDisable(GL_DEPTH_TEST);			

		// Set our blend method and enable blending
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);	
		glEnable(GL_BLEND);					

		// Bind the rendered texture to our QUAD
		glBindTexture(GL_TEXTURE_2D, g_Texture[textureID]);			

		// Decrease the alpha value of the blend by %10 so it will fade nicely
		glColor4f(1, 1, 1, 0.9f);

		// Switch to 2D mode (Ortho mode)
		OrthoMode(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		// Display a 2D quad with our blended texture
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
		
		// Let's set our mode back to perspective 3D mode.
		PerspectiveMode();

		// Turn depth testing back on and texturing off.  If you do NOT do these 2 lines of 
		// code it produces a cool flash effect that might come in handy somewhere down the road.
		glEnable(GL_DEPTH_TEST);						
		glDisable(GL_BLEND);							

	// Go back to our original matrix
	glPopMatrix();
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

	// 	       Position        View	     Up Vector
	gluLookAt(0, 0, 10,     0, 0, 0,     0, 1, 0);		// Move the camera back by 10 units

	// We want to have the cube rotating along all axis's so we can better see the motion
	// blur at different rotations.  You can press the space bar to stop/start the rotation.

	glRotatef(g_RotateX, 1.0f, 0.0f, 0.0f);				// Rotate the cubes around the X-axis
	glRotatef(g_RotateY, 0.0f, 1.0f, 0.0f);				// Rotate the cubes around the Y-axis	
	glRotatef(g_RotateZ, 0.0f, 0.0f, 1.0f);				// Rotate the cubes around the Z-axis

	// Below we render to the texture.  We can control how much we render to the texture
	// per second by the LEFT and RIGHT arrow keys.  The more we render to the texture, the
	// smoother the blur is.  

	// Render to the texture only a controlled amount of frames per second
	if( AnimateNextFrame(g_BlurRate) )
	{
		// Shrink the viewport by the current viewport size.  Hit 1, 2, or 3 to change the size.
		glViewport(0, 0, g_Viewport, g_Viewport);								
								
		// This is where we redraw the current rendered texture before we draw the spinning box.
		// This creates the recursive fading of the motion blur.  We pass in the texture ID
		// we are using for the rendered texture.
		RenderMotionBlur(0);
		
		// Now we get to render the spinning box to the texture.  We just
		// need to create the box and it will be effected by the current rotation.
		// The parameters are the texture ID, the center (x,y,z), dimensions and uv scale.
		CreateBox(1,	0, 0, 0,	1, 6, 1,	1, 3);

		// Before we copy the screen to a texture, we need to specify the current
		// texture to draw to by calling glBindTexture() with the appropriate texture 
		glBindTexture(GL_TEXTURE_2D,g_Texture[0]);				

		// Render the current screen to our texture
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, g_Viewport, g_Viewport, 0);

		// Here we clear the screen and depth bits of the small viewport
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			

		// Set our viewport back to it's normal size
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);	
	}

	// Now that we have updated our rendered texture, let us display it to the screen
	// by blending it once again.  This should have all the blur trails rendered to it.
	RenderMotionBlur(0);

	// Redraw the box at the same position as it was drawn into the rendered texture
	CreateBox(1,	0, 0, 0,	1, 6, 1,	1, 3);

	// If we want rotation, increase the current rotations along each axises
	if(g_bRotate)
	{
		g_RotateX += 2; g_RotateY += 3; g_RotateZ += 2;
	}

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
	bool bUpdate = false;

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

			// We are able to control the amount of motion blur that happens
			// by using the left and right arrow keys.  We can also start and stop
			// the rotation of the box by using the space bar.  Like in the 
			// tutorial about rendering to a texture, we can also use 1, 2, of 3
			// to specify texture quality.  128 is atrocious though.

			case VK_LEFT:								// If we hit the LEFT arrow key

				g_BlurRate -= 10;						// Decrease the current blurs per second

				if(g_BlurRate < 10) g_BlurRate = 10;	// Make sure we don't go below 10

				bUpdate = true;							// Update the title information
				break;
	
			case VK_RIGHT:								// If we hit the RIGHT arrow key

				g_BlurRate += 10;						// Increase the current blurs per second

				if(g_BlurRate > 100) g_BlurRate = 100;	// Make sure we don't go above 100

				bUpdate = true;							// Update the title information
				break;

			case VK_SPACE:	g_bRotate = !g_bRotate;		// Turn on/off rotation
				break;

			case '1':	g_Viewport = 128;				// Change viewport size to 128
				bUpdate = true;							// Update the title information
				break;

			case '2':	g_Viewport = 256;				// Change viewport size to 256
				bUpdate = true;							// Update the title information
				break;

			case '3':	g_Viewport = 512;				// Change viewport size to 512
				bUpdate = true;							// Update the title information
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


/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

	if(bUpdate)
	{
		static char strTitle[255] = {0};
		sprintf(strTitle, "Current Texture Renders Per Second: %d   Current Texture Size: %d",
						   g_BlurRate,								g_Viewport);

		SetWindowText(g_hWnd, strTitle);		// Update the title information
	}

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

    return lRet;										// Return by default
}

/////////////////////////////////////////////////////////////////////////////////
//
// * QUICK NOTES * 
//
// Motion looks neat doesn't it?  This is true, but be careful to not use it
// all the time.  Though this technique is fast, everything that you blur has
// to be drawn twice.  That means you wouldn't want to blur everything in your
// entire frustum unless you are at like a cut scene or something, and the slow
// down is part of the drama and effect.  Obviously, the better your video card,
// the more detailed your blurred textures can be.  If you go to a 128 x 128 texture
// you will notice that it looks like crap.  It only gets worse the higher your
// screen resolution goes.  I recommend using at LEAST 256.  You could possibly
// get by with using 128 if the motion was really quick and far away.  I have 
// found that 512 seems to be the best.  It isn't too massive, so you don't get
// a huge memory footprint, and yet it isn't too small where you lose all the
// detail when the image is scaled at a higher resolution.  
// 
// I hope this helps someone out!
// 
//
// © 2000-2006 GameTutorials
//
