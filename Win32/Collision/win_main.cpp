/*
	Done by TheTutor

	Collision ---

		Without collision, you limit the types of games you can make -- 
		With the ability to do collision SO many things are possible.  For instance,
		you can have planes that shoot bullets, and when those bullets COLLIDE with other
		planes they can explode.

		So what this tutorial does is show a BASIC, SIMPLE way to do collision.  This is 
		certainly NOT the best way to collision, it's merely an introduction to the 
		behemoth subject named "Collision".

*/

#include "winobj.h"

// Width and height of our squares
#define SQ_WIDTH 50
#define SQ_HEIGHT 50

// Window Globals ***

CWinObj gWinObj; // Main window object

// The upper left corner of our "movable square"
int gXPos = 0;
int gYpos = 0;

bool gRedraw = true; // When this is set to true redraw the window

RECT gSolidRect; // Our non-movable square -- We'll fill this in on the WM_CREATE message

// *** End of Globals 

void DrawSquares(); // This will draw the squares to the screen

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
   
	HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};
	char class_name[] = "GameTutorials_Collision";
	
	// Init the WNDCLASSEX fields we care about **Note WNDCLASSEX is the "newer" version of WNDCLASS
    wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.lpszClassName = class_name;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load default arrow cursor

	// Creating a "extended style window" -- Newer than just RegisterClass()
    RegisterClassEx(&wndclassex);

    hwnd = CreateWindowEx(WS_EX_APPWINDOW, // This is a flag that can give our window different properties
						  class_name,
						  "GameTutorials -- Collision",
						  WS_OVERLAPPED | WS_SYSMENU,	// Window won't be resizable
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
		
	// Init the window object
	if(!gWinObj.init(hwnd))
	{
		MessageBox(hwnd,"Error trying to initialize window object",
				   "**ERROR**",MB_OK | MB_ICONERROR);

			return EXIT_FAILURE; // Fatal error time to leave this app
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
		else if(gRedraw) // Else if we have to redraw -- do so
		{	
			DrawSquares();

			// Blit our back HDC (our buffer we do all drawing to) to our window HDC
			// (basically the screen)
			BitBlt(gWinObj.getHDC(),0,0,gWinObj.getClientWid(),gWinObj.getClientHgt(),
				   gWinObj.getBackHDC(),0,0,SRCCOPY);

			gRedraw = false; // We redrew -- We won't draw any more 'till we have to
		}

	} // end of while(1)

	UnregisterClass(class_name,hinstance); // Unregister the WNDCLASSEX -- You have to unregister 
										  // "extended window classes" to free up memory
		return (int)msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
		
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


		// If the window is moved around, it will stay looking pretty
		case WM_PAINT:
			BeginPaint(hwnd, &ps);
			
			BitBlt(gWinObj.getHDC(),0,0,gWinObj.getClientWid(),gWinObj.getClientHgt(),
				   gWinObj.getBackHDC(),0,0,SRCCOPY);

			EndPaint(hwnd, &ps);
				return 0;

		// This message gets sent when the mouse is moved
		case WM_MOUSEMOVE:
		{
			gRedraw = true; // EVERY TIME THE MOUSE IS MOVED WE REDRAW!!!!!

			// Capture the new position of the cursor
			int tempX = LOWORD(lparam);  // horizontal position of cursor
			int tempY = HIWORD(lparam);  // vertical position of cursor

		

			// Make a temp bounding box (rect) of where the movable square would be
			// if tempX and tempY were it's upper left corner coordinates -- Did you get that?

			RECT intersectRect = {0}; // This is going to be filled with a RECT that
									 // defines the intersection of the two squares -- If they
								    // don't intersect it will still remain empty (all members
								   // will be zero)
									
			RECT tempRect = {0}; // I'm going to create the RECT then fill it with the "current"
							    // bounding box of our "movable" square
			
			tempRect.left = tempX;
			tempRect.top = tempY;
			tempRect.right = tempX + SQ_WIDTH;
			tempRect.bottom = tempY + SQ_HEIGHT;

			// Okay here's the magic function that is going to tell us if
			// COLLISION has happened! -- IntersectRect() will return "true" if a "collision"
			// has occurred -- Now intersectRect (the variable) will be filled with the
			// dimensions of the bounding box that defines the collision area
			// Man was that wordy :)
			// However we really don't care about that (at least not in this tutorial)
			if(IntersectRect(&intersectRect,&gSolidRect,&tempRect))
				return 0; // If there was an intersection (collision) -- simply return
						  // By just returning we won't be updating the "movable square's"
						  // position so it WON'T be able to be moved over top the "solid square"

			// If we get here there WAS NOT a collision so update the "movable square's" 
			// upper left corner coordinates
			gXPos = tempX;
			gYpos = tempY;
				return 0;


		} // end of case WM_MOUSEMOVE:

		case WM_DESTROY:			

			PostQuitMessage(0);
				return 0;
    
	} // end of switch(message)

    return(DefWindowProc(hwnd, message, wparam, lparam)); // We didn't handle the message
														 // So just let the default WinProc()
														// handle the message
}

void DrawSquares()
{
	// A rect that defines our back buffer
	RECT rect = {0,0,gWinObj.getClientWid(),gWinObj.getClientHgt()}; 

	// This fills our back buffer with all white
	FillRect(gWinObj.getBackHDC(),&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

	// Draw a our "non-movable square" in the MIDDLE of the screen
	Rectangle(gWinObj.getBackHDC(),gSolidRect.left, gSolidRect.top,
			  gSolidRect.right, gSolidRect.bottom);

	// Create a solid pen
	HPEN hpen = CreatePen(PS_SOLID,5,RGB(25,250,2));

		// Error Check
		if(!hpen)
			return;
	
	// Select the pen into the back HDC
	HPEN old_pen = (HPEN)SelectObject(gWinObj.getBackHDC(),hpen);

		// Error Check
		if(!old_pen)
		{
			DeleteObject(hpen);
				return;
		}

	// Draw the square that we can move
	Rectangle(gWinObj.getBackHDC(),gXPos,gYpos, gXPos + SQ_WIDTH, gYpos + SQ_HEIGHT);

	// Select back the old_pen
	SelectObject(gWinObj.getBackHDC(),old_pen);

	// Free up hpen
	DeleteObject(hpen);

} // end of DrawSquares()

// EXPANSION PACK --

/*	So how do like collision?  Well I'm sure you've seen that doing it this way sorta sucks.
	If you move the mouse quickly (from left to right say) and you movable square is on the 
	right of "solid object", it could "collide" a distance away from the actual "solid object"

	Also if you move the "movable square" from left to right over the "solid square" you'll notice
	once you pass over the square it kinda just "hops" from one side to the other --

	So how do we get rid of this nasty feature??????

	Well for starters -- If a collision is detected you could set the upper-left coordinate
	of the movable square.  You would use the intersectRect (which we said we didn't care about)
	to calculate how much over "the movable square" is and then set the upper-left corner
	coordinates	of the movable square depending on these amounts.

	Also if we don't do mouse input, but keyboard input instead -- We can move the square pixel
	by pixel in a given direction (up, down, left, right) -- We wouldn't have to worry about
	flying from one side of a solid object to another 'cause we'd only be moving one pixel at
	a time.  If you switch to the keyboard input method, Tetris shouldn't be too big of a stretch :)

	I'll leave that as a challenge to you :)

	But as usual if you're having trouble let us at www.GameTutorials.com know --

	Happy Colliding %)
*/


/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
