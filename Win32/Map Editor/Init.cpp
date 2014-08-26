//***********************************************************************//
//																		 //
//		- "Talk to me like I'm a 3 year old!" Programming Lessons -		 //
//                                                                       //
//		$Author:		Ben Humphrey	DigiBen@gametutorials.com		 //
//																		 //
//		$Program:		MapEditor1										 //
//																		 //
//		$Description:	A simple tile based map editor					 //
//																		 //
//***********************************************************************//

#include "main.h"

///////////////////////////////// CREATE MY WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates a window, but doesn't have a message loop
/////
///////////////////////////////// CREATE MY WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

HWND CreateMyWindow(LPSTR strWindowName, int width, int height, DWORD dwStyle, HINSTANCE hInstance)
{
	HWND hWnd;
	WNDCLASS wndclass;
	int positionFlag = CW_USEDEFAULT;
	
	memset(&wndclass, 0, sizeof(WNDCLASS));				// Init the size of the class
	wndclass.style = CS_HREDRAW | CS_VREDRAW;			// Regular drawing capabilities
	wndclass.lpfnWndProc = WndProc;						// Pass our function pointer as the window procedure
	wndclass.hInstance = hInstance;						// Assign our hInstance
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);	// General icon
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);		// An arrow for the cursor
	wndclass.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);	// A white window
	wndclass.lpszClassName = "GameTutorials";			// Assign the class name

	wndclass.lpszMenuName = "EditorMenu";

	RegisterClass(&wndclass);							// Register the class
	
	if(!dwStyle)										// Assign styles to the window depending on the choice
		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	g_hInstance = hInstance;							// Assign our global hInstance to the window's hInstance

	RECT rWindow;
	rWindow.left	= 0;								// Set Left Value To 0
	rWindow.right	= width;							// Set Right Value To Requested Width
	rWindow.top	    = 0;								// Set Top Value To 0
	rWindow.bottom	= height;							// Set Bottom Value To Requested Height

	AdjustWindowRect( &rWindow, dwStyle, false);		// Adjust Window To True Requested Size

														// Create the window
	hWnd = CreateWindow("GameTutorials", strWindowName, dwStyle, positionFlag , positionFlag,
						rWindow.right  - rWindow.left, rWindow.bottom - rWindow.top, 
						NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_SHOWNORMAL);					// Show the window
	UpdateWindow(hWnd);									// Draw the window

	SetFocus(hWnd);										// Sets Keymap Focus To The Window	

	return hWnd;
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// CREATE TOOL WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function creates our tool bar window
/////
///////////////////////////////// CREATE TOOL WINDOW \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

HWND CreateToolWindow(HWND hWndParent, LPSTR strWindowName, int width, int height, DWORD dwStyle, HINSTANCE hInstance)
{
	WNDCLASSEX wndclassEx;
	
	memset(&wndclassEx, 0, sizeof(WNDCLASS));				// Init the size of the class
	wndclassEx.cbSize = sizeof(wndclassEx);
	wndclassEx.style = CS_HREDRAW | CS_VREDRAW;				// Regular drawing capabilities
	wndclassEx.lpfnWndProc = WndProcPopup;					// Pass our function pointer as the window procedure
	wndclassEx.hInstance = hInstance;						// Assign our hInstance
	wndclassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);		// General icon
	wndclassEx.hCursor = LoadCursor(NULL, IDC_ARROW);		// An arrow for the cursor
	wndclassEx.hbrBackground = (HBRUSH) (COLOR_WINDOW+1);	// A white window
	wndclassEx.lpszClassName = "ToolWindowClass";			// Assign the class name
	wndclassEx.hIconSm = 0;

	RegisterClassEx(&wndclassEx);

	RECT rWindow;
	rWindow.left	= 0;								// Set Left Value To 0
	rWindow.right	= width;							// Set Right Value To Requested Width
	rWindow.top	    = 0;								// Set Top Value To 0
	rWindow.bottom	= height;							// Set Bottom Value To Requested Height

	AdjustWindowRect( &rWindow, dwStyle, false);		// Adjust Window To True Requested Size

	RECT rParentWindow = {0};							

	GetWindowRect(hWndParent, &rParentWindow);			// Get the window position of the main window
														// Set the tool bar on the right of the main window
	HWND hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, "ToolWindowClass", strWindowName, dwStyle, 
							   rParentWindow.right, rParentWindow.top, rWindow.right  - rWindow.left, 
							   rWindow.bottom - rWindow.top, hWndParent, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_SHOWNORMAL);					// Show the window
	UpdateWindow(hWnd);									// Draw the window

	return hWnd;
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


///////////////////////////////// DE INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function cleans up and then posts a quit message to the window
/////
///////////////////////////////// DE INIT \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void DeInit()
{	


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

	// Here we free all of the HBITMAP's that were loaded in the beginning
	for(int i = 0; i < (int)g_vTiles.size(); i++)
		DeleteObject(g_vTiles[i].GetBitmap());
	for(int i = 0; i < (int)g_vItems.size(); i++)
		DeleteObject(g_vItems[i].GetBitmap());
	for(int i = 0; i < (int)g_vMonsters.size(); i++)
		DeleteObject(g_vMonsters[i].GetBitmap());
	for(int i = 0; i < (int)g_vNpcs.size(); i++)
		DeleteObject(g_vNpcs[i].GetBitmap());

	UnregisterClass("ToolWindowClass", g_hInstance);	// Free the tool bar window class

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


	UnregisterClass("GameTutorials", g_hInstance);		// Free the window class

	PostQuitMessage (0);								// Post a QUIT message to the window
}


//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////

///////////////////////////////// WIN MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This function handles registering and creating the window.
/////
///////////////////////////////// WIN MAIN \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{	
	// Create the main window and offset the height by the menu pixel height
	HWND hWnd = CreateMyWindow("www.GameTutorials.com - MapEditor(1)", 
			    			   SCREEN_WIDTH, SCREEN_HEIGHT + MENU_OFFSET, 0, hInstance);

	// If we never got a valid window handle, quit the program
	if(hWnd == NULL) return TRUE;

	// Create our tool bar window offset by the scroll bar pixel width and to compensate
	// for the small title bar we offset the height with the menu offset
	g_hWndTool = CreateToolWindow(hWnd, "", TILE_WIDTH + SCROLL_BAR_OFFSET, HEIGHT + MENU_OFFSET, 
								  WS_POPUP| WS_SYSMENU | WS_CAPTION , hInstance);

	// Initialize our important things after the window is created
	Init(hWnd);													

	// Run our message loop and after it's done, return the result
	return (int)MainLoop();						
}

//////////// *** NEW *** ////////// *** NEW *** ///////////// *** NEW *** ////////////////////


////////////////////////////////////////////////////////////////////
//
// *Quick Notes*
//  
// In this file we initialize our window information for both the
// main window and the tool bar window.  To make the tool bar work/look
// like it does we use the flags WS_POPUP| WS_SYSMENU | WS_CAPTION.
// 
// We also make sure everything is freed in our DeInit() function.
// 
// 
// Ben Humphrey (DigiBen)
// Game Programmer
// DigiBen@GameTutorials.com
// Co-Web Host of www.GameTutorials.com
//
// © 2000-2005 GameTutorials      