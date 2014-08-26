// Done by TheTutor

/*
	Perhaps you will recall (and if not here's a little refresher) that OpenGL has
	three different matrices we can play with:  The projection matrix, the model view matrix
	and the texture matrix.  The projection matrix more or less dictates how the 3D scence is 
	going to be	projected to our 2D window.  The model view matrix dictates the view of 3D scene, it 
	more or less gives moves our objects into world space and then gives us their view from 
	where we've set the camera.
	
	So what does the texture matrix do?  Well that's what this tutoiral is all about.  Using the
	texture matrix we are going to create so "shader-esque" effects.  Now days, alot of texture
	effects are done with pixel shaders.  These shaders are typically written in a shader language
	and are then executed by the graphics card.  However, texture manipulation can also be acheived
	by using the texture matrix.  For some effects, it may prove to be more benefical to use
	the texture matrix than writting a pixel shader routine for it.
	
	What we are going to do is create an app that lets us:  scale, rotate, and translate our
	texture's UVs.  The bulk of our texture maniputlation code lies in gl_texture CPP and H file.
	
	The controls for this application are as follows:
	
		UP ARROW -- Increases UVs scale
		DOWN ARROW -- Decreases UVs scale
		LEFT ARROW -- Translates the UVs left (left being relative to how we view the texture)
		RIGHT ARROW -- Translates the UVS right(right being relative to how we view the texture)
		**NOTE** By default the texture UVs are always rotating in a clockwise direction
		
	Enough of the beginning chatter, it's time to write code!	
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

// Make sure we load all the libraries we need
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define class_name "GT_OGL_TextureMatrix"

#define WIN_WID 800 // width of our window
#define WIN_HGT 600 // height of our window

#define TEXTURE_NAME "image.bmp" // Name of the file holding our texture

#include "gl_obj.h"
#include "gl_texture.h"

// Globals ***

CGLObj gGLObj; // Our OpenGL object
CGLTexture gTex; // Our OpenGL texture object

// *** End of Globals

bool LockFrameRate(int fps = 60); // Locks the frame rate to "fps"

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

	// Create our window
	hwnd = CreateWindowEx(winStyleEx,
						  class_name,
						  "www.GameTutorials.com -- Texture Matrix",
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
		{
			UnregisterClass(class_name,hinstance); // Free up the WNDCLASSEX
				return EXIT_FAILURE; // Something bad happened
		}
	
	// Get the client RECT
	GetClientRect(hwnd, &rect);
	
	// If the client width or height isn't what we requested, there's been
	// an error somewhere and we're going to bail ship
	if(rect.right != WIN_WID || rect.bottom != WIN_HGT)
	{
		UnregisterClass(class_name,hinstance); // Free up the WNDCLASSEX
			return EXIT_FAILURE; // Something bad happened
	}
		
	HDC hdc = GetDC(hwnd); // Get the window's hdc

	// Initialize OpenGL
	if(gGLObj.init(hdc, 32, 16) == false)
	{
		ReleaseDC(hwnd, hdc); // Free up the HDC
		UnregisterClass(class_name,hinstance); // Free up the WNDCLASSEX
			return EXIT_FAILURE; // Something bad happened
	}
	
	// Load the texture
	if(gTex.load(TEXTURE_NAME) == false)
	{
		ReleaseDC(hwnd, hdc); // Free up the HDC
		UnregisterClass(class_name,hinstance); // Free up the WNDCLASSEX
			return EXIT_FAILURE; // Something bad happened
	}		

	// Set up the view port
	gGLObj.setViewPort(WIN_WID,WIN_HGT,60.0);

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
		else if(LockFrameRate()) // If it's time to draw
		{
			static float angle = 0.0f;
		
			gGLObj.begin(); // Begin to draw

				// Bind the texture
				gTex.select();
				gTex.setRotation(angle); // Set the angle of rotation for the texture's UVs
				angle += TEX_ROTATION_AMT; // Increment the rotation angle

				// Draw a quad
				glBegin(GL_QUADS);

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

	ReleaseDC(hwnd, hdc); // Be sure to free up the window's HDC
	UnregisterClass(class_name,hinstance); // Free up the WNDCLASSEX
		return (int)msg.wParam;
}

// Simple WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// This stores our current texture UV scale amount
	static float scaleAmt = 1.0f;

   	switch(message)
    {
		case WM_KEYDOWN:

			switch(wparam)
			{
				case VK_LEFT: // If they push left, scroll the UVs left
					gTex.scroll(CGLTexture::eLeft);
						break;
						
				case VK_RIGHT: // If they push right, scroll the UVs right
					gTex.scroll(CGLTexture::eRight);
						break;
						
				case VK_UP: // If they push up, increase the scale amount
					scaleAmt += TEX_SCROLL_AMT;
					gTex.setScale(scaleAmt);
						break;
						
				case VK_DOWN: // If they push down, decrease the scale amount
					scaleAmt -= TEX_SCROLL_AMT;
					gTex.setScale(scaleAmt);
						break;
						
				case VK_ESCAPE: // On ESC quit the application
					PostQuitMessage(0);
						break;						
			}
			
		return 0;
	
		case WM_DESTROY:
			
			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// Locks the frame rate at "frame_rate"
// Returns true when it's okay to draw, false otherwise
bool LockFrameRate(int frame_rate)
{
	static float lastTime = 0.0f;
	
	// Get current time in seconds (milliseconds * .001 = seconds)
	float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	// If the desired frame rate amount of seconds has passed -- return true (ie Blit())
	if((currentTime - lastTime) > (1.0f / frame_rate))
	{
		// Reset the last time
		lastTime = currentTime;	
			return true;
	}

	return false;
}

/*
	Hopefully you've learned a little bit more about the OGL texture matrix.  If texture
	manipulation is your thing, the texture matrix allows you some pretty power flexibility
	and creativity to do some cool texture tricks.  Without too much trouble, you should
	now be able to create your own shader language.
	
	Questions, comments, sneers and cheers can be posted on the forums at:  
	www.GameTutorials.com	
*/


/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
