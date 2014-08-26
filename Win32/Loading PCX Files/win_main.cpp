// Done by TheTutor

/*
	Loading files is always a tad tricky.  One of the hardest parts is getting a layout of
	the file you are trying to load.  This tutorials shows how to load two types of .pcx files.
	First it shows how to load 24-bit .pcx files.  Second it shows how to load 8-bit .pcx files.
	We will load these files into a Win32 style bitmap and then draw them to the screen.  Most of
	the hard core implementation resides in "bitmap_class.cpp".
	
	Let us wait no longer... 
*/

// In Visual Studio 2005 Microsoft added safer versions (denoted by a "_s" suffix) to many 
// common C/C++ run time functions.  Currently the ISO Standards Committee for C and C++ 
// is determining if they are going to add them to the official language set.  Until then, 
// this define will remove warnings about using the "unsafe" versions of these functions.
#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include "bitmap_class.h"

////////////////
// Constants //
//////////////

int kWinWid = 800;
int kWinHgt = 800;

const char *fileName = "Image8Bit.pcx";
//const char *fileName = "Image24Bit.pcx"; // To see the 24-bit .pcx file loaded
										  // comment out the above "fileName" and comment
										 // this one in

//////////////
// Globals //
////////////

HDC gHDC = NULL; // Window's HDC
CBitmap gPCXImage; // Used for loading/displaying the .pcx file

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	
	char class_name[] = "GameTutorials_PCXLoading";
	
	// Init the fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
    wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclassex.lpszClassName = class_name;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED);

	RegisterClassEx(&wndclassex);

	// Create the window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  class_name,
						  "www.GameTutorials.com -- PCX File Loader",
						  WS_SYSMENU|WS_OVERLAPPED|WS_CAPTION,
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  kWinWid,
						  kWinHgt,
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error Check
		if(!hwnd)
			return EXIT_FAILURE;
				
	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);
    
    gHDC = GetDC(hwnd);
    
		// Error Check
		if(!gHDC)
			return EXIT_FAILURE;
	
	// Attempt to load the .pcx file
	if(!gPCXImage.loadPCX(fileName))
		return EXIT_FAILURE;
	
	// Calculate the upper left corner to approximately center the image in the window
	int xPos = (kWinWid / 2) - (gPCXImage.getWidth() / 2);
	int yPos = (kWinHgt / 2) - (gPCXImage.getHeight() / 2);
	
	// Draw it to the screen
	BitBlt(gHDC,xPos,yPos,gPCXImage.getWidth(),gPCXImage.getHeight(),
		   gPCXImage.getHDC(),0,0,SRCCOPY);
	
	// **NOTE** We only draw each image once so if the window is moved so the images go off 
	//			screen, they will be erased
	
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
			// This is where the core of your application would go
		}

	} // end of while(1)

	ReleaseDC(hwnd, gHDC); // Free up memory
	UnregisterClass(class_name,hinstance); // Free mem associated with WNDCLASSEX
		return msg.wParam;
}

// The WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
  	switch(message)
    {
		case WM_DESTROY:

			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return DefWindowProc(hwnd, message, wparam, lparam);
}

// PCX Express
/*
	The PCX file format is slightly old school, but could be useful if you have
	images that you want all the color components (ie the 'R', 'G' and 'B') clumped together
	instead of interwoven such as they are in a BMP file.
	
	If you have any programming questions, be sure to post 'em up on the forums and www.GameTutorials.com
*/

/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
