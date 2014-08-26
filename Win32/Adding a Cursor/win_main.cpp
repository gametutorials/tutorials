/*
	Done by TheTutor

	Inside this tutorial we cover adding a CURSOR to your app.  This tutorial will be most valuable if
	you are using .NET because that's what compiler it is being built with.  Here's a list of the 
	steps below for making the CURSOR and including the CURSOR.

	1)  Select "Project" from the tool bar
	2)  Then select "Add Resource..."
	3)  In the list of resources you should see "Cursor" with a small plus sign (expansion button)
		next to it.  Click the expansion button.
	4)  A list of at least three cursor types should appear:  IDC_NODROP, IDC_POINTER, 
		IDC_POINTER_COPY.  Select IDC_POINTER and click "New"
	5)  This will take you into the resource editor.  There you can use the tools to make
		any a cursor icon of your choosing.
	6)  Upon completing your cursor, click the "Save All" button and you should notice that
		your resource.h file (which is auto-generated) will be updated.

	Now we're ready to put the cursor in our program.	
*/

#include <windows.h>
#include "resource.h"

const char kClassName[] = "GT_Cursor"; // WNDCLASSEX name

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline,	int ishow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	
	// Fill the WNDCLASSEX fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // Make the window background white
    wndclassex.lpszClassName = kClassName;

	// The "hCursor" is the member of WNDCLASSEX we need to fill to have a 
	// cursor with our application

	// By parameter:
	// hinstance -- Handle to the instance to load the cursor from, we're passed this by WinMain()
	// MAKEINTRESOURCE(IDC_POINTER) -- Okay the second parameter is a handle to the image to 
	//								   be loaded.  If you noticed when we created the cursor,
	//								   a header file was also created for us "resource.h". It was
	//								   automatically added to the project.  If you look at it, 
	//								   you'll notice it has a #define for IDC_POINTER.
	//								   MAKEINTRESOURCE is a macro that takes an integer value
	//								   to a resource type compatible with Win32 
	//								   resource-management functions.  It converts this integer
	//								   into a corresponding "handle name" for the resource
	// IMAGE_CURSOR -- Specifies the type of image to be loaded
	// 0 -- Width in pixels of image to be loaded
	// 0 -- Height in pixels of image to be loaded
	// LR_DEFAULTSIZE -- Uses the system metric values for cursors (also icons) when loading
	//					 a cursor if the width and height (the two parameters right before this)
	//					 are set to zero
	wndclassex.hCursor = (HCURSOR)LoadImage(hinstance,MAKEINTRESOURCE(IDC_POINTER),IMAGE_CURSOR,
		                                    0,0,LR_DEFAULTSIZE);

    RegisterClassEx(&wndclassex);

    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, // Window should have a sunken edge
						  kClassName,
						  "www.GameTutorials.com -- Adding a Cursor",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, // Default upper x-coord of window
						  CW_USEDEFAULT, // Default upper y-coord of window
						  CW_USEDEFAULT, // Default width of window
						  CW_USEDEFAULT, // Default height of window
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

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
			// Do stuff :)
			
		} // end of if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))...else

	} // end of while(1)

	UnregisterClass(kClassName,hinstance); // Free up WNDCLASSEX
		return int(msg.wParam);
}

// Don't do much in the WinProc
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


/* Cursed Cursors...

  Well you may notice that our cursor is a monochrome cursor.  Which could certainly suffice, but
  what if you want to make something more fancy?  Well you have to first make your cursor using
  software that makes a colored .cur or .ani file.  The .cur represents a normal cursor while 
  .ani represents an animated cursor.  Once you have this cursor made, you can load it with
  a call to:

  LoadCursorFromFile() -- This takes the name of the cursor you want to load

  And that's about all she wrote for cursors.  Any questions, comments, or criticisms 
  can be posted at www.GameTutorials.com

*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2005 GameTutorials |
\*-------------------------*/


