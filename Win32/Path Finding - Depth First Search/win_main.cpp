/*
	Done by TheTutor

	This tutorial is all about path finding, specifically the depth-first search path finding
	method.  It is important to point out right from the get go that this is a VERY simplistic
	application.  Additionally, this is NOT a shortest path finding routine, however, it does
	ALWAYS path find from point A to point B (assuming a valid path exists)

	Here's how this program works:

	By left clicking the mouse you can set a solid square (or take away a solid square) on 
	the grid in the window.  This allows you to build your own "maze" at runtime.

	Right clicking works like this:  The first right mouse button click sets your starting
	position.  The second right mouse button click sets your destination position.  
	
	And last but not least, pressing the space bar will invoke the "path find" from your
	start position to your destination position.  Piece of cake right?

	In the meantime, lets get rolling --
*/

#include "grid.h"

// Width and height of our window
#define WIN_WID 640
#define WIN_HGT 480

#define FRAME_RATE 30 // We want 30 frames a second

// The upper left corner of where to draw the grid in the window
#define XPOS 50
#define YPOS 50

#define class_name "GameTutorials_DFS"

// Global Data ***
CGrid gGrid; // Our grid
CWinObj gWinObj; // Main window object
int gRbPresses = 0; // Counts the number of times the right mouse button has been pressed
// *** End Global Data

// Returns true when drawing should occur -- false otherwise
bool LockFrameRate(int desired_frame_rate);

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
  
	HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	
	// Init the fields we care about			
    wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.lpszClassName = class_name;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load the default arrow cursor

    RegisterClassEx(&wndclassex);

	// Create the window
    hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  class_name,
						  "www.GameTutorials.com -- Depth-First Search",
						  WS_OVERLAPPED | WS_SYSMENU, // Window won't be resizable
						  CW_USEDEFAULT, 
						  CW_USEDEFAULT, 
						  WIN_WID,
						  WIN_HGT,
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

	// Init our window object
	if(gWinObj.init(hwnd) == false)
	{
		MessageBox(hwnd,"Failure creating window object","**ERROR**",MB_OK | MB_ICONERROR);
		UnregisterClass(class_name,hinstance);
			return EXIT_FAILURE;
	}

	// Show and update window
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);
	
	while(1)
	{
		// Checking for window messages -- If we get one we'll deal with it
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				break;
				
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else if(LockFrameRate(FRAME_RATE))	// Otherwise we'll redraw the screen if we have to
		{	
			// Fill the window to black
			FillRect(gWinObj.getBackHDC(),&(gWinObj.getClientRect()),
					 (HBRUSH)GetStockObject(BLACK_BRUSH));

			gGrid.draw(gWinObj.getBackHDC(),XPOS,YPOS);

			// Blit the back buffer to the front buffer
			BitBlt(gWinObj.getHDC(),0,0,gWinObj.getClientWid(),gWinObj.getClientHgt(),
				   gWinObj.getBackHDC(),0,0,SRCCOPY);

			// If we done drawing the path from the start location (cell) to the destination
			// location (cell), prompt the user if they want to path find again
			if(gGrid.doneDrawing())
			{
				if(MessageBox(hwnd,"Do again?","Depth-First Search Completed!!!",
							  MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					gGrid.init();
					gRbPresses = 0;
				}
				else
					break; // We're done with the app
			}
		}

	} // end of while(1)

	UnregisterClass(class_name,hinstance); // Unregister the WNDCLASSEX
		return msg.wParam;
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
		
	switch(message)
    {
		case WM_PAINT:									
			
			BeginPaint(hwnd, &ps);						
			EndPaint(hwnd, &ps);							
				return 0;

		case WM_KEYDOWN:

		// If the space bar is pressed
		if(wparam == VK_SPACE)
		{
			// If they haven't set the start and destination positions
			// we can't path find yet
			if(gRbPresses < 1)
				return 0;

			int x_val;
			int y_val;

			gGrid.getStartPos(x_val,y_val); // First get our starting (x,y)
			gGrid.depthFirstSearch(x_val, y_val); // Path find %)

			// If after the depth-first search we have not found a valid path, print out
			// a message to that effect and quit the app
			if(gGrid.destFound() == false)
			{
				MessageBox(hwnd,"There wasn't a valid path","Good Bye",MB_OK);
				PostQuitMessage(0);
			}
		}

		case WM_LBUTTONDOWN:
		{
			// Can't set new "walls" if we've found the destination
			if(gGrid.destFound())
				return 0;

			int x_pos = LOWORD(lparam);
			int y_pos = HIWORD(lparam);

			// If the x_pos isn't in range, simply return
			if(x_pos < XPOS || (x_pos > XPOS + gGrid.getWidth()))
				return 0;

			// If the y_pos isn't in range, simply return
			if(y_pos < YPOS || (y_pos > YPOS + gGrid.getHeight()))
				return 0;

			// Otherwise we must be clicking in the grid -- 
			x_pos = (x_pos - XPOS) / CCell::width; // x_pos now equals correct "x index"
			y_pos = (y_pos - YPOS) / CCell::height; // y_pos now equals correct "y index"

			if(gGrid.getCellVal(x_pos,y_pos) == SOLID)
				gGrid.setCellVal(x_pos,y_pos,OPEN);

			else if(gGrid.getCellVal(x_pos,y_pos) == OPEN)
				gGrid.setCellVal(x_pos,y_pos,SOLID);
				
			return 0;

		}

		case WM_RBUTTONDOWN:
		{
			int x_pos = LOWORD(lparam);
			int y_pos = HIWORD(lparam);

			// If the x_pos isn't in range, simply return
			if(x_pos < XPOS || (x_pos > XPOS + gGrid.getWidth()))
				return 0;

			// If the y_pos isn't in range, simply return
			if(y_pos < YPOS || (y_pos > YPOS + gGrid.getHeight()))
				return 0;

			// Otherwise we must be clicking in the grid -- 
			x_pos = (x_pos - XPOS) / CCell::width; // x_pos now equals correct "x index"
			y_pos = (y_pos - YPOS) / CCell::height; // y_pos now equals correct "y index"

			// Can't right click on a solid square
			if(gGrid.getCellVal(x_pos,y_pos) == SOLID)
				return 0;

			switch(gRbPresses)
			{
				case 0: // First press -- Set the "start position" for the path find

					gGrid.setCellVal(x_pos,y_pos,START);
						break;

				case 1: // Second press -- Set the "destination position" for the path find

					// Can't set the destination to the starting position
					if(gGrid.getCellVal(x_pos, y_pos) == START)
						return 0;

					gGrid.setCellVal(x_pos,y_pos,DEST);
						break;

			}
			
			gRbPresses++; // Increment times right mouse button has been pressed

			return 0;

		}

		case WM_DESTROY:			
	
			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return(DefWindowProc(hwnd, message, wparam, lparam));
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

// The path ends here...

/*
	If you have any questions be sure to post 'em at www.GameTutorials.com.  To really 
	test your new found skills, try tackling this challenge:

	**CHALLENGE**	

	1)	Let the user click multiple destination spots.  Then path find to the FIRST destination
		spot.  From there path find to the SECOND destination spot, and so on and so forth.
		Also display some sort of visual for the number of ending spots have been entered and
		decrement this counter each time you successfully path find to one of the entered
		destination spots.
*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
