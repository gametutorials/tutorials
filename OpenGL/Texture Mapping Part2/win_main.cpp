// Done by TheTutor

/*	This tutorial will show you how to load a texture map WITHOUT using a bitmap library.
	
	But first, lets review texture mapping in general:

	You have to "turn on texture mapping" by making the following call:
    glEnable(GL_TEXTURE_2D);

    Next you have to load your texture (in our case a 24 or 32-bit .bmp file) instead of 
    using the glaux library (or any other preexisting library) we will do it ourselves.

	Then you call glGenTextures() -- This generates a unique texture reference ID.
	We then call glBindTexture() next to say "Okay this is the texture we are 
	currently working with"
	
	Next up is the loading of the texture.  You do this will a call to:  glTexImage2D().  This
	is what assocaties the pixel bits of the texture with the OpenGL id we binded with 
	the previous call to glBindTexture().
	
	If we have mip-maps we want to create, we'll then call gluBuild2DMipmaps(), this goes through
	and makes every power of 2, counting down, sized mip-map for the texture we just loaded.  So if
	our texture was a 128x128, calling gluBuild2DMipmaps() would generate a 64x64, 32x32, 16x16, 8x8,
	4x4, 2x2, and 1x1 mipmap for that texture.

	Last but not least, we set the "quality" of our texture map with glTexParameteri()
	Depending on what flags we pass in, we can make the texture look really smooth or 
	have it render faster but be a bit more pixilated.

	So now we have our textured loaded, we need to AGAIN call glBindTexture() BEFORE drawing
	to say "use this texture when rendering" -- Then we make calls to glTexCoord2f(), this
	tells OpenGL what coordinates on the texture map to start rendering (drawing) from.

	When it's all said and done be sure to free up the memory used by your texture map with
	a call to glDeleteTextures()
*/

// Make sure we load all the libraries we need -- NOTICE we ARE NOT using the glaux.lib
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define class_name "GT_OGL_Textures"

#define WIN_WID 800 // width of our window
#define WIN_HGT 600 // height of our window

#define TEXTURE_NAME "texture.bmp" // Name of the file holding our texture

#include "gl_obj.h"
#include "bitmap_class.h"

// Globals ***

CGLObj gGLObj; // Our OpenGL object
GLuint gTextureID = 0; // This will hold our OpenGL texture ID

// *** End of Globals

bool LoadTexture(); // This loads the texture into OpenGL so we can use it
void FreeTexture(); // This frees the loaded texture

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	MSG msg = {0};
	HWND hwnd = NULL;
	WNDCLASSEX wndclassex = {0};

	// Fill the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc = WinProc;					
	wndclassex.hInstance = hinstance;	
	wndclassex.lpszClassName = class_name;
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW),
											IMAGE_CURSOR, 0, 0, LR_SHARED);

	RegisterClassEx(&wndclassex); // Register the class

	RECT rect = { 0, 0, WIN_WID, WIN_HGT }; // Desired window client rect
	
	DWORD winStyleEx = WS_EX_APPWINDOW;
	DWORD winStyle = WS_CAPTION | WS_SYSMENU; // Window style

	// Adjust window rect so it gives us our desired client rect when we 
	// create the window
	AdjustWindowRectEx(&rect, winStyle, false, winStyleEx);

	// Create our window -- Notice how we set use different "variables" when creating
	// the window based upon if we were able to switch to full screen or not
	hwnd = CreateWindowEx(winStyleEx,
						  class_name,
						  "www.GameTutorials.com -- Texture Mapping",
						  winStyle,
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  rect.right - rect.left, // Window width
						  rect.bottom - rect.top, // Window height
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error Check
		if(!hwnd)
			goto CLEAN_UP; // Here we are going to use goto in a "good" way
						   // We have a few things we want to initialize so if
						   // any of them fail, we just go to the "clean up code"
	
	// Get the client RECT
	GetClientRect(hwnd, &rect);
	
	// If the client width or height isn't what we requested, there's been
	// an error somewhere and we're going to bail ship
	if(rect.right != WIN_WID || rect.bottom != WIN_HGT)
		goto CLEAN_UP;
		
	HDC hdc = GetDC(hwnd); // Get the window's hdc

	// Initialize OpenGL
	if(gGLObj.init(hdc, 32, 32) == false)
		goto CLEAN_UP;

	// Load the texture
	if(LoadTexture() == false)
		goto CLEAN_UP;

	// Set up the view port
	gGLObj.setViewPort(WIN_WID,WIN_HGT,45.0);

	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gGLObj.begin(); // Begin to draw

				// Bind the texture
				glBindTexture(GL_TEXTURE_2D, gTextureID);

				// Draw a quad (ie a square)
				glBegin(GL_QUADS);

					// glTexCoord2f() takes a U and V (U,V) into our texture.  The U and V are 
					// in the range from 0 to 1.  And work like this:

					/*	  (0,1)  (1,1) 
						    _______
						   |	   |
						   |	   |
						   |	   |			Just like Cartesian coordinates :)
						    -------
						  (0,0)  (1,0)

					*/

					// Display the top left vertex
					glTexCoord2f(0.0f, 1.0f);
					glVertex3f(-1, 1, 0);

					// Display the bottom left vertex
					glTexCoord2f(0.0f, 0.0f);
					glVertex3f(-1, -1, 0);

					// Display the bottom right vertex
					glTexCoord2f(1.0f, 0.0f);
					glVertex3f(1, -1, 0);

					// Display the top right vertex
					glTexCoord2f(1.0f, 1.0f);
					glVertex3f(1, 1, 0);

				glEnd(); // Stop drawing QUADS

			gGLObj.end(); // End drawing in 3D and blit to the screen

		}
		
	} // end of while(1)

CLEAN_UP:

	if(hdc)
		ReleaseDC(hwnd, hdc); // Be sure to free up the window's HDC

	FreeTexture(); // Free the texture
	UnregisterClass(class_name,hinstance); // Free up the WNDCLASSEX
		return msg.wParam;
}

// Simple WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
   	switch(message)
    {
		case WM_KEYDOWN:

			// Here we are TRICKY!!!!  If the key press WAS NOT the escape key we'll simply
			// return 0 (ie effectively do nothing) but if the key press was the escape key
			// we will "fall through" to the WM_CLOSE/WM_DESTROY part of the switch statement
			// and PostQuitMessage(0); will get called
			if(wparam != VK_ESCAPE)
				return 0;
	
		case WM_CLOSE:
		case WM_DESTROY:
			
			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// This loads the texture so OpenGL can use it
bool LoadTexture()
{
	CBitmap image; // We'll use this to load the texture

	// First we load the bitmap
	if(image.loadBMP(TEXTURE_NAME) == false)
		return false;

	// Then we need to generate a "texture ID" for the texture
	// We want ONE texture ID to be stored in our global "gTextureID"
	glGenTextures(1, &gTextureID);

	// The we HAVE to call glBindTexture() so our next function call to gluBuild2DMipmaps()
	// knows what texture ID we're talking about
	glBindTexture(GL_TEXTURE_2D, gTextureID);
	
	// This is the function that really associates the pixels (the texture) with the
	// "gTextureID", it says how the texture is laid out (ie the width, height, how many
	// bits per pixel, etc, etc) -- So by parameter:
	
	// GL_TEXTURE_2D -- This has to be GL_TEXTURE_2D
	// 0 -- The requested mipmap level.  This is the base texture so we don't have
	//		have a mipmap level
	// image.get_channels() -- The number of channels (which is equivalent to saying how
	//						   many bits per pixel are in the image)
	// image.get_width() -- Width of the image (texture)
	// image.get_height() -- Height of the image (texture)
	// 0 -- Pixel border.  If you have a one pixel border around your image, this would be
	//		set to 1, otherwise it will be zero
	// GL_BGR_EXT -- This says what format the pixels are arranged, Window .bmps are arranged
	//				 so their pixels go in BGR (blue, green, red) order
	// GL_UNSIGNED_BYTE -- This say what type of "data type" makes up each component of 
	//					   a pixel -- Each component (ie the R, G, or B) of our pixels
	//					   is stored as an unsigned char (which equals one byte, or one
	//					   unsigned byte)
	// image.getLinePtr(0) -- This is a pointer to the "image pixel data" in memory
	//						  so what we pass in is the pointer to the FIRST scan line
	//						  of our texture
	glTexImage2D(GL_TEXTURE_2D, 0, image.getChannels(), image.getWidth(), image.getHeight(),
				 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, image.getLinePtr(0));

	// It's basically the same call to build the mipmaps.  So by parameter:
	
	// GL_TEXTURE_2D -- This has to be GL_TEXTURE_2D
	// image.get_channels() -- The number of channels (which is equivalent to saying how
	//						   many bits per pixel are in the image)
	// image.get_width() -- Width of the image (texture)
	// image.get_height() -- Height of the image (texture)
	// GL_BGR_EXT -- This says what format the pixels are arranged, Window .bmps are arranged
	//				 so their pixels go in BGR (blue, green, red) order
	// GL_UNSIGNED_BYTE -- This say what type of "data type" makes up each component of 
	//					   a pixel -- Each component (ie the R, G, or B) of our pixels
	//					   is stored as an unsigned char (which equals one byte, or one
	//					   unsigned byte)
	// image.getLinePtr(0) -- This is a pointer to the "image pixel data" in memory
	//						  so what we pass in is the pointer to the FIRST scan line
	//						  of our texture
	gluBuild2DMipmaps(GL_TEXTURE_2D, image.getChannels(), image.getWidth(), 
					  image.getHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE, 
					  image.getLinePtr(0));

	// **NOTE** to load a 32-bit .bmp you would have to change GL_BGR_EXT to GL_BGRA_EXT
	// This accounts for the alpha channel

	// Lastly, we set the quality of our texture --
	// GL_TEXTURE_MIN_FILTER is for anytime we draw our texture map to a MINIMIZED area
	// realtive to the size of our texture map
	// GL_TEXTURE_MAG_FILTER is for anytime we draw our texture map to a MAGNIFIED area
	// realtive to the size of our texture map
	// GL_LINEAR_MIPMAP_NEAREST and GL_LINEAR are flags that dictate how the 
	// mipmaps will be blended. 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
		return true;
}

// Free the texture
void FreeTexture()
{
	if(gTextureID) // If we loaded a texture
		glDeleteTextures(1,&gTextureID); // Free up the texture

	gTextureID = 0;
}

/*
	Now you should be a texture mapping guru :)

	So why would you ever not want to use the glaux library???  Well for starters if it 
	wasn't available to you, which happens to be the case in Visual Studio 2008 and above,
	and it's good to know how to still load a texture without depending on libraries.  Also by loading
	your own textures you can have a lot more flexibility.  For instance, lets say that we
	wanted to make our texture we loaded darker.  Since we loaded it we could just add a method
	to the CBitmap class that say "subtracted 2" from every RGB value (thus making the
	texture darker) then we could bind it to an OpenGL texture ID.

	Also we can load ANY file format we want (not all file formats are supported by glaux)
	As long as we can load a file into our CBitmap class, we can create a texture from it.
	
	If you have any questions or comments, send 'em our way on the GT message board found
	at www.gametutorials.com.
	
*/


/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
