// Done by TheTutor

/*
	Loading files is always a tad tricky.  One of the harder parts is getting a description
	of how the file is laid out.  If you don't know how the file is laid out, it's going to
	be pretty hard to load.  Luckily MSDN does a pretty good job of explaining how an 8-bit
	.bmp is laid out.

	8-bit bitmaps can support up to 256 colors.  8-bit bitmaps pixel data, are actually indexes
	into an array of colors (RGBQUADs) associated with the bitmap.  8-bit bitmaps also 
	have the ability to be compressed.  This tutorial will teach you how to load BOTH 
	compressed and uncompressed 8-bit bitmaps.
	
	Now that we know what we want to do, lets write the code to do it.  
*/

#include "bitmap_class.h"

////////////////
// Constants //
//////////////

int kWinWid = 640;
int kWinHgt = 480;

const char *fileName = "Image8Bit.bmp";
//const char *fileName = "CompressedImage8Bit.bmp"; // To see the compressed .bmp file loaded
												   // comment out the above "fileName" and comment
												  // this one in
												
//////////////
// Globals //
////////////

HDC gHDC = NULL; // Our window's HDC
CBitmap gBitmap; // Used for holding the 8-bit .bmp

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	
	char class_name[] = "GT_8BitBMPRLELoader";
	
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
						  "www.GameTutorials.com -- Loading 8-bit bitmaps",
						  WS_OVERLAPPEDWINDOW,
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
		{
			UnregisterClass(class_name,hinstance);
				return EXIT_FAILURE;
		}

	gHDC = GetDC(hwnd);

		// Error Check
		if(!gHDC)
		{
			UnregisterClass(class_name,hinstance);
				return EXIT_FAILURE;
		}

	// Load the image
	if(gBitmap.loadBMP(fileName,gHDC) == false)
	{
		MessageBox(hwnd,"Couldn't load bitmap -- Who's the funny guy???","Error",MB_OK);
		UnregisterClass(class_name,hinstance); // Free mem associated with WNDCLASSEX
			return EXIT_FAILURE;
		
	}
	
	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);
    
	// Calculate the upper left corner to approximately center the image in the window
	int xPos = (kWinWid / 2) - (gBitmap.getWidth() / 2);
	int yPos = (kWinHgt / 2) - (gBitmap.getHeight() / 2);

	// Draw it to the screen
	BitBlt(gHDC,xPos,yPos,gBitmap.getWidth(),gBitmap.getHeight(),
		   gBitmap.getHDC(),0,0,SRCCOPY);

	// **NOTE** We only draw the image once so if the window is moved so the image goes off 
	//			screen, it will be erased
	
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

	ReleaseDC(hwnd,gHDC); // Release our HDC
	UnregisterClass(class_name,hinstance); // Free mem associated with WNDCLASSEX

		return (int)msg.wParam;
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

/* Eight Bit Beauty
	
	 When is a good time to use 8-bit images?  Well, if you want to really space space
	 (say your making an app on a cell phone) then 8-bit images can be a huge help.  Run length
	 encoding can make your image even smaller, but only if it has a lot of repeating colors.
	 
	 Questions?  Inquires?  Coding Problems?  Post 'em on the forums at:
	 www.GameTutorials.com
*/

/*-------------------------*\
|  Done by:  TheTutor		|
|  ©2005 GameTutorials, LLC |
\*-------------------------*/
