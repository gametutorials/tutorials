/*
	Done by TheTutor

	Collision Part2 ---

		This is basically the same as the first collision tutorial -- The only
		real difference is that we're using keyboard input, instead of mouse input, to 
		do the moving.

		You'll notice that this makes everything smoother -- So now there is no
		excuses for not making a Tetris style game :)

*/

#include "winobj.h"

// Width and height of our squares
#define SQ_WIDTH 50
#define SQ_HEIGHT 50

// Window Globals -----------------------------------------

CWinObj gWinObj; // Main window object

// The upper left corner of our "movable square"
int gSqXpos = 0;
int gSqYpos = 0;

bool gRedraw = true; // When this is set to true gRedraw the window

// The rect (the bounding box) of the "nonmoving, solid" square
RECT gSolidRect;

// End of Globals -----------------------------------------

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

void DrawSquares(); // Draws the squares to the screen

RECT MakeSquareRect(); // This function returns the rect (bounding box) of the movable
					   // square at it's current position

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	char class_name[] = "GameTutorials_Collision";
	
	// Init the WNDCLASSEX fields we care about
	// **Note** WNDCLASSEX is the "newer" version of WNDCLASS
    wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.lpszClassName = class_name;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load the default arrow

	// Creating a "extended style window" -- Newer than just RegisterClass()
    RegisterClassEx(&wndclassex);

    hwnd = CreateWindowEx(NULL, // No extended style properties
						  class_name,
						  "www.GameTutorials.com -- Collision Part2",
						  WS_OVERLAPPED | WS_SYSMENU,	// Window won't be resizable
						  CW_USEDEFAULT,
						  CW_USEDEFAULT,
						  640, // 640 width in pixels of our window
						  480, // 480 height in pixels of our window
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

		// Error Check
		if(!hwnd)
			return EXIT_FAILURE;
		
	if(gWinObj.init(hwnd) == false)
	{
		MessageBox(hwnd,"All hope is lost!","**ERROR**",MB_OK | MB_ICONERROR);
			return EXIT_FAILURE; // Something bad happened
	}

	ShowCursor(false); // This hides the cursor (the arrow)

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
		else if(gRedraw) // Else if we have to gRedraw -- do so
		{	
			DrawSquares();

			// Blit our buffer we do all drawing to to our window's device context
			BitBlt(gWinObj.getHDC(),0,0,gWinObj.getClientWid(),gWinObj.getClientHgt(),
				   gWinObj.getBackHDC(),0,0,SRCCOPY);

			gRedraw = false; // We redrew -- We won't draw any more 'till we have to
		}

	} // end of while(1)

	UnregisterClass(class_name,hinstance); // Unregister the WNDCLASSEX to free up the  
										  // extended window classes memory

		return msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	RECT intersectRect = {0}; // If we're testing for collision -- This rect will get
					         // filled with the bounding region of the collision,
							// if there is one

	switch(message)
    {
		case WM_CREATE:
		{			
			// We want to fill in the dimensions of our solid rect, but to do that
			// we need to know the dimensions of our window.  Our gWinObj won't have
			// been set up yet so what are we going to do?  Well luckily Windows provides
			// a "create struct" that holds a bunch of creation information about the 
			// window
			LPCREATESTRUCT params = (LPCREATESTRUCT)lparam; // Get the window creation parameters
			
			int win_wid = params->cx; // Width of window
			int win_hgt = params->cy; // Height of window

			// Calculate the rect (the bounding box) of the "nonmoving, solid" square
			// The below calculations for the "gSolidRect" place the "non-movable square" CENTERED
			// in our window -- Also it will be exactly TWICE the size of our "movable square"
			// If you don't believe me, do the calculations on paper and prove it to yourself :)
			// In general, you should always be do calculations of ANY significance on paper 
			// first before you attempt to code something -- Trust me it saves SO MUCH TIME!!!
			gSolidRect.left = (win_wid / 2) - SQ_WIDTH;
			gSolidRect.top =  (win_hgt / 2) - SQ_HEIGHT;
			gSolidRect.right = (win_wid / 2) + SQ_WIDTH;
			gSolidRect.bottom = (win_hgt / 2) + SQ_HEIGHT;
				return 0;
		}

		// If the window gets moved around, it will stay looking pretty
		case WM_PAINT:
			BeginPaint(hwnd, &ps);
			
			BitBlt(gWinObj.getHDC(),0,0,gWinObj.getClientWid(),gWinObj.getClientHgt(),
				   gWinObj.getBackHDC(),0,0,SRCCOPY);

			EndPaint(hwnd, &ps);
				return 0;

		// This message gets sent when a key is pushed/held down
		case WM_KEYDOWN:
		{
			RECT tempRect = {0}; // A temp rect for holding the bounding box of the 
						        // moving square

			gRedraw = true; // Every time they push a key we're going to gRedraw
						  // Whether we "truly" have to or not

			// The wparam will equal the "virtual key code" of the key that was pushed
			switch(wparam)
			{
				// The Esc key was pressed
				case VK_ESCAPE:
						
					SendMessage(hwnd, WM_CLOSE, 0, 0); // Terminate program
						break;

				// The left arrow was pressed
				case VK_LEFT:

					gSqXpos -= 5; // Move five pixels to the left

					tempRect = MakeSquareRect(); // Set the tempRect

					// Okay here's the magic function that is going to tell us if
					// COLLISION has happened! -- IntersectRect() will return "true" if a "collision"
					// has occurred -- Now intersectRect (the variable) will be filled with the
					// dimensions of the bounding box that defines the collision area
					// Man was that wordy :)
					// However we really don't care about that (at least not in this tutorial)
					if(IntersectRect(&intersectRect,&gSolidRect,&tempRect))
					{
						// If there was a collision, we have to move the gSqXpos back
						// to where it was (before they collided)
						gSqXpos += 5;
					}

					// Otherwise THEY DIDN'T COLLIDE so we don't need to do anything
					break;

				// The right arrow was pressed
				case VK_RIGHT:

					gSqXpos += 5; // Move five pixels to the right

					tempRect = MakeSquareRect(); // Set the tempRect

					// This is the function that says if we collided or not
					// A long-winded comment is provided for the "VK_LEFT" case statement
					if(IntersectRect(&intersectRect,&gSolidRect,&tempRect))
					{
						// If there was a collision, we have to move the gSqXpos back
						// to where it was (before they collided)
						gSqXpos -= 5;
					}

					// Otherwise THEY DIDN'T COLLIDE so we don't need to do anything
					break;

				// The up arrow was pressed
				case VK_UP:

					gSqYpos -= 5; // Move five pixels up

					tempRect = MakeSquareRect(); // Set the tempRect

					// This is the function that says if we collided or not
					// A long-winded comment is provided for the "VK_LEFT" case statement
					if(IntersectRect(&intersectRect,&gSolidRect,&tempRect))
					{
						// If there was a collision, we have to move the gSqYpos back
						// to where it was (before they collided)
						gSqYpos += 5;
					}

					// Otherwise THEY DIDN'T COLLIDE so we don't need to do anything
					break;

				// The down arrow was pressed
				case VK_DOWN:

					gSqYpos += 5; // Move five pixels down

					tempRect = MakeSquareRect(); // Set the tempRect

					// This is the function that says if we collided or not
					// A long-winded comment is provided for the "VK_LEFT" case statement
					if(IntersectRect(&intersectRect,&gSolidRect,&tempRect))
					{
						// If there was a collision, we have to move the gSqYpos back
						// to where it was (before they collided)
						gSqYpos -= 5;
					}

					// Otherwise THEY DIDN'T COLLIDE so we don't need to do anything
					break;
				
			} // end of switch(wparam)

			return 0;


		} // end of case WM_KEYDOWN:

		case WM_DESTROY:			

			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return(DefWindowProc(hwnd, message, wparam, lparam)); // We didn't handle the message
														 // So just let the defaut WinProc()
														// handle the message
}

// Draws the squares to the screen
void DrawSquares()
{
	// A rect that defines our back buffer
	RECT rect = {0,0,gWinObj.getClientWid(), gWinObj.getClientHgt()}; 

	// This fills our "back_dc" with all white
	FillRect(gWinObj.getBackHDC(),&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));
	

	// Draw a our "non-movable square" in the MIDDLE of the screen
	Rectangle(gWinObj.getBackHDC(),gSolidRect.left, gSolidRect.top,
			  gSolidRect.right, gSolidRect.bottom);

	// Create a solid pen
	HPEN hpen = CreatePen(PS_SOLID,5,RGB(23,230,2));

		// Error Check
		if(!hpen)
			return;
	
	// Select into the back_dc (the device context we draw to)
	HPEN old_pen = (HPEN)SelectObject(gWinObj.getBackHDC(),hpen);

		// Error Check
		if(!old_pen)
		{
			DeleteObject(hpen);
				return;
		}

	// Draw the square that we can move
	Rectangle(gWinObj.getBackHDC(),gSqXpos,gSqYpos, gSqXpos + SQ_WIDTH, gSqYpos + SQ_HEIGHT);

	SelectObject(gWinObj.getBackHDC(),old_pen); // Select back the old_pen
	DeleteObject(hpen); // Free up hpen

} // end of DrawSquares()

// This returns a rect (bounding box) that defines where our movable square currently is
// in the window
RECT MakeSquareRect()
{
	// Create an empty (everything equals zero) rect
	RECT sqRect = {0};
	
	sqRect.left = gSqXpos; // Top left X-coord
	sqRect.top = gSqYpos; // Top left Y-coord
	sqRect.right = gSqXpos + SQ_WIDTH; // Bottom right X-coord
	sqRect.bottom = gSqYpos + SQ_HEIGHT; // Bottom right Y-coord

		return sqRect;
}

// You've collided with the end of the tutorial...

/*
	IntersectRect() is a "tolerable" way to do collision -- It certainly is NOT the 
	best way to do collisions and should only be used if you need VERY SIMPLE collisions.

	With that being said any old school Atari game should be doable using 
	this style of collision detection.  So now you have the collision knowledge to remake
	some of your favorite classics %)

	Questions, comments???  Post 'em at www.GameTutorials.com

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/



