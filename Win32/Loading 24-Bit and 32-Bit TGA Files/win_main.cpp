// Done by TheTutor

/*
	Okay this tutorial is going to load an uncompressed 24 or 32-bit .tga file "manually"
	We're going to load it into our class HC_BMP (high color bitmap) -- The reason we call it 
	"high color bitmap" is we're going to use a dib section (ie bitmap) to store the
	.tga file in memory.  The class will also contain a few other methods but it's basic 
	functionality is to load 24 or 32-bit .tga files :)

	A few lines down you see a #define for IMAGE_NAME -- If you change the "24" in IMAGE_NAME
	to "32" it will load the 32-bit image instead of the 24-bit image

	Now these are the exact same images, but you can be rest assured one is 24-bit and one
	is 32-bit (look at the file size if you don't believe me)
*/

// These will be the upper-left coordinates of where the image will be blit to
// Totally just picked some numbers out of the air :)
#define XPOS 175
#define YPOS 175

#include "bitmap_class.h"

#define CLASS_NAME "GameTutorials_tgaLoader"
#define IMAGE_NAME "GameTutorials_24.tga" // Name of file to load

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
	HDC win_hdc; // Our window's HDC
	CTarga image; // Used for holding the image

    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	
	// Init the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclassex.lpszClassName = CLASS_NAME;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED);

	RegisterClassEx(&wndclassex); // Register the extended window class

	// Create the window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  CLASS_NAME,
						  "www.GameTutorials.com -- Loading 24 and 32-bit .tga files",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  640,
						  480,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error Check
		if(!hwnd)
			return EXIT_FAILURE;
		
	// Get the window's HDC
	win_hdc = GetDC(hwnd);

		// Error Check
		if(!win_hdc)
			return EXIT_FAILURE;
		
	// Try and load the image
	if(image.loadTGA(IMAGE_NAME) == false)
	{
		MessageBox(hwnd,"Couldn't load image -- Who's the funny guy???","Error",MB_OK);
			return EXIT_FAILURE;
	}

	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

	// Blit the pretty picture
	// **NOTE** any more drawing to window will draw over the pretty picture :)
	BitBlt(win_hdc,XPOS,YPOS,image.getWidth(),image.getHeight(),
		   image.getHDC(),0,0,SRCCOPY);
	
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
			// Do heavy computational stuff here
		}

	} // end of while(1)

	UnregisterClass(CLASS_NAME,hinstance); // Free memory associated with WNDCLASSEX
		return msg.wParam;
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    PAINTSTRUCT ps;
		
	switch(message)
    {	
		case WM_PAINT:
			
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
				return 0;

		case WM_DESTROY:

			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

/*
	Okay for starters, how the heck do you know how a .tga file is laid out?  Well I went
	to http://www.wotsit.org/ and typed "TGA" in the search.  I used the documentation provided
	by David McDuffee.  With this, it was a piece of cake :)

	As always, head to www.GameTutorials.com if you have questions.
*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/

	
