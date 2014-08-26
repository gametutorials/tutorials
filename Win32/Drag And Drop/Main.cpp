//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	digiben@gametutorials.com		 //
//																		 //
//		$Program:		DragAndDrop										 //
//																		 //
//		$Description:	Recognizes files that are drag and dropped		 //
//																		 //
//***********************************************************************//

#include <iostream>			// Include the standard C++ file
#include <windows.h>		// We need to include windows.h
using namespace std;		// Include the standard namespace															
										

/////// * /////////// * /////////// * NEW * /////// * /////////// * /////////// *
//
// This tutorial shows you how to add functionality to Win32 programs so
// that the user can drag and drop files onto your applications.  Just to
// simplify the code as much as possible we will have this tutorial just
// print out the file names that were drag onto the application in a message
// box.
// 
// The basic things that you need in order to allow drag and dropped files is
// the following:
//
// 1) "CreateWindowEx()" instead of "CreateWindow()"
// 2) Add WS_EX_ACCEPTFILES as the "Ex" style
// 3) Handle the WM_DROPFILES windows message in the WndProc()
// 4) Convert the wParam from the WM_DROPFILES message to an HDROP
// 5) Call DragQueryFile() to get how many files were drag and dropped
// 6) Call DragQueryFile() for as many files were drop to get each file name
// 7) Call DragFinish() to destroy the extra allocated memory
//
// That it!  Simple huh?  Most professional applications should be able to
// handle this functionality so be sure to add this to your programming tool
// belt.  
//
// The code for this tutorial was taken primarily from the base code of the 
// "Animation" tutorial, but has been stripped down considerably so there will
// be no "NEW" tags.
//
// Check out the simple code below for more explanation.
//
//


// Create a define for our window height
#define HEIGHT 480											
// Create a define for our window width
#define WIDTH  640							


// This function takes in a wParam from the WM_DROPFILES message and 
// prints all the files to a message box.
void HandleFiles(WPARAM wParam)
{
	// DragQueryFile() takes a LPWSTR for the name so we need a TCHAR string
	TCHAR szName[MAX_PATH];

	// Here we cast the wParam as a HDROP handle to pass into the next functions
	HDROP hDrop = (HDROP)wParam;

	// This functions has a couple functionalities.  If you pass in 0xFFFFFFFF in
	// the second parameter then it returns the count of how many filers were drag
	// and dropped.  Otherwise, the function fills in the szName string array with
	// the current file being queried.
	int count = DragQueryFile(hDrop, 0xFFFFFFFF, szName, MAX_PATH);

	// Here we go through all the files that were drag and dropped then display them
	for(int i = 0; i < count; i++)
	{
		// Grab the name of the file associated with index "i" in the list of files dropped.
		// Be sure you know that the name is attached to the FULL path of the file.
		DragQueryFile(hDrop, i, szName, MAX_PATH);

		// Bring up a message box that displays the current file being processed
		MessageBox(GetForegroundWindow(), szName, "Current file received", MB_OK);
	}

	// Finally, we destroy the HDROP handle so the extra memory
	// allocated by the application is released.
	DragFinish(hDrop);
}


// Here is the WndProc that handles all the messages															
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// Check which message was passed in
    switch (iMsg)											
    {
		// This is the message given when people drag files to the window
		case WM_DROPFILES:
			
			// Call our function we created to display all the files.
			// We pass the wParam because it's the HDROP handle.
			HandleFiles(wParam);
			break;

		// This message is sent when the window is destroyed
		case WM_DESTROY:									

			// Tell our main program loop to quit
			PostQuitMessage(0);									
			break;				
	}													

	// Process/handle the default messages
	return DefWindowProc (hwnd, iMsg, wParam, lParam);		
															
}


// This creates our window and handles our main loop
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)			
{														
	// Create the handle to the window.  
    HWND        hwnd;										
	// Create the variable to hold the window messages
    MSG         msg;	
	
	// Create the structure that holds the attributes of the window.
    WNDCLASSEX  wndclass = {sizeof (wndclass), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInstance,
						  LoadIcon (NULL, IDI_WINLOGO), LoadCursor (NULL, IDC_ARROW), (HBRUSH) GetStockObject (WHITE_BRUSH),
						  NULL, "GameTutorials", LoadIcon (NULL, IDI_WINLOGO)};
	

	// Register the window class with the operating system
	RegisterClassEx (&wndclass);	

	// This is the style that we need to add to our CreateWindowEx() function to accept files
	DWORD dwStyleEx = WS_EX_ACCEPTFILES;
	
	// Now, we actually create the window
    hwnd = CreateWindowEx(dwStyleEx,						// The style for accepting files
						 "GameTutorials",					// Window class name 
						 "DragAndDrop - Drag files to me!",	// Window's Title    
						 WS_OVERLAPPEDWINDOW,				// Window style		 
						 CW_USEDEFAULT,						// initial x position
						 CW_USEDEFAULT,						// initial y position
						 WIDTH,								// Here we pass in our desired width (800)	 
						 HEIGHT,						    // Here we pass in our desired height (600)	 
						 NULL,								// This is the parent window handle.  
						 NULL,								// This is the window menu handle
						 hInstance,						    // This is the programs instance handle.
						 NULL);								// We don't want to pass any extra data in, so NULL

	// This shows our window. 
    ShowWindow (hwnd, iCmdShow);							

	// This pretty much paints our window to the screen.
    UpdateWindow (hwnd);									

	// Here is our main loop. 
	while (1)					
    {	
		// Check if there is a window message and remove it from the queue
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If the message was to quit, break out of the while loop
			if(msg.message == WM_QUIT) break;

			// This Translates messages so windows understands them.
			TranslateMessage (&msg);					
		
			// This sends the messages to the WndProc().
			DispatchMessage (&msg);							
		}
		else
		{
			Sleep(1);
		}
    }

	// Unregister the window class with the operating system
	UnregisterClass("GameTutorials",hInstance);
    
	// Quit the program
	return (int)msg.wParam ;										
}