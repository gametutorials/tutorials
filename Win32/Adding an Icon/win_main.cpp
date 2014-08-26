/*
	Done by TheTutor

	This tutorial shows how to add an ICON to your app.  This tutorial will be most valuable 
	if you are using VC++ .NET because that's what compiler it is being built with.  Below 
	is a list of the steps for making the ICON and including the ICON:

	1)  Select "Project" from the tool bar
	2)  Then select "Add Resource..."
	3)  Select "Icon" then click the "New" button
	4)  Using the .NET resource editor tools, we create our snazzy icon :)
	5)  Click the "Save All" button when we're finished and the project should
		be automatically updated with a resouce.h file and saved .ico file

	Now we're ready to add the icon to our program
*/

#include <windows.h>
#include "resource.h"

////////////////
// Constants //
//////////////

const char kClassName[] = "GT_Icon";

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

// WinMain
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline,	int ishow)
{
    HWND hwnd = NULL;
    MSG msg = {0};
    WNDCLASSEX wndclassex = {0};
	
	// Fill the WNDCLASSEX fields we care about
	wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // Make the window background black
    wndclassex.lpszClassName = kClassName;
    wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR, 
											0, 0, LR_SHARED); // Load default arrow as cursor

	// In general there are two types of icons you can associate with an application.  A large
	// icon (32x32 pixels) and a small icon (16x16) pixels.  The large icon is what the icon that
	// gets attached to your executable.  It is also the icon that represents the executable when
	// ALT+TAB between applications.  The small icon is the icon that is shown in the upper-left
	// hand corner of the application.
	
	// The "hIcon" is the member of WNDCLASSEX we need to fill to have an 32x32
	// icon with our application

	// By parameter:
	// hinstance -- Handle to the instance to load the icon from, we're are passed this by WinMain()
	// MAKEINTRESOURCE(IDI_ICON1) -- Okay the second parameter is a handle to the image to be
	//								 loaded.  If you noticed when we created the icon, a header
	//								 file was also created for you "resource.h".  I added
	//								 this file to the project.  If you look at it, you'll
	//								 notice it has a #define for IDI_ICON1.
	//								 MAKEINTRESOURCE is a macro that takes an integer value
	//								 to a resource type compatible with Win32 
	//								 resource-management functions.  It converts this integer
	//								 into a corresponding "handle name" for the resource
	// IMAGE_ICON -- Specifies the type of image to be loaded
	// 0 -- Width in pixels of image to be loaded
	// 0 -- Height in pixels of image to be loaded
	// LR_DEFAULTSIZE -- Uses the system metric values for icons (also cursors) when loading
	//					 an icon if the width and height (the two parameters right before this)
	//					 are set to zero
	wndclassex.hIcon = (HICON)LoadImage(hinstance,MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,0,0,LR_DEFAULTSIZE);
	
	// Now here is where we do some Win32 trickery!  We are going to set the small icon to be the
	// same icon as the large icon.  Unless you want to use two distinct icons, this solution should
	// do the job.
	wndclassex.hIconSm = wndclassex.hIcon;

	// Register the WNDCLASSEX
    RegisterClassEx(&wndclassex);

	// Create the window
    hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, // Window should have a sunken edge
						  kClassName,
						  "www.GameTutorials.com -- Adding an Icon",
						  WS_OVERLAPPEDWINDOW,
						  CW_USEDEFAULT, // Default upper x-coord of window
						  CW_USEDEFAULT, // Defualt upper y-coord of window
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


/* Iconic Info...

  We have went through and made an icon for our Win32 App.  But like all things in Windows, there
  is more than one way to do any task.  For more information on icons, check out the following
  functions:

  CreateIcon()
  LookupIconIdFromDirectoryEx()
  CreateIconFromResourceEx()

  Questions, comments, criticisms can be posted at www.GameTutorials.com

*/

/*-------------------------*\
|  Programmed by:  TheTutor |
|  ©2000-2005 GameTutorials |
\*-------------------------*/