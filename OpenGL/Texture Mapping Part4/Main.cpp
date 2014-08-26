//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		DigiBen		digiben@gametutorials.com			 //
//																		 //
//		$Program:		TexturingIV										 //
//																		 //
//		$Description:	Texture maps a QUAD with a TGA image			 //
//																		 //
//***********************************************************************//


// This is a compiler directive that includes libraries (For Visual Studio)
// You can manually include the libraries in the "Project->settings" menu under
// the "Link" tab.  You need these libraries to compile this program.
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include "main.h"										// This includes our header file

bool  g_bFullScreen = TRUE;								// Set full screen as default
HWND  g_hWnd;											// This is the handle for the window
RECT  g_rRect;											// This holds the window dimensions
HDC   g_hDC;											// General HDC - (handle to device context)
HGLRC g_hRC;											// General OpenGL_DC - Our Rendering Context for OpenGL
HINSTANCE g_hInstance;									// This holds the global hInstance for UnregisterClass() in DeInit()


UINT g_Texture[MAX_TEXTURES];							// This will reference to our texture data stored with OpenGL UINT is an unsigned int (only positive numbers)

/////////////////////////////////////////////////////////////////////////////////
//
// This tutorial works off of the original texture mapping tutorial that used
// the glaux library to load bitmaps.  We change very little code to enable
// us to create texture maps from targa (TGA) files.  Take a look down at 
// the *Quick Notes* section near the bottom of this page to get more information.  
// The targa loader supports 16, 24 and 32-bit images, along with RLE compression.
// The texture loading in this tutorial happens to be a 24-bit image.
// 
// It's important to keep in mind that the TGA code is not robust and still needs
// some work before added to an engine, but it should be a great start.  I would
// recommend that you add more error checking and any other TGA formats we are missing.
// It would also need to be moved into a nice class.  I didn't because of simplicity's sake.
//
// It should be understood that I don't intend to go into a huge explanation of the
// TGA file format, since we already have a great tutorial on this in our win32 section
// on our website.  If you are confused with how TGA files are constructed, read it.
//
//

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

	// This is where we load all of our textures.  We are loading
	// just one in this case, but you can add many more if you want.
	// We pass in our global textureArray, our file we want to load, 
	// and the texture ID we want associated with it.

	// Load "Image.tga" into OpenGL as a texture
	CreateTexture(g_Texture, "Image.tga", 0);			

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *

}

///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function Handles the main game loop
/////
///////////////////////////////// MAIN GAME LOOP \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

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
			RenderScene();								// Redraw the scene every frame
        } 
	}

	DeInit();											// Free all the app's memory allocated

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
	
		// 	  Position      View	   Up Vector
	gluLookAt(0, 0, 6,     0, 0, 0,     0, 1, 0);		// This determines where the camera's position and view is

	// Bind the texture stored at the zero index of g_Texture[]
	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);

	// Display a quad texture to the screen
	glBegin(GL_QUADS);

		// Display the top left vertice
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1, 1, 0);

		// Display the bottom left vertice
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1, -1, 0);

		// Display the bottom right vertice
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1, -1, 0);

		// Display the top right vertice
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1, 1, 0);

	// Stop drawing QUADS
	glEnd();											

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
// There you have it!  That is how to texture map with targa files.  It's pretty easy.
// All you really care about is that you need to use the function:
//
// tImageTGA *LoadTGA(const char *filename);
//
// This will return a pointer to the TGA information.  Be sure to free the pointer
// and the pixel "data" after you are done with it, just like the aux* bitmap function.
//
// Thought this targa loader isn't robust, you can still load 16, 24 and 32-bit images.
// There is even added support for RLE compressed images, which generally aren't used.
// Our image that we load is 24-bit.  
//
// Here is the tImageTGA structure that I created to store the important data:
//
// struct tImageTGA
// {
//	  int channels;			// The channels of the image
//	  int sizeX;			// The width in pixels for the image
//	  int sizeY;			// The height in pixels for the image
//	  unsigned char *data;	// The pixel data for the image
// };
// 
//
// As a review, here are the steps to texture mapping:
// 
// 1) Turn on texture mapping by saying:  
//
//	  glEnable(GL_TEXTURE_2D);
//
//    This is usually done in the beginning of the program.  BUT, you will
//    want to turn it back off before you draw primitives using color instead.
//    So, say:  glDisable(GL_TEXTURE_2D)  - Then draw your colored shapes, then
//    turn it back on by saying glEnable(GL_TEXTURE_2D);
// 
// 2) Load a texture.  Once you have the data of the texture you loaded, you need to
//    register it with OpenGL.  The first step to do this is:
//
//	  glGenTextures(1, &textureArray[textureID]);
//
//    This generates a unique texture reference to the desired index of our texture array.
//	  We then pass this data into glBindTexture() to set the current texture being mapped.
//
//	  glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);
//
//    We NEED to call glBindTexture() after we generate the texture using glGenTextures, 
//    otherwise it won't be assigned the data from the next function, gluBuild2DMipmaps();
//
//	  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, pImage->sizeX, pImage->sizeY, GL_RGB, GL_UNSIGNED_BYTE, pImage->data);
//
//    What is a mip map you ask?  Mip maps are a bunch of different scaled sizes of the texture.
//    The reason for this is because it makes the texture look nicer if we are farther away or
//    up close to the texture.  It picks the best size depending on the distance we are from it.
//    It is a good idea to make textures that are a power of 2, like 2, 4, 8, 16, 32, 64, 128, 256, etc...
//    This makes it so it can get a smoother representation when it scales, otherwise it could look worse when scaled.
//    After we build the mip maps, we then need to set the type of filter we want to use.
//
//	  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
//	  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
//
//    If we use GL_LINEAR it will look smoother, but takes a little speed hit.  If we use
//    GL_NEAREST it will be a bit faster, but will look pixilated and blotchy.  The MIN filter
//    is used whenever the pixel being textured maps to an area greater than one texture element. 
//    The MAX filter is used when the pixel being textured maps to an area less than or equal to one texture element
//
// 3) After we load the texture, we just need to call glBindTexture() to set that as the current
//    texture being mapped.
// 
// 4) Next, we need to give each vertex a texture coordinate, also called (U, V) coordinates.
//    We do that by calling:
//
//    glTexCoord2f(U, V);
//
//    This assigns a part of the image to the next vertice passed in.  It's like putting a
//    sticker on paper, you just match the correct corner with the correct paper corner.
//    You have to be careful when you do this or else the image will be flipped or upside down.
//    It depends on the direction of your camera though.  The order is VERY important.
// 
// 5) Lastly, you need to delete the textures after you are finished.  You do this by calling:
// 
//    glDeleteTextures(1, &textureArray[textureIndex]);
// 
// That's it!  Let us know at GameTutorials.com if this tutorial helps you.  We would love to
// see what you come up with.
// 
//
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// ©2000-2005 GameTutorials
// © 2000-2005 GameTutorials
//
