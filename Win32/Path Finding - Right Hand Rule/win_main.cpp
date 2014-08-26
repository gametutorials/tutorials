/*
	Done by TheTutor

	Now this tutorial is titled "path finding" but a more fitting name would probably be
	"maze solving" (you'll see what I mean :) -- The path finding technique that is discussed
	is called the "Right Hand Rule" -- This technique works like this:

		1)  First you pick an arbitrary direction to head in (either NORTH, SOUTH, EAST, or WEST)
		2)  If you can TURN RIGHT (ie a open space is to your right, which is obviously dependent
			on what direction you are facing) you turn right AND MOVE FORWARD one space
		3)  If you CAN NOT turn right, move forward in the current direction you are facing
		4)  Keep doing #2 and #3 UNTIL you CAN NOT turn right OR move forward
		5)  Then TURN LEFT and go back to #2
		6)  When you've found your destination, you are done!

	What this does, if you can visualize it, is places a person in a maze with their
	right hand placed on the wall they are next to -- Now no matter how they move, their
	right hand will not come off of the wall, thus ensuring they will eventually go down
	every corridor and valid path of the maze.  Which of course means they will eventually
	find the "exit".

	Now this "path finding" technique certainly WILL NOT find you the quickest OR the shortest
	path, but it will get you to your destination sooner or later.

	It is also important to note that I've sectioned off the "drawing code" and "window code"
	to there own seperate classes.  By now, you are gurus at doing both, so less explanation
	is needed and more comments can be added to the path finiding %)

	Finally, there are some things to consider that WILL BREAK this algorithm that I list at
	the	bottom of this file -- But for now on with the show :)

*/

#include "winobj.h"
#include "path.h"

#define class_name "GT_PathFinding"

// Globals
CWinObj gWinObj; // Main window object
CBitmap gBitmap; // Our double buffering "bitmap"

// Locks the frame rate at 15 frames per second
bool LockFrameRate(int frame_rate = 15);

// Checks for when the "entity" has reached the "end position" --
// Then prompt the user if they want to restart the application
void CheckEndState();

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline, int ishow)
{
    
	HWND hwnd;
    MSG msg;
    WNDCLASSEX wndclassex = {0};

	// Seed the random number generator 
	// **NOTE**  hinstance is always an "arbitrary number"
	srand(GetTickCount() * (int)hinstance);
			
	// Init fields we care about
    wndclassex.cbSize = sizeof(WNDCLASSEX);
    wndclassex.style = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc = WinProc;
    wndclassex.hInstance = hinstance;
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // Make background black
	wndclassex.lpszClassName = class_name;
	wndclassex.hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(IDC_ARROW), IMAGE_CURSOR,
											0, 0, LR_SHARED); // Load the default arrow cursor

    RegisterClassEx(&wndclassex);
    
    DWORD exStyle = WS_EX_APPWINDOW; // Extended window style
    DWORD style = WS_SYSMENU | WS_CAPTION; // Normal window style
    
    // We know we want the client size of our window to be (32*MAP_WIDTH) x (32*MAP_HEIGHT) because
    // our maze is MAP_WIDTH x MAP_HEIGHT blocks in dimension and each block is 32x32 pixels.
    RECT rect = { 0, 0, MAP_WIDTH * 32, MAP_HEIGHT * 32 }; // Desired client rect
    
    // This fucntion adjusts the "rect" so that when we use the adjucsted dimensions
    // to create our window, the client rect of our window will be the dimensions
    // we passed to the function.  So by paramter:
    // &rect -- Desired client dimensions.  When the function returns the window dimensions,
    //		    we must use to get the desired client rect dimensions, will be contained in "rect"
    // style -- The window style to be used to create the window
    // false -- True/False of whether the window will have a menu
    // exStyle -- The extended window style of the window
    AdjustWindowRectEx(&rect, style, false, exStyle); 

    hwnd = CreateWindowEx(exStyle,
						  class_name,
						  "www.GameTutorials.com -- Right Handed Rule",
						  style,
						  CW_USEDEFAULT, // Default upper x-pos for window
						  CW_USEDEFAULT, // Default upper y-pos for window
						  rect.right - rect.left, // Window width
						  rect.bottom - rect.top, // Window height
						  NULL,
						  NULL,
						  hinstance,
						  NULL);

	// Init our WIN_OBJ
	if(!gWinObj.init(hwnd))
		return EXIT_FAILURE;
	
	// Init our double buffering
	// **NOTE** Normally we'd just use the "double buffering" that our CWinObj
	// gives us -- However, our display bitmap is the same dimension as our back
	// buffer and we're going to use it just as we would use our back buffer (ie draw 
	// everything to the gBitmap first) so instead of drawing from the gBitmap to our
	// back buffer to our front buffer, we're going to draw directly from our gBitmap
	// to our front buffer
	if(!gBitmap.setSize(gWinObj.getClientWid(),gWinObj.getClientHgt()))
		return EXIT_FAILURE;
	
	// Init the path finding variables
	if(!PFIND_Init())
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
		else if(LockFrameRate())
		{
			Move();
			
			DrawMap(gBitmap);

			// Draw to screen
			gBitmap.blit(gWinObj.getHDC(),gWinObj.getClientRect());

			CheckEndState();
		}

	} // end of while(1)

	UnregisterClass(class_name,hinstance); // Unregister the WNDCLASSEX
		return (int)msg.wParam;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
		
	switch(message)
    {
		case WM_PAINT:
			
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
				return 0;

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

// Checks to see if we reached the destination point or not
void CheckEndState()
{
	// If we've reached the destination point
	if(DestBeenReached())
	{
		// Prompt user if they want to "do it one more time"
		if(MessageBox(NULL,"One more time???",
					  "We made it!",MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			InitPositions(); // If they answer "Yes" -- Reinit the starting and ending positions
				return;
		}
		else
			PostQuitMessage(0); // Otherwise we're done 
	}

}

/* Path Find To This:

	Now the "right hand rule" really is a great way to "solve a maze" -- It is path finding
	because as you saw, eventually our "entity" reached it's destination, but what the 
	right hand rule is really good for is having enemies in a game say, "keep walking a dungeon"

	The right hand rule breaks down if your maze (your map) has any areas that are 
	circular or loop -- What will happen is the entity will keep traveling in that circular
	path forever, so if you're going to use the right hand rule, your level (or map or whatever)
	MUST NOT have any "circular loops"

	As always questions and comments can be directed to www.GameTutorials.com

*/

/*----------------------------*\
|  TheTutor                    |
|  thetutor@gametutorials.com  |
|  © 2000-2005 GameTutorials   |
\*----------------------------*/

