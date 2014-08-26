// Done by TheTutor

/*
	This program shows one way you can have an "AI character" find and travel the
	shortest path between two "points".

	So no point of beating around the bush, how does one exactly find the shortest
	path between two points?  Well actually it's pretty self-explanatory.  What we
	going to do is start at our starting point (naturally) and build ALL possible
	paths until we reach our destination.  Which ever path leads us to the destination
	first is obviously the shortest one.  Sound horrible inefficient?  Well unfortunately,
	there is no better algorithm (in the worst case) for finding the "shortest path" compared
	to finding all possible paths between two points.

	If you think about it that makes sense.  Because lets say you have 10 paths to 
	get from A to B.  Well the only way you know which one of those 10 is the SHORTEST is to 
	know the "distances" of all 10 paths.  Then you could quickly compare and figure
	out the shortest.  So being we have no idea of how many possible paths to the destination
	there are AND we don't know the distances of any of these possible paths, what we are going
	to do is make and store EVERY possible path until we reach our destination.

	To accomplish this will use a "breadth-first" approach.  The breadth-first search
	is a common pathing algorithm.  It simply works like this.  First we check all the 
	cells connected to our starting cell.  If none of them are the destination, we check all
	the cells connected to the cells connected to our starting cell.  We'll keep doing this
	pattern until we find our destination.  Okay, okay I'll do an ASCII diagram %)

	* = Open Cell
	~ = Visited Cell
	S = Start Cell
	D = Dest Cell

	Fist we have our start and destination cell:
	
	D***
	****
	**S*
	****

	On our FIRST path find attempt in a breath-first search we'd check these 
	cells first (this assumes we can only move north, east, west, and south):
	
	D***		// So the numbers here are in order from checking the surrounding 
	**1*		// cells in clockwise order -- Which direction you start with OR the 
	*4S2		// order you check the surrounding cells really isn't important.  For
	**3*		// this example we'll check NORTH first and travel in a clockwise order.
				

	On our SECOND path find attempt in a breath-first search we'd check these 
	cells first:
	
	D*1*
	*3~2		// Again, the numbers here are in order from checking the surrounding cells 
	*~S~		// in clockwise order -- Notice how we start with the cell that was  
	**~*		// the first cell we checked last time

	On our THIRD path find attempt in a breath-first search we'd check these 
	cells first:
	
	D*~*		// From the #2 cell from our first path find attempt there's only one  
	*~~~		// surrounding cell to check.  So we check that, it's not it and we'd move on
	*~S~		// to the #3 cell from our first path find attempt.  This pattern keeps    
	**~1		// continuing until the destination is found.
				
	That pretty much explains the concept of a breadth-first search in a nutshell.

	So using a breadth-first approach, we'll begin at the starting cell making "paths"
	from the starting cell towards the destination path.  We'll construct every possible
	path, ending a path only when we reach a dead end or the destination.  Whichever
	path we've constructed reaches the destination first is our shortest path. 

	It is important to note, that this does NOT mean that this is the ONLY shortest path.
	There could be multiple	paths that are the same minimum distance to get from "cell A"
	to "cell B" but this algorithm guarantees to give us a shortest path, if a path exists 
	at all.

	Feeling overwhelmed already?  Hopefully not, but if so just keep reading %)
	The toughest thing to grasp with this tutorial (as far as I'm concerned) is the 
	"tricky" STL syntax we use %)

	**NOTE** If you are NOT comfortable with the concepts of a "queue" and a "stack"
			 you should brush up on those concepts before reading this tutorial


	Before we really get started, lets quickly go over the controls for this application:

	Left click the mouse to set a solid square or take away a solid square.
	This allows you to build your own "maze" at runtime.

	Right click the mouse to set your starting position.  Right click the mouse again
	to set your destination position.  
	
	Press the space bar to invoke the "path find" from your start position to your
	destination position.

*/

//#include <time.h>
#include "grid.h"

// Width and height of our window
#define WIN_WID 640
#define WIN_HGT 480

// The upper left corner of where to draw the grid in the window
#define XPOS 50
#define YPOS 50

#define FRAME_RATE 30	// 30 sounds good to me %)

#define class_name "GT_ShortestPath"

// Global
CGrid gGrid; // Our grid
CWinObj gWinObj; // Main window object
int gRbTimes = 0; // Times the right mouse button is pressed.
				 // If this equals 0, set the START cell with the right mouse button
				// If this equals 1, set the DEST cell with the right mouse button

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
											0, 0, LR_SHARED);

    RegisterClassEx(&wndclassex);

	// Create the window
    hwnd = CreateWindowEx(WS_EX_APPWINDOW,
						  class_name,
						  "www.GameTutorials.com -- Shortest Path",
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
			return EXIT_FAILURE;
		
	// Init our CWinObj (sets up double buffering as well)
	if(gWinObj.init(hwnd) == false)
		return EXIT_FAILURE;

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

			// If done drawing, prompt the user to see if they want to do it again
			if(gGrid.doneDrawing())
			{
				if(MessageBox(hwnd,"Do again?","Shortest path completed!!!",
							  MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					gGrid.init();
					gRbTimes = 0;
				}
				else
					break; // We're done with the app
			}

		} // end of else if(LockFrameRate(FRAME_RATE))

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

			// If the space button is pressed
			if(wparam == VK_SPACE)
			{
				// If they haven't set the start and destination positions
				// we can't path find yet
				if(gRbTimes < 1)
					return 0;

				// If you try to path find on a grid without a valid path, give
				// a message to that effect and quit the app
				if(gGrid.pathFind() == false)
				{
					MessageBox(hwnd,"There wasn't a valid path","Good Bye",MB_OK);
					SendMessage(hwnd, WM_CLOSE, 0, 0);
				}
			}

			return 0;

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

			switch(gRbTimes)
			{
				case 0: // First press -- Set the "start position" for the path find

					gGrid.setCellVal(x_pos,y_pos,START);
						break;

				case 1: // Second press -- Set the "destination position" for the path find

					// Can't set the destination on the starting position
					if(gGrid.getCellVal(x_pos, y_pos) == START)
						return 0;

					gGrid.setCellVal(x_pos,y_pos,DEST);
						break;

			}
			
			gRbTimes++; // Increment times right mouse button has been clicked
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



/*	CHALLENGE:

	Change the path finding so you can travel in 8 directions (N, NW, NE, E, SE, SW, W)
	
	/////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////
	
	Well that's how you find the shortest path (you did already read the whole tutorial right?)
	Finding the shortest path can be extremely handy and useful depending on the game you
	are creating, however, don't expect to look at any Quake code and see this algorithm used.
	For	games that already chew up alot of processing power just making things look	pretty, 
	path points system(s) are usually the way to go (a path point system is where
	an AI bot follows predetermined paths).

	As always, any questions or quandries, post 'em at www.gametutorials.com

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/
